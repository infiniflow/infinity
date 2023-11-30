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
#line 2 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"

#include <stdint.h>
#include <stdio.h>
#include "parser.h"
#include "lexer.h"

void yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg);

#line 87 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"

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
  YYSYMBOL_DATABASE = 26,                  /* DATABASE  */
  YYSYMBOL_TABLE = 27,                     /* TABLE  */
  YYSYMBOL_COLLECTION = 28,                /* COLLECTION  */
  YYSYMBOL_TABLES = 29,                    /* TABLES  */
  YYSYMBOL_INTO = 30,                      /* INTO  */
  YYSYMBOL_VALUES = 31,                    /* VALUES  */
  YYSYMBOL_AST = 32,                       /* AST  */
  YYSYMBOL_PIPELINE = 33,                  /* PIPELINE  */
  YYSYMBOL_RAW = 34,                       /* RAW  */
  YYSYMBOL_LOGICAL = 35,                   /* LOGICAL  */
  YYSYMBOL_PHYSICAL = 36,                  /* PHYSICAL  */
  YYSYMBOL_VIEW = 37,                      /* VIEW  */
  YYSYMBOL_INDEX = 38,                     /* INDEX  */
  YYSYMBOL_ANALYZE = 39,                   /* ANALYZE  */
  YYSYMBOL_VIEWS = 40,                     /* VIEWS  */
  YYSYMBOL_GROUP = 41,                     /* GROUP  */
  YYSYMBOL_BY = 42,                        /* BY  */
  YYSYMBOL_HAVING = 43,                    /* HAVING  */
  YYSYMBOL_AS = 44,                        /* AS  */
  YYSYMBOL_NATURAL = 45,                   /* NATURAL  */
  YYSYMBOL_JOIN = 46,                      /* JOIN  */
  YYSYMBOL_LEFT = 47,                      /* LEFT  */
  YYSYMBOL_RIGHT = 48,                     /* RIGHT  */
  YYSYMBOL_OUTER = 49,                     /* OUTER  */
  YYSYMBOL_FULL = 50,                      /* FULL  */
  YYSYMBOL_ON = 51,                        /* ON  */
  YYSYMBOL_INNER = 52,                     /* INNER  */
  YYSYMBOL_CROSS = 53,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 54,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 55,                     /* WHERE  */
  YYSYMBOL_ORDER = 56,                     /* ORDER  */
  YYSYMBOL_LIMIT = 57,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 58,                    /* OFFSET  */
  YYSYMBOL_ASC = 59,                       /* ASC  */
  YYSYMBOL_DESC = 60,                      /* DESC  */
  YYSYMBOL_IF = 61,                        /* IF  */
  YYSYMBOL_NOT = 62,                       /* NOT  */
  YYSYMBOL_EXISTS = 63,                    /* EXISTS  */
  YYSYMBOL_IN = 64,                        /* IN  */
  YYSYMBOL_FROM = 65,                      /* FROM  */
  YYSYMBOL_TO = 66,                        /* TO  */
  YYSYMBOL_WITH = 67,                      /* WITH  */
  YYSYMBOL_DELIMITER = 68,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 69,                    /* FORMAT  */
  YYSYMBOL_HEADER = 70,                    /* HEADER  */
  YYSYMBOL_CAST = 71,                      /* CAST  */
  YYSYMBOL_END = 72,                       /* END  */
  YYSYMBOL_CASE = 73,                      /* CASE  */
  YYSYMBOL_ELSE = 74,                      /* ELSE  */
  YYSYMBOL_THEN = 75,                      /* THEN  */
  YYSYMBOL_WHEN = 76,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 77,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 78,                   /* INTEGER  */
  YYSYMBOL_INT = 79,                       /* INT  */
  YYSYMBOL_TINYINT = 80,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 81,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 82,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 83,                   /* HUGEINT  */
  YYSYMBOL_CHAR = 84,                      /* CHAR  */
  YYSYMBOL_VARCHAR = 85,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 86,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 87,                    /* DOUBLE  */
  YYSYMBOL_REAL = 88,                      /* REAL  */
  YYSYMBOL_DECIMAL = 89,                   /* DECIMAL  */
  YYSYMBOL_DATE = 90,                      /* DATE  */
  YYSYMBOL_TIME = 91,                      /* TIME  */
  YYSYMBOL_DATETIME = 92,                  /* DATETIME  */
  YYSYMBOL_TIMESTAMP = 93,                 /* TIMESTAMP  */
  YYSYMBOL_UUID = 94,                      /* UUID  */
  YYSYMBOL_POINT = 95,                     /* POINT  */
  YYSYMBOL_LINE = 96,                      /* LINE  */
  YYSYMBOL_LSEG = 97,                      /* LSEG  */
  YYSYMBOL_BOX = 98,                       /* BOX  */
  YYSYMBOL_PATH = 99,                      /* PATH  */
  YYSYMBOL_POLYGON = 100,                  /* POLYGON  */
  YYSYMBOL_CIRCLE = 101,                   /* CIRCLE  */
  YYSYMBOL_BLOB = 102,                     /* BLOB  */
  YYSYMBOL_BITMAP = 103,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 104,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 105,                   /* VECTOR  */
  YYSYMBOL_BIT = 106,                      /* BIT  */
  YYSYMBOL_PRIMARY = 107,                  /* PRIMARY  */
  YYSYMBOL_KEY = 108,                      /* KEY  */
  YYSYMBOL_UNIQUE = 109,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 110,                 /* NULLABLE  */
  YYSYMBOL_IS = 111,                       /* IS  */
  YYSYMBOL_TRUE = 112,                     /* TRUE  */
  YYSYMBOL_FALSE = 113,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 114,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 115,                   /* SECOND  */
  YYSYMBOL_SECONDS = 116,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 117,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 118,                  /* MINUTES  */
  YYSYMBOL_HOUR = 119,                     /* HOUR  */
  YYSYMBOL_HOURS = 120,                    /* HOURS  */
  YYSYMBOL_DAY = 121,                      /* DAY  */
  YYSYMBOL_DAYS = 122,                     /* DAYS  */
  YYSYMBOL_MONTH = 123,                    /* MONTH  */
  YYSYMBOL_MONTHS = 124,                   /* MONTHS  */
  YYSYMBOL_YEAR = 125,                     /* YEAR  */
  YYSYMBOL_YEARS = 126,                    /* YEARS  */
  YYSYMBOL_EQUAL = 127,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 128,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 129,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 130,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 131,                  /* BETWEEN  */
  YYSYMBOL_AND = 132,                      /* AND  */
  YYSYMBOL_OR = 133,                       /* OR  */
  YYSYMBOL_EXTRACT = 134,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 135,                     /* LIKE  */
  YYSYMBOL_DATA = 136,                     /* DATA  */
  YYSYMBOL_LOG = 137,                      /* LOG  */
  YYSYMBOL_BUFFER = 138,                   /* BUFFER  */
  YYSYMBOL_KNN = 139,                      /* KNN  */
  YYSYMBOL_USING = 140,                    /* USING  */
  YYSYMBOL_NUMBER = 141,                   /* NUMBER  */
  YYSYMBOL_142_ = 142,                     /* '='  */
  YYSYMBOL_143_ = 143,                     /* '<'  */
  YYSYMBOL_144_ = 144,                     /* '>'  */
  YYSYMBOL_145_ = 145,                     /* '+'  */
  YYSYMBOL_146_ = 146,                     /* '-'  */
  YYSYMBOL_147_ = 147,                     /* '*'  */
  YYSYMBOL_148_ = 148,                     /* '/'  */
  YYSYMBOL_149_ = 149,                     /* '%'  */
  YYSYMBOL_150_ = 150,                     /* '['  */
  YYSYMBOL_151_ = 151,                     /* ']'  */
  YYSYMBOL_152_ = 152,                     /* '('  */
  YYSYMBOL_153_ = 153,                     /* ')'  */
  YYSYMBOL_154_ = 154,                     /* '.'  */
  YYSYMBOL_155_ = 155,                     /* ';'  */
  YYSYMBOL_156_ = 156,                     /* ','  */
  YYSYMBOL_YYACCEPT = 157,                 /* $accept  */
  YYSYMBOL_input_pattern = 158,            /* input_pattern  */
  YYSYMBOL_statement_list = 159,           /* statement_list  */
  YYSYMBOL_statement = 160,                /* statement  */
  YYSYMBOL_explainable_statement = 161,    /* explainable_statement  */
  YYSYMBOL_create_statement = 162,         /* create_statement  */
  YYSYMBOL_table_element_array = 163,      /* table_element_array  */
  YYSYMBOL_table_element = 164,            /* table_element  */
  YYSYMBOL_table_column = 165,             /* table_column  */
  YYSYMBOL_column_type = 166,              /* column_type  */
  YYSYMBOL_column_constraints = 167,       /* column_constraints  */
  YYSYMBOL_column_constraint = 168,        /* column_constraint  */
  YYSYMBOL_table_constraint = 169,         /* table_constraint  */
  YYSYMBOL_identifier_array = 170,         /* identifier_array  */
  YYSYMBOL_delete_statement = 171,         /* delete_statement  */
  YYSYMBOL_insert_statement = 172,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 173, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 174,        /* explain_statement  */
  YYSYMBOL_explain_type = 175,             /* explain_type  */
  YYSYMBOL_update_statement = 176,         /* update_statement  */
  YYSYMBOL_update_expr_array = 177,        /* update_expr_array  */
  YYSYMBOL_update_expr = 178,              /* update_expr  */
  YYSYMBOL_drop_statement = 179,           /* drop_statement  */
  YYSYMBOL_copy_statement = 180,           /* copy_statement  */
  YYSYMBOL_select_statement = 181,         /* select_statement  */
  YYSYMBOL_select_with_paren = 182,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 183,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 184, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 185, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 186, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 187,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 188,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 189,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 190,            /* order_by_type  */
  YYSYMBOL_limit_expr = 191,               /* limit_expr  */
  YYSYMBOL_offset_expr = 192,              /* offset_expr  */
  YYSYMBOL_distinct = 193,                 /* distinct  */
  YYSYMBOL_from_clause = 194,              /* from_clause  */
  YYSYMBOL_where_clause = 195,             /* where_clause  */
  YYSYMBOL_having_clause = 196,            /* having_clause  */
  YYSYMBOL_group_by_clause = 197,          /* group_by_clause  */
  YYSYMBOL_set_operator = 198,             /* set_operator  */
  YYSYMBOL_table_reference = 199,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 200,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 201,     /* table_reference_name  */
  YYSYMBOL_table_name = 202,               /* table_name  */
  YYSYMBOL_table_alias = 203,              /* table_alias  */
  YYSYMBOL_with_clause = 204,              /* with_clause  */
  YYSYMBOL_with_expr_list = 205,           /* with_expr_list  */
  YYSYMBOL_with_expr = 206,                /* with_expr  */
  YYSYMBOL_join_clause = 207,              /* join_clause  */
  YYSYMBOL_join_type = 208,                /* join_type  */
  YYSYMBOL_show_statement = 209,           /* show_statement  */
  YYSYMBOL_flush_statement = 210,          /* flush_statement  */
  YYSYMBOL_expr_array = 211,               /* expr_array  */
  YYSYMBOL_expr_array_list = 212,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 213,               /* expr_alias  */
  YYSYMBOL_expr = 214,                     /* expr  */
  YYSYMBOL_operand = 215,                  /* operand  */
  YYSYMBOL_knn_expr = 216,                 /* knn_expr  */
  YYSYMBOL_function_expr = 217,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 218,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 219,             /* between_expr  */
  YYSYMBOL_in_expr = 220,                  /* in_expr  */
  YYSYMBOL_case_expr = 221,                /* case_expr  */
  YYSYMBOL_case_check_array = 222,         /* case_check_array  */
  YYSYMBOL_cast_expr = 223,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 224,            /* subquery_expr  */
  YYSYMBOL_column_expr = 225,              /* column_expr  */
  YYSYMBOL_constant_expr = 226,            /* constant_expr  */
  YYSYMBOL_array_expr = 227,               /* array_expr  */
  YYSYMBOL_long_array_expr = 228,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 229, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 230,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 231, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 232,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 233,         /* copy_option_list  */
  YYSYMBOL_copy_option = 234,              /* copy_option  */
  YYSYMBOL_file_path = 235,                /* file_path  */
  YYSYMBOL_if_exists = 236,                /* if_exists  */
  YYSYMBOL_if_not_exists = 237,            /* if_not_exists  */
  YYSYMBOL_semicolon = 238,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 239,       /* if_not_exists_info  */
  YYSYMBOL_with_index_para_list = 240,     /* with_index_para_list  */
  YYSYMBOL_index_para_list = 241,          /* index_para_list  */
  YYSYMBOL_index_para = 242                /* index_para  */
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
#define YYFINAL  63
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   740

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  157
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  86
/* YYNRULES -- Number of rules.  */
#define YYNRULES  299
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  592

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   396


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
       2,     2,     2,     2,     2,     2,     2,   149,     2,     2,
     152,   153,   147,   145,   156,   146,   154,   148,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   155,
     143,   142,   144,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   150,     2,   151,     2,     2,     2,     2,     2,     2,
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
     135,   136,   137,   138,   139,   140,   141
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   444,   444,   448,   454,   461,   462,   463,   464,   465,
     466,   467,   468,   469,   470,   472,   473,   474,   475,   476,
     477,   478,   479,   480,   487,   500,   515,   539,   555,   573,
     599,   603,   609,   612,   618,   657,   695,   696,   697,   698,
     699,   700,   701,   702,   703,   704,   705,   706,   707,   708,
     709,   710,   711,   712,   713,   714,   715,   716,   718,   719,
     720,   721,   722,   723,   724,   725,   726,   727,   728,   729,
     730,   731,   732,   733,   734,   735,   736,   737,   738,   739,
     740,   741,   742,   761,   765,   775,   778,   781,   784,   788,
     793,   800,   806,   816,   832,   844,   857,   860,   867,   873,
     876,   879,   882,   885,   888,   891,   898,   911,   915,   920,
     933,   946,   961,   976,   991,  1014,  1055,  1100,  1103,  1106,
    1115,  1125,  1128,  1132,  1137,  1144,  1147,  1152,  1167,  1170,
    1174,  1178,  1183,  1189,  1192,  1195,  1199,  1203,  1205,  1209,
    1211,  1214,  1218,  1221,  1225,  1228,  1232,  1235,  1239,  1242,
    1246,  1249,  1252,  1255,  1263,  1266,  1281,  1281,  1283,  1297,
    1306,  1313,  1324,  1329,  1334,  1340,  1347,  1350,  1354,  1357,
    1362,  1374,  1381,  1395,  1398,  1401,  1404,  1407,  1410,  1413,
    1419,  1423,  1427,  1438,  1453,  1457,  1461,  1470,  1474,  1479,
    1483,  1510,  1516,  1520,  1521,  1522,  1523,  1524,  1526,  1529,
    1535,  1538,  1539,  1540,  1541,  1542,  1544,  1653,  1661,  1669,
    1678,  1685,  1692,  1699,  1706,  1713,  1721,  1729,  1737,  1745,
    1753,  1761,  1769,  1777,  1785,  1793,  1801,  1809,  1838,  1846,
    1855,  1863,  1872,  1880,  1886,  1893,  1899,  1906,  1911,  1918,
    1925,  1933,  1961,  1967,  1973,  1980,  1988,  1995,  2002,  2007,
    2017,  2022,  2027,  2032,  2037,  2042,  2047,  2050,  2062,  2065,
    2069,  2073,  2078,  2083,  2087,  2092,  2097,  2103,  2109,  2115,
    2121,  2127,  2133,  2139,  2145,  2151,  2157,  2163,  2174,  2178,
    2183,  2202,  2208,  2214,  2218,  2219,  2221,  2222,  2224,  2225,
    2237,  2245,  2250,  2253,  2257,  2261,  2266,  2271,  2279,  2286
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
  "DATABASE", "TABLE", "COLLECTION", "TABLES", "INTO", "VALUES", "AST",
  "PIPELINE", "RAW", "LOGICAL", "PHYSICAL", "VIEW", "INDEX", "ANALYZE",
  "VIEWS", "GROUP", "BY", "HAVING", "AS", "NATURAL", "JOIN", "LEFT",
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
  "NUMBER", "'='", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['",
  "']'", "'('", "')'", "'.'", "';'", "','", "$accept", "input_pattern",
  "statement_list", "statement", "explainable_statement",
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
  "offset_expr", "distinct", "from_clause", "where_clause",
  "having_clause", "group_by_clause", "set_operator", "table_reference",
  "table_reference_unit", "table_reference_name", "table_name",
  "table_alias", "with_clause", "with_expr_list", "with_expr",
  "join_clause", "join_type", "show_statement", "flush_statement",
  "expr_array", "expr_array_list", "expr_alias", "expr", "operand",
  "knn_expr", "function_expr", "conjunction_expr", "between_expr",
  "in_expr", "case_expr", "case_check_array", "cast_expr", "subquery_expr",
  "column_expr", "constant_expr", "array_expr", "long_array_expr",
  "unclosed_long_array_expr", "double_array_expr",
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

#define YYPACT_NINF (-267)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-292)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      51,   172,    40,   203,    75,    15,    75,   363,    10,    50,
     269,    91,   -48,   100,   -39,  -267,  -267,  -267,  -267,  -267,
    -267,  -267,  -267,   261,  -267,  -267,   116,  -267,  -267,    81,
      81,    81,    81,    -5,    75,   108,   108,   108,   108,   108,
      17,   164,    75,   -14,  -267,  -267,  -267,  -267,  -267,  -267,
     150,  -267,  -267,    75,  -267,  -267,  -267,  -267,   145,    39,
    -267,    58,    65,  -267,    20,  -267,   204,  -267,  -267,    -4,
     178,  -267,   198,   166,   249,    75,    75,    75,   253,   213,
     122,   202,   292,    75,    75,    75,   293,   301,   302,   220,
     307,   307,  -267,  -267,  -267,  -267,  -267,  -267,  -267,   261,
    -267,  -267,  -267,   160,    91,  -267,  -267,  -267,  -267,    -4,
    -267,  -267,  -267,   304,   275,   273,   266,  -267,   -38,  -267,
     122,  -267,    75,   331,   -10,  -267,  -267,  -267,  -267,  -267,
     295,  -267,   206,   -47,  -267,   304,  -267,  -267,   294,   303,
     116,  -267,   216,   230,   238,  -267,  -267,   569,   368,   239,
     248,   210,   376,  -267,  -267,   395,   260,   263,   456,   456,
    -267,   274,   -53,  -267,    35,   412,  -267,  -267,  -267,  -267,
    -267,  -267,  -267,  -267,   271,  -267,  -267,   304,   304,   356,
    -267,   -48,     8,   383,   276,  -267,  -112,   277,  -267,    75,
     304,   302,  -267,  -108,   280,   281,   282,  -267,  -267,   180,
    -267,  -267,  -267,  -267,  -267,  -267,  -267,  -267,  -267,  -267,
    -267,  -267,   456,   284,   437,   367,   304,   304,   -27,   174,
    -267,   569,  -267,   433,   304,   262,   262,   287,    44,     4,
     304,   220,   439,   304,   304,   -51,   296,   -24,   456,   456,
     456,   456,   456,   456,   456,   456,   456,   456,   456,   456,
     456,   456,     2,   288,  -267,   -37,  -108,   304,  -267,   261,
     578,   337,   300,   -63,  -267,  -267,  -267,   -48,   331,  -267,
     443,   304,   291,  -267,  -108,  -267,   354,   354,  -267,   304,
    -267,   -45,   367,   319,   311,    41,   -41,   227,  -267,   304,
     304,   390,    32,  -267,  -267,   -48,   309,   485,  -267,    25,
    -267,  -267,   416,  -267,  -267,   334,   315,   456,   274,   353,
    -267,   505,   505,    76,    76,   482,   505,   505,    76,    76,
     262,   262,  -267,  -267,  -267,  -267,  -267,   304,  -267,  -267,
    -267,  -108,  -267,  -267,  -267,  -267,  -267,  -267,  -267,   316,
     321,  -267,  -267,  -267,   323,  -267,  -267,  -267,  -267,  -267,
    -267,  -267,  -267,  -267,  -267,  -267,  -267,   326,   327,   340,
     341,    84,   344,   331,  -267,     8,   261,    26,  -267,   113,
     345,   479,   491,  -267,   165,  -267,   175,   182,  -267,   347,
    -267,   578,   304,  -267,   304,   -13,    48,   456,   348,    -6,
       4,   455,  -267,  -267,  -267,  -267,  -267,  -267,   457,  -267,
     501,  -267,   463,   465,   274,   505,   357,   186,  -267,   456,
    -267,   500,   503,   506,   510,   511,   181,   272,   401,   411,
    -267,  -267,    84,  -267,   331,   187,  -267,   381,  -267,   304,
    -267,  -267,  -267,   354,  -267,  -267,  -267,   369,  -108,    -7,
    -267,   304,   342,   196,   370,  -267,   -83,  -267,    56,    25,
     485,     4,     4,   372,   304,   304,  -267,   375,   207,  -267,
    -267,   437,   392,   396,   212,   397,   398,   380,   406,   407,
     415,   417,   418,   419,   420,   421,   422,   431,   432,   435,
     436,   438,   440,  -267,  -267,  -267,   223,  -267,   522,   228,
    -267,  -267,  -267,  -108,  -267,  -267,  -267,   548,  -267,   547,
    -267,   584,  -267,  -267,   546,   331,   442,  -108,  -267,  -267,
    -267,  -267,  -267,   593,  -267,  -267,   594,   598,   599,   600,
     601,   607,   609,   612,   613,   614,   615,   616,   617,   630,
     631,   632,  -267,   572,  -267,   486,  -267,  -267,   304,   229,
     488,   490,   492,   493,   543,   544,   545,   549,   550,   551,
     552,   553,   554,   555,   556,   557,   558,   560,  -267,   640,
    -108,  -267,  -267,  -267,  -267,  -267,  -267,  -267,  -267,  -267,
    -267,  -267,  -267,  -267,  -267,  -267,  -267,  -267,  -267,   696,
     561,   559,   236,  -267,  -267,   310,  -267,   696,  -267,  -267,
    -267,  -267
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     167,     0,     0,     0,     0,     0,     0,   105,     0,     0,
       0,     0,   167,     0,   289,     3,     5,    10,    12,    13,
      11,     6,     7,     9,   118,   117,     0,     8,    14,   287,
     287,   287,   287,   287,     0,   285,   285,   285,   285,   285,
     160,     0,     0,     0,   100,   104,   101,   102,   103,    99,
     167,   180,   181,     0,   182,   184,   185,   186,     0,   166,
     168,     0,     0,     1,   167,     2,   150,   152,   153,     0,
     141,   123,   129,     0,     0,     0,     0,     0,     0,     0,
      97,     0,     0,     0,     0,     0,     0,     0,     0,   145,
       0,     0,    98,    15,    20,    22,    21,    16,    17,    19,
      18,    23,   183,     0,     0,   122,   121,     4,   151,     0,
     119,   120,   140,     0,     0,   137,     0,    24,     0,    25,
      97,   290,     0,     0,   167,   284,   110,   112,   111,   113,
       0,   161,     0,   145,   107,     0,    93,   283,     0,     0,
       0,   169,     0,     0,   246,   250,   253,   254,     0,     0,
       0,     0,     0,   251,   252,     0,     0,     0,     0,     0,
     248,   167,   143,   187,   192,   193,   205,   202,   197,   196,
     195,   203,   204,   194,   201,   200,   257,     0,     0,   139,
     286,   167,     0,     0,     0,    91,     0,     0,    95,     0,
       0,     0,   106,   144,     0,     0,     0,   126,   125,     0,
     267,   266,   269,   268,   271,   270,   273,   272,   275,   274,
     277,   276,     0,     0,   212,   167,     0,     0,     0,     0,
     255,     0,   256,     0,     0,   214,   213,     0,     0,     0,
       0,   145,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   128,   130,   135,   136,     0,   124,    27,
       0,     0,     0,     0,    30,    32,    33,   167,     0,    96,
       0,     0,    94,   114,   109,   108,     0,     0,   170,     0,
     207,     0,   167,     0,     0,     0,     0,     0,   237,     0,
       0,     0,     0,   199,   198,   167,   142,   154,   156,   165,
     157,   188,   149,   191,   230,   231,     0,     0,   167,     0,
     211,   221,   222,   225,   226,     0,   228,   220,   223,   224,
     216,   215,   217,   218,   219,   247,   249,     0,   133,   134,
     132,   138,    36,    39,    40,    37,    38,    41,    42,    58,
      60,    43,    45,    44,    64,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,     0,     0,     0,
       0,    34,     0,     0,    26,     0,    28,     0,    92,     0,
       0,     0,     0,   282,     0,   278,     0,     0,   208,     0,
     242,     0,     0,   235,     0,     0,     0,     0,     0,     0,
       0,     0,   174,   175,   176,   177,   173,   178,     0,   163,
       0,   158,     0,   147,   167,   229,     0,     0,   210,     0,
     131,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      86,    87,    35,    83,     0,     0,    31,     0,   189,     0,
     281,   280,   116,     0,   115,   209,   243,     0,   239,     0,
     238,     0,     0,     0,     0,   258,     0,   259,     0,   165,
     155,     0,     0,   162,     0,     0,   127,     0,     0,   244,
     233,   232,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    88,    85,    84,     0,    90,     0,     0,
     279,   241,   236,   240,   227,   264,   261,     0,   260,     0,
     263,     0,   159,   171,     0,     0,   148,   146,   245,   234,
      59,    61,    63,     0,    65,    66,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    89,   293,   190,     0,   262,   265,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,     0,
     172,   164,    62,    70,    71,    68,    69,    72,    73,    74,
      67,    78,    79,    76,    77,    80,    81,    82,    75,     0,
       0,   296,     0,   294,   206,     0,   292,     0,   297,   299,
     298,   295
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -267,  -267,  -267,   636,  -267,   663,  -267,   350,  -267,   335,
    -267,   297,  -267,  -266,   667,   668,   602,  -267,  -267,   670,
    -267,   530,   673,   674,   -40,   713,   -11,   586,   618,   -55,
    -267,  -267,   402,  -267,  -267,  -267,  -267,  -267,  -124,  -267,
    -267,  -267,  -267,   338,  -207,    68,   283,  -267,  -267,   626,
    -267,  -267,   681,   683,  -196,  -267,   504,  -135,  -111,  -267,
    -267,  -267,  -267,  -267,  -267,   517,  -267,  -267,  -267,  -267,
    -267,  -267,  -267,  -267,  -267,   581,   460,   305,   648,   199,
     258,  -267,  -267,  -267,  -267,   153
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    13,    14,    15,    92,    16,   263,   264,   265,   361,
     422,   423,   266,   186,    17,    18,   124,    19,    50,    20,
     133,   134,    21,    22,    23,    24,    25,    71,   110,    72,
     115,   253,   254,   330,   179,   258,   113,   231,   136,   456,
     403,    69,   296,   297,   298,   299,   401,    26,    59,    60,
     300,   398,    27,    28,   162,   272,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   219,   172,   173,   174,   175,
     444,   445,   446,   447,   448,   176,   374,   375,   138,    82,
      74,    65,    79,   558,   582,   583
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     193,    62,   367,   281,    70,   325,   181,    40,   135,   192,
      99,   260,   229,   306,   111,    66,   218,    67,    68,    11,
    -288,   187,   328,   329,   233,   234,   228,     1,   399,     2,
       3,     4,     5,     6,   382,     7,     8,   214,   309,    51,
       9,   269,   255,   256,   270,    10,  -291,   225,   226,   217,
      52,    90,    91,    40,   143,   274,    73,    11,     1,   440,
       2,     3,     4,     5,     6,   492,     7,     8,   498,   400,
      34,     9,    41,   499,    43,   369,    10,    54,    40,   232,
      42,   285,   286,   377,   307,   381,   310,    11,    53,   292,
     364,   233,   234,   365,    58,   233,   234,   425,   304,   305,
      63,   214,    80,   230,    12,   233,   234,   302,   378,   191,
      89,   230,   407,   188,   182,   261,    64,   262,    11,   233,
     234,   102,   331,   441,    70,   233,   234,   311,   312,   313,
     314,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     324,   259,    73,   118,   119,   120,   418,   449,   109,   326,
     227,   127,   128,   129,   385,   386,   295,     1,   486,     2,
       3,     4,     5,     6,   233,   234,     8,   233,   234,    81,
       9,    87,    12,   233,   234,    10,   233,   234,    88,   427,
     233,   234,   270,   144,   145,   146,   147,   237,   388,   103,
     184,   419,   255,   420,   421,   104,   405,   294,    29,    30,
      31,   495,   496,    12,   284,  -292,  -292,   500,   458,    32,
      33,   105,   501,   144,   145,   146,   147,    11,   106,  -292,
    -292,   247,   248,   249,   250,   251,   108,   366,   116,    35,
      36,    37,   112,   489,   279,    83,    84,    85,    86,   539,
      38,    39,   148,   149,   503,   504,   288,   438,   289,   439,
     290,   150,   117,   151,   114,   389,   121,   273,   506,   467,
     468,   469,   470,   471,   122,   125,   428,   472,   473,   230,
     152,   379,   148,   149,   123,   135,   442,   144,   145,   146,
     147,   150,    66,   151,    67,    68,   217,   474,    75,    76,
      77,    78,   153,   154,   155,   126,   130,   406,   461,   383,
     152,   384,    12,   290,   131,   132,   493,   144,   145,   146,
     147,   137,   140,   588,   156,   589,   590,   177,   432,   157,
     507,   433,   153,   154,   155,   158,   159,   160,   434,   180,
     178,   433,   161,   280,   185,   435,   148,   149,   230,   460,
     487,    11,   230,   270,   156,   150,   189,   151,   190,   157,
     475,   476,   477,   478,   479,   158,   159,   160,   480,   481,
     509,   194,   161,   230,   152,   512,   148,   149,   513,   197,
     195,   144,   145,   146,   147,   150,   532,   151,   482,   270,
     220,   534,   561,   198,   230,   270,   153,   154,   155,   586,
     199,   215,   587,   457,   152,    44,    45,    46,    47,    48,
     216,   221,    49,   560,   283,    55,    56,    57,   156,   249,
     250,   251,   223,   157,   257,   224,   153,   154,   155,   158,
     159,   160,   371,   372,   373,   252,   161,   267,   268,   271,
     212,   213,   276,   277,    11,   278,   282,   291,   156,   150,
     293,   151,   303,   157,   327,   362,   368,   370,   308,   158,
     159,   160,   363,   237,   307,   387,   161,   402,   152,   144,
     145,   146,   147,   408,   380,   390,   233,   404,   411,   238,
     239,   240,   241,   412,   235,   413,   236,   243,   414,   415,
     153,   154,   155,   430,   244,   245,   246,   247,   248,   249,
     250,   251,   416,   417,   431,   494,   424,   429,   443,   283,
     436,   451,   156,   452,   453,   454,   462,   157,   455,   463,
     459,   483,   464,   158,   159,   160,   465,   466,   212,   484,
     161,   488,   491,   237,   505,   533,   497,   150,   508,   151,
     391,  -179,   392,   393,   394,   395,   516,   396,   397,   238,
     239,   240,   241,   242,   283,   510,   152,   243,   237,   511,
     514,   515,   535,   536,   244,   245,   246,   247,   248,   249,
     250,   251,   517,   518,   238,   239,   240,   241,   153,   154,
     155,   519,   243,   520,   521,   522,   523,   524,   525,   244,
     245,   246,   247,   248,   249,   250,   251,   526,   527,   537,
     156,   528,   529,   237,   530,   157,   531,   538,   230,   540,
     541,   158,   159,   160,   542,   543,   544,   545,   161,   238,
     239,   240,   241,   546,   409,   547,   237,   243,   548,   549,
     550,   551,   552,   553,   244,   245,   246,   247,   248,   249,
     250,   251,  -292,  -292,   240,   241,   554,   555,   556,   557,
    -292,   562,   559,   563,   580,   564,   565,  -292,   245,   246,
     247,   248,   249,   250,   251,   332,   333,   334,   335,   336,
     337,   338,   339,   340,   341,   342,   343,   344,   345,   346,
     347,   348,   349,   350,   351,   352,   353,   354,   355,   356,
     357,   358,   359,   360,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   566,   567,   568,   581,
     107,   585,   569,   570,   571,   572,   573,   574,   575,   576,
     577,   578,   579,    93,   584,   426,   437,    94,    95,   485,
      96,   275,   183,    97,    98,    61,   196,   142,   450,   410,
     141,   100,   502,   101,   301,   287,   222,   376,   490,   139,
     591
};

static const yytype_int16 yycheck[] =
{
     135,    12,   268,   199,     8,     3,    44,     3,    55,   133,
      50,     3,    65,    64,    69,    21,   151,    23,    24,    67,
       0,    31,    59,    60,   132,   133,   161,     7,     3,     9,
      10,    11,    12,    13,    75,    15,    16,   148,    62,    29,
      20,   153,   177,   178,   156,    25,    51,   158,   159,    76,
      40,    65,    66,     3,   109,   190,    61,    67,     7,    72,
       9,    10,    11,    12,    13,    72,    15,    16,   151,    44,
      30,    20,     4,   156,     6,   271,    25,     9,     3,    44,
      65,   216,   217,   279,   135,    44,   110,    67,    38,   224,
     153,   132,   133,   156,     3,   132,   133,   363,   233,   234,
       0,   212,    34,   156,   152,   132,   133,   231,   153,   156,
      42,   156,   308,   124,   152,   107,   155,   109,    67,   132,
     133,    53,   257,    75,     8,   132,   133,   238,   239,   240,
     241,   242,   243,   244,   245,   246,   247,   248,   249,   250,
     251,   181,    61,    75,    76,    77,    62,   153,   152,   147,
     161,    83,    84,    85,   289,   290,   152,     7,   424,     9,
      10,    11,    12,    13,   132,   133,    16,   132,   133,    61,
      20,   154,   152,   132,   133,    25,   132,   133,    14,   153,
     132,   133,   156,     3,     4,     5,     6,   111,   156,    44,
     122,   107,   327,   109,   110,   156,   307,   153,    26,    27,
      28,     5,     6,   152,   215,   129,   130,   151,   404,    37,
      38,   153,   156,     3,     4,     5,     6,    67,   153,   143,
     144,   145,   146,   147,   148,   149,    22,   267,    62,    26,
      27,    28,    54,   429,    54,    36,    37,    38,    39,   505,
      37,    38,    62,    63,   451,   452,    72,   382,    74,   384,
      76,    71,     3,    73,    56,   295,     3,   189,   454,    78,
      79,    80,    81,    82,    51,    63,   153,    86,    87,   156,
      90,   282,    62,    63,   152,    55,   387,     3,     4,     5,
       6,    71,    21,    73,    23,    24,    76,   106,    30,    31,
      32,    33,   112,   113,   114,     3,     3,   308,   409,    72,
      90,    74,   152,    76,     3,     3,   441,     3,     4,     5,
       6,     4,   152,     3,   134,     5,     6,    42,   153,   139,
     455,   156,   112,   113,   114,   145,   146,   147,   153,    63,
      57,   156,   152,   153,     3,   153,    62,    63,   156,   153,
     153,    67,   156,   156,   134,    71,    51,    73,   142,   139,
      78,    79,    80,    81,    82,   145,   146,   147,    86,    87,
     153,    67,   152,   156,    90,   153,    62,    63,   156,   153,
      67,     3,     4,     5,     6,    71,   153,    73,   106,   156,
       4,   153,   153,   153,   156,   156,   112,   113,   114,   153,
     152,   152,   156,   404,    90,    32,    33,    34,    35,    36,
     152,     6,    39,   538,    62,   136,   137,   138,   134,   147,
     148,   149,   152,   139,    58,   152,   112,   113,   114,   145,
     146,   147,    68,    69,    70,   154,   152,    44,   152,   152,
      62,    63,   152,   152,    67,   153,   152,     4,   134,    71,
     153,    73,     3,   139,   156,   108,     3,   156,   152,   145,
     146,   147,   152,   111,   135,    65,   152,    41,    90,     3,
       4,     5,     6,   110,   153,   156,   132,   152,   152,   127,
     128,   129,   130,   152,    62,   152,    64,   135,   152,   152,
     112,   113,   114,     4,   142,   143,   144,   145,   146,   147,
     148,   149,   152,   152,     3,   153,   152,   152,   150,    62,
     153,    46,   134,    46,     3,    42,     6,   139,    43,     6,
     153,   110,     6,   145,   146,   147,     6,     6,    62,   108,
     152,   140,   153,   111,   152,     3,   156,    71,   153,    73,
      45,    46,    47,    48,    49,    50,   156,    52,    53,   127,
     128,   129,   130,   131,    62,   153,    90,   135,   111,   153,
     153,   153,     4,     6,   142,   143,   144,   145,   146,   147,
     148,   149,   156,   156,   127,   128,   129,   130,   112,   113,
     114,   156,   135,   156,   156,   156,   156,   156,   156,   142,
     143,   144,   145,   146,   147,   148,   149,   156,   156,     5,
     134,   156,   156,   111,   156,   139,   156,    51,   156,     6,
       6,   145,   146,   147,     6,     6,     6,     6,   152,   127,
     128,   129,   130,     6,   132,     6,   111,   135,     6,     6,
       6,     6,     6,     6,   142,   143,   144,   145,   146,   147,
     148,   149,   127,   128,   129,   130,     6,     6,     6,    67,
     135,   153,   156,   153,     4,   153,   153,   142,   143,   144,
     145,   146,   147,   148,   149,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   153,   153,   153,     3,
      64,   142,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   153,   152,    50,   153,   365,   381,    50,    50,   422,
      50,   191,   120,    50,    50,    12,   140,   109,   390,   327,
     104,    50,   449,    50,   230,   218,   155,   277,   433,    91,
     587
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    15,    16,    20,
      25,    67,   152,   158,   159,   160,   162,   171,   172,   174,
     176,   179,   180,   181,   182,   183,   204,   209,   210,    26,
      27,    28,    37,    38,    30,    26,    27,    28,    37,    38,
       3,   202,    65,   202,    32,    33,    34,    35,    36,    39,
     175,    29,    40,    38,   202,   136,   137,   138,     3,   205,
     206,   182,   183,     0,   155,   238,    21,    23,    24,   198,
       8,   184,   186,    61,   237,   237,   237,   237,   237,   239,
     202,    61,   236,   236,   236,   236,   236,   154,    14,   202,
      65,    66,   161,   162,   171,   172,   176,   179,   180,   181,
     209,   210,   202,    44,   156,   153,   153,   160,    22,   152,
     185,   186,    54,   193,    56,   187,    62,     3,   202,   202,
     202,     3,    51,   152,   173,    63,     3,   202,   202,   202,
       3,     3,     3,   177,   178,    55,   195,     4,   235,   235,
     152,   206,   185,   186,     3,     4,     5,     6,    62,    63,
      71,    73,    90,   112,   113,   114,   134,   139,   145,   146,
     147,   152,   211,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   223,   224,   225,   226,   232,    42,    57,   191,
      63,    44,   152,   173,   202,     3,   170,    31,   183,    51,
     142,   156,   195,   214,    67,    67,   184,   153,   153,   152,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,    62,    63,   215,   152,   152,    76,   214,   222,
       4,     6,   232,   152,   152,   215,   215,   183,   214,    65,
     156,   194,    44,   132,   133,    62,    64,   111,   127,   128,
     129,   130,   131,   135,   142,   143,   144,   145,   146,   147,
     148,   149,   154,   188,   189,   214,   214,    58,   192,   181,
       3,   107,   109,   163,   164,   165,   169,    44,   152,   153,
     156,   152,   212,   202,   214,   178,   152,   152,   153,    54,
     153,   211,   152,    62,   183,   214,   214,   222,    72,    74,
      76,     4,   214,   153,   153,   152,   199,   200,   201,   202,
     207,   213,   195,     3,   214,   214,    64,   135,   152,    62,
     110,   215,   215,   215,   215,   215,   215,   215,   215,   215,
     215,   215,   215,   215,   215,     3,   147,   156,    59,    60,
     190,   214,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   166,   108,   152,   153,   156,   181,   170,     3,   211,
     156,    68,    69,    70,   233,   234,   233,   211,   153,   183,
     153,    44,    75,    72,    74,   214,   214,    65,   156,   181,
     156,    45,    47,    48,    49,    50,    52,    53,   208,     3,
      44,   203,    41,   197,   152,   215,   183,   211,   110,   132,
     189,   152,   152,   152,   152,   152,   152,   152,    62,   107,
     109,   110,   167,   168,   152,   170,   164,   153,   153,   152,
       4,     3,   153,   156,   153,   153,   153,   166,   214,   214,
      72,    75,   215,   150,   227,   228,   229,   230,   231,   153,
     200,    46,    46,     3,    42,    43,   196,   183,   211,   153,
     153,   215,     6,     6,     6,     6,     6,    78,    79,    80,
      81,    82,    86,    87,   106,    78,    79,    80,    81,    82,
      86,    87,   106,   110,   108,   168,   170,   153,   140,   211,
     234,   153,    72,   214,   153,     5,     6,   156,   151,   156,
     151,   156,   203,   201,   201,   152,   211,   214,   153,   153,
     153,   153,   153,   156,   153,   153,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   153,     3,   153,     4,     6,     5,    51,   170,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,    67,   240,   156,
     214,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   153,   153,   153,   153,   153,   153,   153,   153,   152,
       4,     3,   241,   242,   153,   142,   153,   156,     3,     5,
       6,   242
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   157,   158,   159,   159,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   162,   162,   162,   162,   162,   162,
     163,   163,   164,   164,   165,   165,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   167,   167,   168,   168,   168,   168,   169,
     169,   170,   170,   171,   172,   172,   173,   173,   174,   175,
     175,   175,   175,   175,   175,   175,   176,   177,   177,   178,
     179,   179,   179,   179,   179,   180,   180,   181,   181,   181,
     181,   182,   182,   183,   184,   185,   185,   186,   187,   187,
     188,   188,   189,   190,   190,   190,   191,   191,   192,   192,
     193,   193,   194,   194,   195,   195,   196,   196,   197,   197,
     198,   198,   198,   198,   199,   199,   200,   200,   201,   201,
     202,   202,   203,   203,   203,   203,   204,   204,   205,   205,
     206,   207,   207,   208,   208,   208,   208,   208,   208,   208,
     209,   209,   209,   209,   210,   210,   210,   211,   211,   212,
     212,   213,   213,   214,   214,   214,   214,   214,   215,   215,
     215,   215,   215,   215,   215,   215,   216,   217,   217,   217,
     217,   217,   217,   217,   217,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   217,   217,   217,   217,   217,   217,
     218,   218,   219,   220,   220,   221,   221,   221,   221,   222,
     222,   223,   224,   224,   224,   224,   225,   225,   225,   225,
     226,   226,   226,   226,   226,   226,   226,   226,   227,   227,
     228,   229,   229,   230,   231,   231,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   233,   233,
     234,   234,   234,   235,   236,   236,   237,   237,   238,   238,
     239,   239,   240,   240,   241,   241,   242,   242,   242,   242
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     4,     4,     7,     6,     7,    11,
       1,     3,     1,     1,     2,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     4,
       1,     4,     6,     4,     1,     4,     4,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     1,     2,     2,     1,     1,     2,     5,
       4,     1,     3,     4,     6,     5,     3,     0,     3,     1,
       1,     1,     1,     1,     1,     0,     5,     1,     3,     3,
       4,     4,     4,     4,     6,     8,     8,     1,     1,     3,
       3,     3,     3,     2,     4,     3,     3,     7,     3,     0,
       1,     3,     2,     1,     1,     0,     2,     0,     2,     0,
       1,     0,     2,     0,     2,     0,     2,     0,     3,     0,
       1,     2,     1,     1,     1,     3,     1,     1,     2,     4,
       1,     3,     2,     1,     5,     0,     2,     0,     1,     3,
       5,     4,     6,     1,     1,     1,     1,     1,     1,     0,
       2,     2,     2,     3,     2,     2,     2,     1,     3,     3,
       5,     3,     1,     1,     1,     1,     1,     1,     3,     3,
       1,     1,     1,     1,     1,     1,    10,     3,     4,     5,
       4,     3,     2,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     6,     3,     4,
       3,     3,     5,     5,     6,     4,     6,     3,     5,     4,
       5,     6,     4,     5,     5,     6,     1,     3,     1,     3,
       1,     1,     1,     1,     1,     2,     2,     1,     1,     1,
       2,     2,     3,     2,     2,     3,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     1,     3,
       2,     2,     1,     1,     2,     0,     3,     0,     1,     0,
       2,     0,     4,     0,     1,     3,     1,     3,     3,     3
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
#line 282 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 1893 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 282 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 1901 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 196 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 1915 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 186 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 1929 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 275 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 1940 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 286 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 1949 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 286 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 1958 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 246 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 1972 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 239 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 1983 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 321 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1993 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 321 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2003 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 321 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2013 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 321 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2023 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 321 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2033 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 321 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2043 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 229 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2057 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 229 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2071 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 309 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2081 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 291 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2089 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 291 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2097 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 304 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2106 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 291 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2114 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 291 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2122 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 206 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2136 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 304 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2145 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 304 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2154 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 304 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2163 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 266 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2176 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 299 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2185 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 256 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2199 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 256 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2213 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 315 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2223 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 304 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2232 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 206 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2246 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_expr_array_list: /* expr_array_list  */
#line 216 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 2263 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 291 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2271 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 291 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2279 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 291 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2287 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_knn_expr: /* knn_expr  */
#line 291 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2295 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 291 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2303 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 291 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2311 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 291 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2319 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 291 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2327 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 291 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2335 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 327 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 2348 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 291 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2356 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 291 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2364 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 291 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2372 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 295 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2380 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 295 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2388 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 295 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2396 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 295 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2404 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 295 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2412 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 295 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2420 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 295 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2428 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 282 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2436 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 179 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 2447 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
        break;

    case YYSYMBOL_with_index_para_list: /* with_index_para_list  */
#line 162 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    fprintf(stderr, "destroy create index para list\n");
    if ((((*yyvaluep).with_index_para_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_para_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_para_list_t));
    }
}
#line 2461 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
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
#line 83 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
{
  // Initialize
  yylloc.first_column = 0;
  yylloc.last_column = 0;
  yylloc.first_line = 0;
  yylloc.last_line = 0;
  yylloc.total_column = 0;
  yylloc.string_length = 0;
}

#line 2569 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"

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
#line 444 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 2784 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 448 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 2795 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 454 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 2806 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 461 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 2812 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 462 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 2818 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 463 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 2824 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 464 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 2830 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 465 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 2836 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 466 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 2842 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 467 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 2848 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 468 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 2854 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 469 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 2860 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 470 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 2866 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 15: /* explainable_statement: create_statement  */
#line 472 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 2872 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 16: /* explainable_statement: drop_statement  */
#line 473 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 2878 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 17: /* explainable_statement: copy_statement  */
#line 474 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 2884 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 18: /* explainable_statement: show_statement  */
#line 475 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 2890 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 19: /* explainable_statement: select_statement  */
#line 476 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 2896 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 20: /* explainable_statement: delete_statement  */
#line 477 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 2902 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 21: /* explainable_statement: update_statement  */
#line 478 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 2908 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 22: /* explainable_statement: insert_statement  */
#line 479 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 2914 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 23: /* explainable_statement: flush_statement  */
#line 480 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 2920 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 24: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 487 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                            {
    (yyval.create_stmt) = new infinity::CreateStatement();
    std::shared_ptr<infinity::CreateSchemaInfo> create_schema_info = std::make_shared<infinity::CreateSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    create_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.create_stmt)->create_info_ = create_schema_info;
    (yyval.create_stmt)->create_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 2936 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 25: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 500 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 2954 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 26: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')'  */
#line 515 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 2982 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 27: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 539 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 3002 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 28: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 555 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 3023 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 29: /* create_statement: CREATE INDEX if_not_exists_info ON table_name '(' identifier_array ')' USING IDENTIFIER with_index_para_list  */
#line 573 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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

    create_index_info->index_para_list_ = (yyvsp[0].with_index_para_list_t);
    (yyval.create_stmt)->create_info_ = create_index_info;
}
#line 3053 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 30: /* table_element_array: table_element  */
#line 599 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3062 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 31: /* table_element_array: table_element_array ',' table_element  */
#line 603 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3071 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 32: /* table_element: table_column  */
#line 609 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3079 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 33: /* table_element: table_constraint  */
#line 612 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3087 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 34: /* table_column: IDENTIFIER column_type  */
#line 618 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
        case infinity::LogicalType::kVarchar: {
            type_info_ptr = infinity::VarcharInfo::Make((yyvsp[0].column_type_t).width);
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
#line 3131 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 35: /* table_column: IDENTIFIER column_type column_constraints  */
#line 657 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
        case infinity::LogicalType::kVarchar: {
            type_info_ptr = infinity::VarcharInfo::Make((yyvsp[-1].column_type_t).width);
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
#line 3172 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 36: /* column_type: BOOLEAN  */
#line 695 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean}; }
#line 3178 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 37: /* column_type: TINYINT  */
#line 696 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt}; }
#line 3184 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 38: /* column_type: SMALLINT  */
#line 697 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt}; }
#line 3190 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 39: /* column_type: INTEGER  */
#line 698 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger}; }
#line 3196 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 40: /* column_type: INT  */
#line 699 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger}; }
#line 3202 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 41: /* column_type: BIGINT  */
#line 700 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt}; }
#line 3208 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 42: /* column_type: HUGEINT  */
#line 701 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt}; }
#line 3214 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 43: /* column_type: FLOAT  */
#line 702 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat}; }
#line 3220 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 44: /* column_type: REAL  */
#line 703 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat}; }
#line 3226 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 45: /* column_type: DOUBLE  */
#line 704 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble}; }
#line 3232 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 46: /* column_type: DATE  */
#line 705 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate}; }
#line 3238 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 47: /* column_type: TIME  */
#line 706 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime}; }
#line 3244 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 48: /* column_type: DATETIME  */
#line 707 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime}; }
#line 3250 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 49: /* column_type: TIMESTAMP  */
#line 708 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp}; }
#line 3256 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 50: /* column_type: UUID  */
#line 709 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid}; }
#line 3262 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 51: /* column_type: POINT  */
#line 710 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint}; }
#line 3268 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 52: /* column_type: LINE  */
#line 711 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine}; }
#line 3274 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 53: /* column_type: LSEG  */
#line 712 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg}; }
#line 3280 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 54: /* column_type: BOX  */
#line 713 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox}; }
#line 3286 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 55: /* column_type: PATH  */
#line 714 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPath}; }
#line 3292 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 56: /* column_type: POLYGON  */
#line 715 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPolygon}; }
#line 3298 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 57: /* column_type: CIRCLE  */
#line 716 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle}; }
#line 3304 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 58: /* column_type: CHAR  */
#line 718 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar}; }
#line 3310 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 59: /* column_type: CHAR '(' LONG_VALUE ')'  */
#line 719 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, (yyvsp[-1].long_value)}; }
#line 3316 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 60: /* column_type: VARCHAR  */
#line 720 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar}; }
#line 3322 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 61: /* column_type: VARCHAR '(' LONG_VALUE ')'  */
#line 721 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, (yyvsp[-1].long_value)}; }
#line 3328 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 62: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 722 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value)}; }
#line 3334 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 63: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 723 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0}; }
#line 3340 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 64: /* column_type: DECIMAL  */
#line 724 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0}; }
#line 3346 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 65: /* column_type: BLOB '(' LONG_VALUE ')'  */
#line 725 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBlob, (yyvsp[-1].long_value)}; }
#line 3352 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 66: /* column_type: BITMAP '(' LONG_VALUE ')'  */
#line 726 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBitmap, (yyvsp[-1].long_value)}; }
#line 3358 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 727 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3364 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 728 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3370 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 729 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3376 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 730 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3382 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 731 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3388 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 732 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3394 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 733 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3400 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 734 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3406 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 75: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 735 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3412 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 76: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 736 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3418 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 77: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 737 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3424 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 78: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 738 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3430 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 79: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 739 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3436 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 80: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 740 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3442 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 81: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 741 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3448 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 82: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 742 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3454 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 83: /* column_constraints: column_constraint  */
#line 761 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                       {
    (yyval.column_constraints_t) = new std::unordered_set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 3463 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 84: /* column_constraints: column_constraints column_constraint  */
#line 765 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 3477 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 85: /* column_constraint: PRIMARY KEY  */
#line 775 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 3485 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 86: /* column_constraint: UNIQUE  */
#line 778 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 3493 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 87: /* column_constraint: NULLABLE  */
#line 781 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 3501 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 88: /* column_constraint: NOT NULLABLE  */
#line 784 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 3509 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 89: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 788 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 3519 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 90: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 793 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 3529 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 91: /* identifier_array: IDENTIFIER  */
#line 800 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 3540 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 92: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 806 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 3551 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 93: /* delete_statement: DELETE FROM table_name where_clause  */
#line 816 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 3568 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 94: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 832 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 3585 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 95: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 844 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 3602 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 96: /* optional_identifier_array: '(' identifier_array ')'  */
#line 857 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 3610 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 97: /* optional_identifier_array: %empty  */
#line 860 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 3618 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 98: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 867 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 3628 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 99: /* explain_type: ANALYZE  */
#line 873 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 3636 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 100: /* explain_type: AST  */
#line 876 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 3644 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 101: /* explain_type: RAW  */
#line 879 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 3652 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 102: /* explain_type: LOGICAL  */
#line 882 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 3660 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 103: /* explain_type: PHYSICAL  */
#line 885 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3668 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 104: /* explain_type: PIPELINE  */
#line 888 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 3676 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 105: /* explain_type: %empty  */
#line 891 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3684 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 106: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 898 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 3701 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 107: /* update_expr_array: update_expr  */
#line 911 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 3710 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 108: /* update_expr_array: update_expr_array ',' update_expr  */
#line 915 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 3719 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 109: /* update_expr: IDENTIFIER '=' expr  */
#line 920 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 3731 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 110: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 933 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 3747 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 111: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 946 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 3765 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 112: /* drop_statement: DROP TABLE if_exists table_name  */
#line 961 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 3783 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 113: /* drop_statement: DROP VIEW if_exists table_name  */
#line 976 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 3801 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 114: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 991 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 3824 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 115: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1014 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 3870 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 116: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1055 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 3916 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 117: /* select_statement: select_without_paren  */
#line 1100 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3924 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 118: /* select_statement: select_with_paren  */
#line 1103 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3932 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 119: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1106 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 3946 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 120: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1115 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 3960 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 121: /* select_with_paren: '(' select_without_paren ')'  */
#line 1125 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3968 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 122: /* select_with_paren: '(' select_with_paren ')'  */
#line 1128 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3976 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 123: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1132 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3985 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 124: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1137 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                                                                   {
    (yyvsp[-3].select_stmt)->order_by_list = (yyvsp[-2].order_by_expr_list_t);
    (yyvsp[-3].select_stmt)->limit_expr_ = (yyvsp[-1].expr_t);
    (yyvsp[-3].select_stmt)->offset_expr_ = (yyvsp[0].expr_t);
    (yyval.select_stmt) = (yyvsp[-3].select_stmt);
}
#line 3996 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 125: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1144 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4004 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 126: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1147 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4012 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 127: /* select_clause_without_modifier: SELECT distinct expr_array from_clause where_clause group_by_clause having_clause  */
#line 1152 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                                                  {
    (yyval.select_stmt) = new infinity::SelectStatement();
    (yyval.select_stmt)->select_list_ = (yyvsp[-4].expr_array_t);
    (yyval.select_stmt)->select_distinct_ = (yyvsp[-5].bool_value);
    (yyval.select_stmt)->table_ref_ = (yyvsp[-3].table_reference_t);
    (yyval.select_stmt)->where_expr_ = (yyvsp[-2].expr_t);
    (yyval.select_stmt)->group_by_list_ = (yyvsp[-1].expr_array_t);
    (yyval.select_stmt)->having_expr_ = (yyvsp[0].expr_t);

    if((yyval.select_stmt)->group_by_list_ == nullptr && (yyval.select_stmt)->having_expr_ != nullptr) {
        yyerror(&yyloc, scanner, result, "HAVING clause should follow after GROUP BY clause");
        YYERROR;
    }
}
#line 4031 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 128: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1167 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 4039 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 129: /* order_by_clause: %empty  */
#line 1170 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 4047 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 130: /* order_by_expr_list: order_by_expr  */
#line 1174 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 4056 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 131: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1178 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 4065 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 132: /* order_by_expr: expr order_by_type  */
#line 1183 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 4075 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 133: /* order_by_type: ASC  */
#line 1189 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4083 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 134: /* order_by_type: DESC  */
#line 1192 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 4091 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 135: /* order_by_type: %empty  */
#line 1195 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4099 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 136: /* limit_expr: LIMIT expr  */
#line 1199 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4107 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 137: /* limit_expr: %empty  */
#line 1203 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4113 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 138: /* offset_expr: OFFSET expr  */
#line 1205 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4121 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 139: /* offset_expr: %empty  */
#line 1209 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4127 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 140: /* distinct: DISTINCT  */
#line 1211 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 4135 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 141: /* distinct: %empty  */
#line 1214 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
  {
    (yyval.bool_value) = false;
}
#line 4143 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 142: /* from_clause: FROM table_reference  */
#line 1218 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4151 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 143: /* from_clause: %empty  */
#line 1221 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 4159 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 144: /* where_clause: WHERE expr  */
#line 1225 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4167 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 145: /* where_clause: %empty  */
#line 1228 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4175 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 146: /* having_clause: HAVING expr  */
#line 1232 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4183 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 147: /* having_clause: %empty  */
#line 1235 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4191 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 148: /* group_by_clause: GROUP BY expr_array  */
#line 1239 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 4199 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 149: /* group_by_clause: %empty  */
#line 1242 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 4207 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 150: /* set_operator: UNION  */
#line 1246 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 4215 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 151: /* set_operator: UNION ALL  */
#line 1249 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 4223 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 152: /* set_operator: INTERSECT  */
#line 1252 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 4231 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 153: /* set_operator: EXCEPT  */
#line 1255 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 4239 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 154: /* table_reference: table_reference_unit  */
#line 1263 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4247 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 155: /* table_reference: table_reference ',' table_reference_unit  */
#line 1266 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 4265 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 158: /* table_reference_name: table_name table_alias  */
#line 1283 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 4283 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 159: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1297 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 4294 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 160: /* table_name: IDENTIFIER  */
#line 1306 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                        {
    if(!result->IsError()) {
        (yyval.table_name_t) = new infinity::TableName();
        ParserHelper::ToLower((yyvsp[0].str_value));
        (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
    }
}
#line 4306 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 161: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1313 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                            {
    if(!result->IsError()) {
        (yyval.table_name_t) = new infinity::TableName();
        ParserHelper::ToLower((yyvsp[-2].str_value));
        ParserHelper::ToLower((yyvsp[0].str_value));
        (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
        (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
    }
}
#line 4320 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 162: /* table_alias: AS IDENTIFIER  */
#line 1324 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4330 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 163: /* table_alias: IDENTIFIER  */
#line 1329 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4340 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 164: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1334 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 4351 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 165: /* table_alias: %empty  */
#line 1340 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 4359 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 166: /* with_clause: WITH with_expr_list  */
#line 1347 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 4367 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 167: /* with_clause: %empty  */
#line 1350 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 4375 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 168: /* with_expr_list: with_expr  */
#line 1354 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 4384 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 169: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1357 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 4393 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 170: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1362 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 4405 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 171: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1374 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 4417 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 172: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1381 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 4430 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 173: /* join_type: INNER  */
#line 1395 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 4438 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 174: /* join_type: LEFT  */
#line 1398 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 4446 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 175: /* join_type: RIGHT  */
#line 1401 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 4454 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 176: /* join_type: OUTER  */
#line 1404 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4462 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 177: /* join_type: FULL  */
#line 1407 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4470 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 178: /* join_type: CROSS  */
#line 1410 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 4478 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 179: /* join_type: %empty  */
#line 1413 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                {
}
#line 4485 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 180: /* show_statement: SHOW TABLES  */
#line 1419 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                            {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 4494 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 181: /* show_statement: SHOW VIEWS  */
#line 1423 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 4503 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 182: /* show_statement: DESCRIBE table_name  */
#line 1427 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 4519 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 183: /* show_statement: DESCRIBE INDEX table_name  */
#line 1438 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 4535 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 184: /* flush_statement: FLUSH DATA  */
#line 1453 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 4544 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 185: /* flush_statement: FLUSH LOG  */
#line 1457 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 4553 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 186: /* flush_statement: FLUSH BUFFER  */
#line 1461 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 4562 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 187: /* expr_array: expr_alias  */
#line 1470 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 4571 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 188: /* expr_array: expr_array ',' expr_alias  */
#line 1474 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 4580 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 189: /* expr_array_list: '(' expr_array ')'  */
#line 1479 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 4589 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 190: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 1483 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 4609 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 191: /* expr_alias: expr AS IDENTIFIER  */
#line 1510 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 4620 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 192: /* expr_alias: expr  */
#line 1516 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4628 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 198: /* operand: '(' expr ')'  */
#line 1526 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                      {
   (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 4636 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 199: /* operand: '(' select_without_paren ')'  */
#line 1529 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4647 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 200: /* operand: constant_expr  */
#line 1535 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 4655 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 206: /* knn_expr: KNN '(' expr ',' array_expr ',' STRING ',' STRING ')'  */
#line 1544 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                                 {
    infinity::KnnExpr* knn_expr = new infinity::KnnExpr();

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

    } else if(strcmp((yyvsp[-3].str_value), "tinyint") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-5].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt8;
        knn_expr->embedding_data_ptr_ = new char[knn_expr->dimension_];

        for(long i = 0; i < knn_expr->dimension_; ++ i) {
            ((char*)knn_expr->embedding_data_ptr_)[i] = (yyvsp[-5].const_expr_t)->long_array_[i];
        }
    } else if(strcmp((yyvsp[-3].str_value), "smallint") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-5].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt16;
        knn_expr->embedding_data_ptr_ = new short int[knn_expr->dimension_];

        for(long i = 0; i < knn_expr->dimension_; ++ i) {
            ((short int*)knn_expr->embedding_data_ptr_)[i] = (yyvsp[-5].const_expr_t)->long_array_[i];
        }
    } else if(strcmp((yyvsp[-3].str_value), "integer") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-5].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt32;
        knn_expr->embedding_data_ptr_ = new int[knn_expr->dimension_];

        for(long i = 0; i < knn_expr->dimension_; ++ i) {
            ((int*)knn_expr->embedding_data_ptr_)[i] = (yyvsp[-5].const_expr_t)->long_array_[i];
        }

    } else if(strcmp((yyvsp[-3].str_value), "bigint") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-5].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt64;
        knn_expr->embedding_data_ptr_ = new long[knn_expr->dimension_];

        memcpy(knn_expr->embedding_data_ptr_, (void*)(yyvsp[-5].const_expr_t)->long_array_.data(), knn_expr->dimension_ * sizeof(long));
    } else if(strcmp((yyvsp[-3].str_value), "bit") == 0 and knn_expr->distance_type_ == infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-5].const_expr_t)->long_array_.size();
        if(knn_expr->dimension_ % 8 == 0) {
            knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemBit;
            long embedding_size = knn_expr->dimension_ / 8;
            knn_expr->embedding_data_ptr_ = new char[embedding_size];

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
                        yyerror(&yyloc, scanner, result, "Invalid bit embedding type data");
                        YYERROR;
                    }
                }
                ((char*)knn_expr->embedding_data_ptr_)[i] = embedding_unit;
            }
        } else {
            yyerror(&yyloc, scanner, result, "KNN data type is bit which length should be aligned with 8");
            YYERROR;
        }

    } else if(strcmp((yyvsp[-3].str_value), "double") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-5].const_expr_t)->double_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemDouble;
        knn_expr->embedding_data_ptr_ = new double[knn_expr->dimension_];

        memcpy(knn_expr->embedding_data_ptr_, (void*)(yyvsp[-5].const_expr_t)->double_array_.data(), knn_expr->dimension_ * sizeof(double));
    } else {
        yyerror(&yyloc, scanner, result, "Invalid knn data type");
        YYERROR;
    }

    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    delete (yyvsp[-5].const_expr_t);

    (yyval.expr_t) = knn_expr;
}
#line 4768 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 207: /* function_expr: IDENTIFIER '(' ')'  */
#line 1653 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 4781 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 208: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 1661 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 4794 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 209: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 1669 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 4808 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 210: /* function_expr: operand IS NOT NULLABLE  */
#line 1678 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4820 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 211: /* function_expr: operand IS NULLABLE  */
#line 1685 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4832 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 212: /* function_expr: NOT operand  */
#line 1692 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4844 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 213: /* function_expr: '-' operand  */
#line 1699 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4856 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 214: /* function_expr: '+' operand  */
#line 1706 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4868 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 215: /* function_expr: operand '-' operand  */
#line 1713 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4881 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 216: /* function_expr: operand '+' operand  */
#line 1721 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4894 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 217: /* function_expr: operand '*' operand  */
#line 1729 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4907 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 218: /* function_expr: operand '/' operand  */
#line 1737 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4920 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 219: /* function_expr: operand '%' operand  */
#line 1745 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4933 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 220: /* function_expr: operand '=' operand  */
#line 1753 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4946 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 221: /* function_expr: operand EQUAL operand  */
#line 1761 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4959 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 222: /* function_expr: operand NOT_EQ operand  */
#line 1769 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4972 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 223: /* function_expr: operand '<' operand  */
#line 1777 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4985 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 224: /* function_expr: operand '>' operand  */
#line 1785 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4998 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 225: /* function_expr: operand LESS_EQ operand  */
#line 1793 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5011 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 226: /* function_expr: operand GREATER_EQ operand  */
#line 1801 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5024 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 227: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 1809 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    free((yyvsp[-3].str_value));
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5058 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 228: /* function_expr: operand LIKE operand  */
#line 1838 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5071 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 229: /* function_expr: operand NOT LIKE operand  */
#line 1846 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5084 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 230: /* conjunction_expr: expr AND expr  */
#line 1855 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5097 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 231: /* conjunction_expr: expr OR expr  */
#line 1863 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5110 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 232: /* between_expr: operand BETWEEN operand AND operand  */
#line 1872 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 5122 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 233: /* in_expr: operand IN '(' expr_array ')'  */
#line 1880 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 5133 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 234: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 1886 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 5144 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 235: /* case_expr: CASE expr case_check_array END  */
#line 1893 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 5155 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 236: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 1899 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 5167 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 237: /* case_expr: CASE case_check_array END  */
#line 1906 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 5177 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 238: /* case_expr: CASE case_check_array ELSE expr END  */
#line 1911 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 5188 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 239: /* case_check_array: WHEN expr THEN expr  */
#line 1918 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 5200 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 240: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 1925 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 5212 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 241: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 1933 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
        case infinity::LogicalType::kVarchar: {
            type_info_ptr = infinity::VarcharInfo::Make((yyvsp[-1].column_type_t).width);
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
#line 5244 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 242: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 1961 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5255 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 243: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 1967 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5266 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 244: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 1973 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5278 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 245: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 1980 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5290 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 246: /* column_expr: IDENTIFIER  */
#line 1988 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 5302 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 247: /* column_expr: column_expr '.' IDENTIFIER  */
#line 1995 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 5314 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 248: /* column_expr: '*'  */
#line 2002 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 5324 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 249: /* column_expr: column_expr '.' '*'  */
#line 2007 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 5338 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 250: /* constant_expr: STRING  */
#line 2017 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5348 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 251: /* constant_expr: TRUE  */
#line 2022 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 5358 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 252: /* constant_expr: FALSE  */
#line 2027 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 5368 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 253: /* constant_expr: DOUBLE_VALUE  */
#line 2032 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5378 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 254: /* constant_expr: LONG_VALUE  */
#line 2037 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5388 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 255: /* constant_expr: DATE STRING  */
#line 2042 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5398 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 256: /* constant_expr: INTERVAL interval_expr  */
#line 2047 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 5406 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 257: /* constant_expr: interval_expr  */
#line 2050 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 5414 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 258: /* array_expr: long_array_expr  */
#line 2062 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 5422 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 259: /* array_expr: double_array_expr  */
#line 2065 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 5430 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 260: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 2069 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 5438 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 261: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2073 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 5448 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 262: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2078 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 5457 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 263: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 2083 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 5465 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 264: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 2087 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 5475 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 265: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2092 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 5484 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 266: /* interval_expr: LONG_VALUE SECONDS  */
#line 2097 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5495 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 267: /* interval_expr: LONG_VALUE SECOND  */
#line 2103 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5506 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 268: /* interval_expr: LONG_VALUE MINUTES  */
#line 2109 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5517 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 269: /* interval_expr: LONG_VALUE MINUTE  */
#line 2115 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5528 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 270: /* interval_expr: LONG_VALUE HOURS  */
#line 2121 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5539 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 271: /* interval_expr: LONG_VALUE HOUR  */
#line 2127 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5550 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 272: /* interval_expr: LONG_VALUE DAYS  */
#line 2133 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5561 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 273: /* interval_expr: LONG_VALUE DAY  */
#line 2139 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5572 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 274: /* interval_expr: LONG_VALUE MONTHS  */
#line 2145 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5583 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 275: /* interval_expr: LONG_VALUE MONTH  */
#line 2151 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5594 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 276: /* interval_expr: LONG_VALUE YEARS  */
#line 2157 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5605 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 277: /* interval_expr: LONG_VALUE YEAR  */
#line 2163 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5616 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 278: /* copy_option_list: copy_option  */
#line 2174 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 5625 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 279: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2178 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 5634 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 280: /* copy_option: FORMAT IDENTIFIER  */
#line 2183 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
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
#line 5658 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 281: /* copy_option: DELIMITER STRING  */
#line 2202 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kDelimiter;
    (yyval.copy_option_t)->delimiter_ = (yyvsp[0].str_value)[0];
    free((yyvsp[0].str_value));
}
#line 5669 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 282: /* copy_option: HEADER  */
#line 2208 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 5679 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 283: /* file_path: STRING  */
#line 2214 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 5687 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 284: /* if_exists: IF EXISTS  */
#line 2218 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                     { (yyval.bool_value) = true; }
#line 5693 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 285: /* if_exists: %empty  */
#line 2219 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
  { (yyval.bool_value) = false; }
#line 5699 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 286: /* if_not_exists: IF NOT EXISTS  */
#line 2221 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                              { (yyval.bool_value) = true; }
#line 5705 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 287: /* if_not_exists: %empty  */
#line 2222 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
  { (yyval.bool_value) = false; }
#line 5711 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 290: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 2237 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5724 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 291: /* if_not_exists_info: %empty  */
#line 2245 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 5732 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 292: /* with_index_para_list: WITH '(' index_para_list ')'  */
#line 2250 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                                    {
    (yyval.with_index_para_list_t) = std::move((yyvsp[-1].index_para_list_t));
}
#line 5740 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 293: /* with_index_para_list: %empty  */
#line 2253 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
  {
    (yyval.with_index_para_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 5748 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 294: /* index_para_list: index_para  */
#line 2257 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                             {
    (yyval.index_para_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_para_list_t)->push_back((yyvsp[0].index_para_t));
}
#line 5757 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 295: /* index_para_list: index_para_list ',' index_para  */
#line 2261 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                                 {
    (yyvsp[-2].index_para_list_t)->push_back((yyvsp[0].index_para_t));
    (yyval.index_para_list_t) = (yyvsp[-2].index_para_list_t);
}
#line 5766 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 296: /* index_para: IDENTIFIER  */
#line 2266 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                        {
    (yyval.index_para_t) = new infinity::InitParameter();
    (yyval.index_para_t)->para_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5776 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 297: /* index_para: IDENTIFIER '=' IDENTIFIER  */
#line 2271 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                            {
    (yyval.index_para_t) = new infinity::InitParameter();
    (yyval.index_para_t)->para_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_para_t)->para_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5789 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 298: /* index_para: IDENTIFIER '=' LONG_VALUE  */
#line 2279 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                            {
    (yyval.index_para_t) = new infinity::InitParameter();
    (yyval.index_para_t)->para_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_para_t)->para_value_ = std::to_string((yyvsp[0].long_value));
}
#line 5801 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;

  case 299: /* index_para: IDENTIFIER '=' DOUBLE_VALUE  */
#line 2286 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"
                              {
    (yyval.index_para_t) = new infinity::InitParameter();
    (yyval.index_para_t)->para_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_para_t)->para_value_ = std::to_string((yyvsp[0].double_value));
}
#line 5813 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"
    break;


#line 5817 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.cpp"

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

#line 2294 "/home/shenyushi/Documents/Github/infiniflow/old_infinity/core/parser/parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

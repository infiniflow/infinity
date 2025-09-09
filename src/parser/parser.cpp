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

#include "parser.h"
#include "lexer.h"

void yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg);

#line 85 "parser.cpp"

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
  YYSYMBOL_ADD = 24,                       /* ADD  */
  YYSYMBOL_RENAME = 25,                    /* RENAME  */
  YYSYMBOL_DUMP = 26,                      /* DUMP  */
  YYSYMBOL_EXCEPT = 27,                    /* EXCEPT  */
  YYSYMBOL_FLUSH = 28,                     /* FLUSH  */
  YYSYMBOL_USE = 29,                       /* USE  */
  YYSYMBOL_OPTIMIZE = 30,                  /* OPTIMIZE  */
  YYSYMBOL_PROPERTIES = 31,                /* PROPERTIES  */
  YYSYMBOL_DATABASE = 32,                  /* DATABASE  */
  YYSYMBOL_TABLE = 33,                     /* TABLE  */
  YYSYMBOL_COLLECTION = 34,                /* COLLECTION  */
  YYSYMBOL_TABLES = 35,                    /* TABLES  */
  YYSYMBOL_INTO = 36,                      /* INTO  */
  YYSYMBOL_VALUES = 37,                    /* VALUES  */
  YYSYMBOL_VIEW = 38,                      /* VIEW  */
  YYSYMBOL_INDEX = 39,                     /* INDEX  */
  YYSYMBOL_TASKS = 40,                     /* TASKS  */
  YYSYMBOL_DATABASES = 41,                 /* DATABASES  */
  YYSYMBOL_SEGMENT = 42,                   /* SEGMENT  */
  YYSYMBOL_SEGMENTS = 43,                  /* SEGMENTS  */
  YYSYMBOL_BLOCK = 44,                     /* BLOCK  */
  YYSYMBOL_BLOCKS = 45,                    /* BLOCKS  */
  YYSYMBOL_COLUMN = 46,                    /* COLUMN  */
  YYSYMBOL_COLUMNS = 47,                   /* COLUMNS  */
  YYSYMBOL_INDEXES = 48,                   /* INDEXES  */
  YYSYMBOL_CHUNK = 49,                     /* CHUNK  */
  YYSYMBOL_SYSTEM = 50,                    /* SYSTEM  */
  YYSYMBOL_GROUP = 51,                     /* GROUP  */
  YYSYMBOL_BY = 52,                        /* BY  */
  YYSYMBOL_HAVING = 53,                    /* HAVING  */
  YYSYMBOL_AS = 54,                        /* AS  */
  YYSYMBOL_NATURAL = 55,                   /* NATURAL  */
  YYSYMBOL_JOIN = 56,                      /* JOIN  */
  YYSYMBOL_LEFT = 57,                      /* LEFT  */
  YYSYMBOL_RIGHT = 58,                     /* RIGHT  */
  YYSYMBOL_OUTER = 59,                     /* OUTER  */
  YYSYMBOL_FULL = 60,                      /* FULL  */
  YYSYMBOL_ON = 61,                        /* ON  */
  YYSYMBOL_INNER = 62,                     /* INNER  */
  YYSYMBOL_CROSS = 63,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 64,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 65,                     /* WHERE  */
  YYSYMBOL_ORDER = 66,                     /* ORDER  */
  YYSYMBOL_LIMIT = 67,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 68,                    /* OFFSET  */
  YYSYMBOL_ASC = 69,                       /* ASC  */
  YYSYMBOL_DESC = 70,                      /* DESC  */
  YYSYMBOL_IF = 71,                        /* IF  */
  YYSYMBOL_NOT = 72,                       /* NOT  */
  YYSYMBOL_EXISTS = 73,                    /* EXISTS  */
  YYSYMBOL_IN = 74,                        /* IN  */
  YYSYMBOL_FROM = 75,                      /* FROM  */
  YYSYMBOL_TO = 76,                        /* TO  */
  YYSYMBOL_WITH = 77,                      /* WITH  */
  YYSYMBOL_DELIMITER = 78,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 79,                    /* FORMAT  */
  YYSYMBOL_HEADER = 80,                    /* HEADER  */
  YYSYMBOL_HIGHLIGHT = 81,                 /* HIGHLIGHT  */
  YYSYMBOL_CAST = 82,                      /* CAST  */
  YYSYMBOL_END = 83,                       /* END  */
  YYSYMBOL_CASE = 84,                      /* CASE  */
  YYSYMBOL_ELSE = 85,                      /* ELSE  */
  YYSYMBOL_THEN = 86,                      /* THEN  */
  YYSYMBOL_WHEN = 87,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 88,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 89,                   /* INTEGER  */
  YYSYMBOL_INT = 90,                       /* INT  */
  YYSYMBOL_TINYINT = 91,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 92,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 93,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 94,                   /* HUGEINT  */
  YYSYMBOL_VARCHAR = 95,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 96,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 97,                    /* DOUBLE  */
  YYSYMBOL_REAL = 98,                      /* REAL  */
  YYSYMBOL_DECIMAL = 99,                   /* DECIMAL  */
  YYSYMBOL_DATE = 100,                     /* DATE  */
  YYSYMBOL_TIME = 101,                     /* TIME  */
  YYSYMBOL_DATETIME = 102,                 /* DATETIME  */
  YYSYMBOL_FLOAT16 = 103,                  /* FLOAT16  */
  YYSYMBOL_BFLOAT16 = 104,                 /* BFLOAT16  */
  YYSYMBOL_UNSIGNED = 105,                 /* UNSIGNED  */
  YYSYMBOL_TIMESTAMP = 106,                /* TIMESTAMP  */
  YYSYMBOL_UUID = 107,                     /* UUID  */
  YYSYMBOL_POINT = 108,                    /* POINT  */
  YYSYMBOL_LINE = 109,                     /* LINE  */
  YYSYMBOL_LSEG = 110,                     /* LSEG  */
  YYSYMBOL_BOX = 111,                      /* BOX  */
  YYSYMBOL_PATH = 112,                     /* PATH  */
  YYSYMBOL_POLYGON = 113,                  /* POLYGON  */
  YYSYMBOL_CIRCLE = 114,                   /* CIRCLE  */
  YYSYMBOL_BLOB = 115,                     /* BLOB  */
  YYSYMBOL_BITMAP = 116,                   /* BITMAP  */
  YYSYMBOL_ARRAY = 117,                    /* ARRAY  */
  YYSYMBOL_TUPLE = 118,                    /* TUPLE  */
  YYSYMBOL_EMBEDDING = 119,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 120,                   /* VECTOR  */
  YYSYMBOL_MULTIVECTOR = 121,              /* MULTIVECTOR  */
  YYSYMBOL_TENSOR = 122,                   /* TENSOR  */
  YYSYMBOL_SPARSE = 123,                   /* SPARSE  */
  YYSYMBOL_TENSORARRAY = 124,              /* TENSORARRAY  */
  YYSYMBOL_BIT = 125,                      /* BIT  */
  YYSYMBOL_TEXT = 126,                     /* TEXT  */
  YYSYMBOL_PRIMARY = 127,                  /* PRIMARY  */
  YYSYMBOL_KEY = 128,                      /* KEY  */
  YYSYMBOL_UNIQUE = 129,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 130,                 /* NULLABLE  */
  YYSYMBOL_IS = 131,                       /* IS  */
  YYSYMBOL_DEFAULT = 132,                  /* DEFAULT  */
  YYSYMBOL_COMMENT = 133,                  /* COMMENT  */
  YYSYMBOL_IGNORE = 134,                   /* IGNORE  */
  YYSYMBOL_TRUE = 135,                     /* TRUE  */
  YYSYMBOL_FALSE = 136,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 137,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 138,                   /* SECOND  */
  YYSYMBOL_SECONDS = 139,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 140,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 141,                  /* MINUTES  */
  YYSYMBOL_HOUR = 142,                     /* HOUR  */
  YYSYMBOL_HOURS = 143,                    /* HOURS  */
  YYSYMBOL_DAY = 144,                      /* DAY  */
  YYSYMBOL_DAYS = 145,                     /* DAYS  */
  YYSYMBOL_MONTH = 146,                    /* MONTH  */
  YYSYMBOL_MONTHS = 147,                   /* MONTHS  */
  YYSYMBOL_YEAR = 148,                     /* YEAR  */
  YYSYMBOL_YEARS = 149,                    /* YEARS  */
  YYSYMBOL_EQUAL = 150,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 151,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 152,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 153,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 154,                  /* BETWEEN  */
  YYSYMBOL_AND = 155,                      /* AND  */
  YYSYMBOL_OR = 156,                       /* OR  */
  YYSYMBOL_EXTRACT = 157,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 158,                     /* LIKE  */
  YYSYMBOL_DATA = 159,                     /* DATA  */
  YYSYMBOL_LOG = 160,                      /* LOG  */
  YYSYMBOL_BUFFER = 161,                   /* BUFFER  */
  YYSYMBOL_TRANSACTIONS = 162,             /* TRANSACTIONS  */
  YYSYMBOL_TRANSACTION = 163,              /* TRANSACTION  */
  YYSYMBOL_MEMINDEX = 164,                 /* MEMINDEX  */
  YYSYMBOL_USING = 165,                    /* USING  */
  YYSYMBOL_SESSION = 166,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 167,                   /* GLOBAL  */
  YYSYMBOL_OFF = 168,                      /* OFF  */
  YYSYMBOL_EXPORT = 169,                   /* EXPORT  */
  YYSYMBOL_CONFIGS = 170,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 171,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 172,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 173,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 174,                 /* VARIABLE  */
  YYSYMBOL_LOGS = 175,                     /* LOGS  */
  YYSYMBOL_CATALOGS = 176,                 /* CATALOGS  */
  YYSYMBOL_CATALOG = 177,                  /* CATALOG  */
  YYSYMBOL_SEARCH = 178,                   /* SEARCH  */
  YYSYMBOL_MATCH = 179,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 180,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 181,                    /* QUERY  */
  YYSYMBOL_QUERIES = 182,                  /* QUERIES  */
  YYSYMBOL_FUSION = 183,                   /* FUSION  */
  YYSYMBOL_ROWLIMIT = 184,                 /* ROWLIMIT  */
  YYSYMBOL_ADMIN = 185,                    /* ADMIN  */
  YYSYMBOL_LEADER = 186,                   /* LEADER  */
  YYSYMBOL_FOLLOWER = 187,                 /* FOLLOWER  */
  YYSYMBOL_LEARNER = 188,                  /* LEARNER  */
  YYSYMBOL_CONNECT = 189,                  /* CONNECT  */
  YYSYMBOL_STANDALONE = 190,               /* STANDALONE  */
  YYSYMBOL_NODES = 191,                    /* NODES  */
  YYSYMBOL_NODE = 192,                     /* NODE  */
  YYSYMBOL_REMOVE = 193,                   /* REMOVE  */
  YYSYMBOL_SNAPSHOT = 194,                 /* SNAPSHOT  */
  YYSYMBOL_SNAPSHOTS = 195,                /* SNAPSHOTS  */
  YYSYMBOL_RECOVER = 196,                  /* RECOVER  */
  YYSYMBOL_RESTORE = 197,                  /* RESTORE  */
  YYSYMBOL_CACHES = 198,                   /* CACHES  */
  YYSYMBOL_CACHE = 199,                    /* CACHE  */
  YYSYMBOL_PERSISTENCE = 200,              /* PERSISTENCE  */
  YYSYMBOL_OBJECT = 201,                   /* OBJECT  */
  YYSYMBOL_OBJECTS = 202,                  /* OBJECTS  */
  YYSYMBOL_FILES = 203,                    /* FILES  */
  YYSYMBOL_MEMORY = 204,                   /* MEMORY  */
  YYSYMBOL_ALLOCATION = 205,               /* ALLOCATION  */
  YYSYMBOL_HISTORY = 206,                  /* HISTORY  */
  YYSYMBOL_CHECK = 207,                    /* CHECK  */
  YYSYMBOL_CLEAN = 208,                    /* CLEAN  */
  YYSYMBOL_CHECKPOINT = 209,               /* CHECKPOINT  */
  YYSYMBOL_IMPORT = 210,                   /* IMPORT  */
  YYSYMBOL_NUMBER = 211,                   /* NUMBER  */
  YYSYMBOL_212_ = 212,                     /* '='  */
  YYSYMBOL_213_ = 213,                     /* '<'  */
  YYSYMBOL_214_ = 214,                     /* '>'  */
  YYSYMBOL_215_ = 215,                     /* '+'  */
  YYSYMBOL_216_ = 216,                     /* '-'  */
  YYSYMBOL_217_ = 217,                     /* '*'  */
  YYSYMBOL_218_ = 218,                     /* '/'  */
  YYSYMBOL_219_ = 219,                     /* '%'  */
  YYSYMBOL_220_ = 220,                     /* '['  */
  YYSYMBOL_221_ = 221,                     /* ']'  */
  YYSYMBOL_222_ = 222,                     /* '('  */
  YYSYMBOL_223_ = 223,                     /* ')'  */
  YYSYMBOL_224_ = 224,                     /* '.'  */
  YYSYMBOL_225_ = 225,                     /* ';'  */
  YYSYMBOL_226_ = 226,                     /* ','  */
  YYSYMBOL_227_ = 227,                     /* '}'  */
  YYSYMBOL_228_ = 228,                     /* '{'  */
  YYSYMBOL_229_ = 229,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 230,                 /* $accept  */
  YYSYMBOL_input_pattern = 231,            /* input_pattern  */
  YYSYMBOL_statement_list = 232,           /* statement_list  */
  YYSYMBOL_statement = 233,                /* statement  */
  YYSYMBOL_explainable_statement = 234,    /* explainable_statement  */
  YYSYMBOL_create_statement = 235,         /* create_statement  */
  YYSYMBOL_table_element_array = 236,      /* table_element_array  */
  YYSYMBOL_column_def_array = 237,         /* column_def_array  */
  YYSYMBOL_table_element = 238,            /* table_element  */
  YYSYMBOL_table_column = 239,             /* table_column  */
  YYSYMBOL_column_type_array = 240,        /* column_type_array  */
  YYSYMBOL_column_type = 241,              /* column_type  */
  YYSYMBOL_column_constraints = 242,       /* column_constraints  */
  YYSYMBOL_column_constraint = 243,        /* column_constraint  */
  YYSYMBOL_default_expr = 244,             /* default_expr  */
  YYSYMBOL_table_constraint = 245,         /* table_constraint  */
  YYSYMBOL_identifier_array = 246,         /* identifier_array  */
  YYSYMBOL_delete_statement = 247,         /* delete_statement  */
  YYSYMBOL_insert_statement = 248,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 249, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 250,        /* explain_statement  */
  YYSYMBOL_update_statement = 251,         /* update_statement  */
  YYSYMBOL_update_expr_array = 252,        /* update_expr_array  */
  YYSYMBOL_update_expr = 253,              /* update_expr  */
  YYSYMBOL_drop_statement = 254,           /* drop_statement  */
  YYSYMBOL_copy_statement = 255,           /* copy_statement  */
  YYSYMBOL_select_statement = 256,         /* select_statement  */
  YYSYMBOL_select_with_paren = 257,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 258,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 259, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 260, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 261, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 262,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 263,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 264,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 265,            /* order_by_type  */
  YYSYMBOL_limit_expr = 266,               /* limit_expr  */
  YYSYMBOL_offset_expr = 267,              /* offset_expr  */
  YYSYMBOL_distinct = 268,                 /* distinct  */
  YYSYMBOL_highlight_clause = 269,         /* highlight_clause  */
  YYSYMBOL_from_clause = 270,              /* from_clause  */
  YYSYMBOL_search_clause = 271,            /* search_clause  */
  YYSYMBOL_optional_search_filter_expr = 272, /* optional_search_filter_expr  */
  YYSYMBOL_where_clause = 273,             /* where_clause  */
  YYSYMBOL_having_clause = 274,            /* having_clause  */
  YYSYMBOL_group_by_clause = 275,          /* group_by_clause  */
  YYSYMBOL_set_operator = 276,             /* set_operator  */
  YYSYMBOL_table_reference = 277,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 278,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 279,     /* table_reference_name  */
  YYSYMBOL_table_name = 280,               /* table_name  */
  YYSYMBOL_table_alias = 281,              /* table_alias  */
  YYSYMBOL_with_clause = 282,              /* with_clause  */
  YYSYMBOL_with_expr_list = 283,           /* with_expr_list  */
  YYSYMBOL_with_expr = 284,                /* with_expr  */
  YYSYMBOL_join_clause = 285,              /* join_clause  */
  YYSYMBOL_join_type = 286,                /* join_type  */
  YYSYMBOL_show_statement = 287,           /* show_statement  */
  YYSYMBOL_flush_statement = 288,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 289,       /* optimize_statement  */
  YYSYMBOL_command_statement = 290,        /* command_statement  */
  YYSYMBOL_compact_statement = 291,        /* compact_statement  */
  YYSYMBOL_admin_statement = 292,          /* admin_statement  */
  YYSYMBOL_alter_statement = 293,          /* alter_statement  */
  YYSYMBOL_check_statement = 294,          /* check_statement  */
  YYSYMBOL_expr_array = 295,               /* expr_array  */
  YYSYMBOL_insert_row_list = 296,          /* insert_row_list  */
  YYSYMBOL_expr_alias = 297,               /* expr_alias  */
  YYSYMBOL_expr = 298,                     /* expr  */
  YYSYMBOL_operand = 299,                  /* operand  */
  YYSYMBOL_match_tensor_expr = 300,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 301,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 302,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 303,          /* match_text_expr  */
  YYSYMBOL_query_expr = 304,               /* query_expr  */
  YYSYMBOL_fusion_expr = 305,              /* fusion_expr  */
  YYSYMBOL_sub_search = 306,               /* sub_search  */
  YYSYMBOL_sub_search_array = 307,         /* sub_search_array  */
  YYSYMBOL_function_expr = 308,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 309,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 310,             /* between_expr  */
  YYSYMBOL_in_expr = 311,                  /* in_expr  */
  YYSYMBOL_case_expr = 312,                /* case_expr  */
  YYSYMBOL_case_check_array = 313,         /* case_check_array  */
  YYSYMBOL_cast_expr = 314,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 315,            /* subquery_expr  */
  YYSYMBOL_column_expr = 316,              /* column_expr  */
  YYSYMBOL_constant_expr = 317,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 318,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 319, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 320,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 321, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 322,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 323,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 324, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 325, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 326, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 327,         /* empty_array_expr  */
  YYSYMBOL_curly_brackets_expr = 328,      /* curly_brackets_expr  */
  YYSYMBOL_unclosed_curly_brackets_expr = 329, /* unclosed_curly_brackets_expr  */
  YYSYMBOL_int_sparse_ele = 330,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 331,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 332,               /* array_expr  */
  YYSYMBOL_long_array_expr = 333,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 334, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 335,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 336, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 337,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 338,         /* copy_option_list  */
  YYSYMBOL_copy_option = 339,              /* copy_option  */
  YYSYMBOL_file_path = 340,                /* file_path  */
  YYSYMBOL_if_exists = 341,                /* if_exists  */
  YYSYMBOL_if_not_exists = 342,            /* if_not_exists  */
  YYSYMBOL_semicolon = 343,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 344,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 345,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 346, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 347,         /* index_param_list  */
  YYSYMBOL_index_param = 348,              /* index_param  */
  YYSYMBOL_index_info = 349                /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 99 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 476 "parser.cpp"

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
#define YYFINAL  138
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1578

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  230
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  120
/* YYNRULES -- Number of rules.  */
#define YYNRULES  564
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1272

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   466


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
       2,     2,     2,     2,     2,     2,     2,   219,     2,     2,
     222,   223,   217,   215,   226,   216,   224,   218,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   229,   225,
     213,   212,   214,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   220,     2,   221,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   228,     2,   227,     2,     2,     2,     2,
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
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   521,   521,   525,   531,   538,   539,   540,   541,   542,
     543,   544,   545,   546,   547,   548,   549,   550,   551,   552,
     553,   555,   556,   557,   558,   559,   560,   561,   562,   563,
     564,   565,   566,   573,   590,   607,   623,   652,   667,   699,
     717,   735,   763,   794,   798,   803,   807,   813,   816,   823,
     843,   865,   889,   915,   919,   925,   926,   927,   928,   929,
     930,   931,   932,   933,   934,   935,   936,   937,   938,   939,
     940,   941,   942,   943,   944,   945,   948,   950,   951,   952,
     953,   956,   957,   958,   959,   960,   961,   962,   963,   964,
     965,   966,   967,   968,   969,   970,   971,   972,   973,   974,
     975,   976,   977,   978,   979,   980,   981,   982,   983,   984,
     985,   986,   987,   988,   989,   990,   991,   992,   993,   994,
     995,   996,   997,   998,   999,  1000,  1001,  1002,  1003,  1004,
    1005,  1006,  1007,  1008,  1009,  1010,  1011,  1012,  1013,  1014,
    1015,  1016,  1017,  1018,  1019,  1020,  1021,  1022,  1026,  1049,
    1053,  1063,  1066,  1069,  1072,  1076,  1079,  1084,  1089,  1096,
    1102,  1112,  1128,  1166,  1182,  1185,  1192,  1209,  1218,  1231,
    1235,  1240,  1253,  1266,  1281,  1296,  1311,  1334,  1387,  1442,
    1493,  1496,  1499,  1508,  1518,  1521,  1525,  1530,  1557,  1560,
    1565,  1582,  1585,  1589,  1593,  1598,  1604,  1607,  1610,  1614,
    1618,  1620,  1624,  1626,  1629,  1633,  1636,  1640,  1643,  1647,
    1652,  1656,  1659,  1663,  1666,  1670,  1673,  1677,  1680,  1684,
    1687,  1690,  1693,  1701,  1704,  1719,  1719,  1721,  1735,  1744,
    1749,  1758,  1763,  1768,  1774,  1781,  1784,  1788,  1791,  1796,
    1808,  1815,  1829,  1832,  1835,  1838,  1841,  1844,  1847,  1853,
    1857,  1861,  1865,  1869,  1876,  1880,  1884,  1888,  1892,  1897,
    1901,  1906,  1910,  1914,  1918,  1924,  1930,  1937,  1948,  1959,
    1970,  1982,  1994,  2007,  2021,  2032,  2047,  2064,  2082,  2086,
    2090,  2097,  2103,  2107,  2111,  2117,  2121,  2125,  2129,  2136,
    2140,  2147,  2151,  2155,  2159,  2164,  2168,  2173,  2178,  2182,
    2187,  2191,  2195,  2200,  2209,  2213,  2217,  2221,  2229,  2240,
    2264,  2270,  2275,  2281,  2287,  2295,  2301,  2307,  2313,  2319,
    2327,  2333,  2339,  2345,  2351,  2359,  2365,  2371,  2379,  2387,
    2393,  2399,  2405,  2411,  2415,  2427,  2440,  2446,  2453,  2461,
    2470,  2480,  2490,  2501,  2512,  2524,  2536,  2546,  2557,  2569,
    2582,  2586,  2591,  2596,  2602,  2606,  2610,  2617,  2621,  2625,
    2632,  2638,  2646,  2652,  2656,  2662,  2666,  2672,  2677,  2682,
    2689,  2698,  2708,  2718,  2730,  2742,  2746,  2762,  2766,  2771,
    2781,  2803,  2809,  2813,  2814,  2815,  2816,  2817,  2819,  2822,
    2828,  2831,  2832,  2833,  2834,  2835,  2836,  2837,  2838,  2839,
    2840,  2844,  2860,  2878,  2896,  2954,  3004,  3058,  3116,  3141,
    3164,  3185,  3206,  3215,  3226,  3237,  3251,  3258,  3268,  3274,
    3286,  3289,  3292,  3295,  3298,  3301,  3305,  3309,  3314,  3322,
    3330,  3339,  3346,  3353,  3360,  3367,  3374,  3381,  3388,  3395,
    3402,  3409,  3416,  3424,  3432,  3440,  3448,  3456,  3464,  3472,
    3480,  3488,  3496,  3504,  3512,  3542,  3550,  3559,  3567,  3576,
    3584,  3590,  3597,  3603,  3610,  3615,  3622,  3629,  3637,  3650,
    3656,  3662,  3669,  3677,  3684,  3691,  3696,  3706,  3711,  3716,
    3721,  3726,  3731,  3736,  3741,  3746,  3751,  3754,  3757,  3760,
    3764,  3767,  3770,  3773,  3777,  3780,  3783,  3787,  3791,  3796,
    3801,  3804,  3808,  3812,  3819,  3826,  3830,  3837,  3844,  3848,
    3851,  3855,  3859,  3864,  3868,  3872,  3875,  3879,  3883,  3888,
    3893,  3897,  3902,  3907,  3913,  3919,  3925,  3931,  3937,  3943,
    3949,  3955,  3961,  3967,  3973,  3984,  3988,  3993,  4025,  4035,
    4040,  4045,  4050,  4056,  4060,  4061,  4063,  4064,  4066,  4067,
    4079,  4087,  4091,  4094,  4098,  4101,  4105,  4109,  4114,  4120,
    4130,  4140,  4148,  4159,  4191
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
  "PREPARE", "UNION", "ALL", "INTERSECT", "COMPACT", "ADD", "RENAME",
  "DUMP", "EXCEPT", "FLUSH", "USE", "OPTIMIZE", "PROPERTIES", "DATABASE",
  "TABLE", "COLLECTION", "TABLES", "INTO", "VALUES", "VIEW", "INDEX",
  "TASKS", "DATABASES", "SEGMENT", "SEGMENTS", "BLOCK", "BLOCKS", "COLUMN",
  "COLUMNS", "INDEXES", "CHUNK", "SYSTEM", "GROUP", "BY", "HAVING", "AS",
  "NATURAL", "JOIN", "LEFT", "RIGHT", "OUTER", "FULL", "ON", "INNER",
  "CROSS", "DISTINCT", "WHERE", "ORDER", "LIMIT", "OFFSET", "ASC", "DESC",
  "IF", "NOT", "EXISTS", "IN", "FROM", "TO", "WITH", "DELIMITER", "FORMAT",
  "HEADER", "HIGHLIGHT", "CAST", "END", "CASE", "ELSE", "THEN", "WHEN",
  "BOOLEAN", "INTEGER", "INT", "TINYINT", "SMALLINT", "BIGINT", "HUGEINT",
  "VARCHAR", "FLOAT", "DOUBLE", "REAL", "DECIMAL", "DATE", "TIME",
  "DATETIME", "FLOAT16", "BFLOAT16", "UNSIGNED", "TIMESTAMP", "UUID",
  "POINT", "LINE", "LSEG", "BOX", "PATH", "POLYGON", "CIRCLE", "BLOB",
  "BITMAP", "ARRAY", "TUPLE", "EMBEDDING", "VECTOR", "MULTIVECTOR",
  "TENSOR", "SPARSE", "TENSORARRAY", "BIT", "TEXT", "PRIMARY", "KEY",
  "UNIQUE", "NULLABLE", "IS", "DEFAULT", "COMMENT", "IGNORE", "TRUE",
  "FALSE", "INTERVAL", "SECOND", "SECONDS", "MINUTE", "MINUTES", "HOUR",
  "HOURS", "DAY", "DAYS", "MONTH", "MONTHS", "YEAR", "YEARS", "EQUAL",
  "NOT_EQ", "LESS_EQ", "GREATER_EQ", "BETWEEN", "AND", "OR", "EXTRACT",
  "LIKE", "DATA", "LOG", "BUFFER", "TRANSACTIONS", "TRANSACTION",
  "MEMINDEX", "USING", "SESSION", "GLOBAL", "OFF", "EXPORT", "CONFIGS",
  "CONFIG", "PROFILES", "VARIABLES", "VARIABLE", "LOGS", "CATALOGS",
  "CATALOG", "SEARCH", "MATCH", "MAXSIM", "QUERY", "QUERIES", "FUSION",
  "ROWLIMIT", "ADMIN", "LEADER", "FOLLOWER", "LEARNER", "CONNECT",
  "STANDALONE", "NODES", "NODE", "REMOVE", "SNAPSHOT", "SNAPSHOTS",
  "RECOVER", "RESTORE", "CACHES", "CACHE", "PERSISTENCE", "OBJECT",
  "OBJECTS", "FILES", "MEMORY", "ALLOCATION", "HISTORY", "CHECK", "CLEAN",
  "CHECKPOINT", "IMPORT", "NUMBER", "'='", "'<'", "'>'", "'+'", "'-'",
  "'*'", "'/'", "'%'", "'['", "']'", "'('", "')'", "'.'", "';'", "','",
  "'}'", "'{'", "':'", "$accept", "input_pattern", "statement_list",
  "statement", "explainable_statement", "create_statement",
  "table_element_array", "column_def_array", "table_element",
  "table_column", "column_type_array", "column_type", "column_constraints",
  "column_constraint", "default_expr", "table_constraint",
  "identifier_array", "delete_statement", "insert_statement",
  "optional_identifier_array", "explain_statement", "update_statement",
  "update_expr_array", "update_expr", "drop_statement", "copy_statement",
  "select_statement", "select_with_paren", "select_without_paren",
  "select_clause_with_modifier", "select_clause_without_modifier_paren",
  "select_clause_without_modifier", "order_by_clause",
  "order_by_expr_list", "order_by_expr", "order_by_type", "limit_expr",
  "offset_expr", "distinct", "highlight_clause", "from_clause",
  "search_clause", "optional_search_filter_expr", "where_clause",
  "having_clause", "group_by_clause", "set_operator", "table_reference",
  "table_reference_unit", "table_reference_name", "table_name",
  "table_alias", "with_clause", "with_expr_list", "with_expr",
  "join_clause", "join_type", "show_statement", "flush_statement",
  "optimize_statement", "command_statement", "compact_statement",
  "admin_statement", "alter_statement", "check_statement", "expr_array",
  "insert_row_list", "expr_alias", "expr", "operand", "match_tensor_expr",
  "match_vector_expr", "match_sparse_expr", "match_text_expr",
  "query_expr", "fusion_expr", "sub_search", "sub_search_array",
  "function_expr", "conjunction_expr", "between_expr", "in_expr",
  "case_expr", "case_check_array", "cast_expr", "subquery_expr",
  "column_expr", "constant_expr", "common_array_expr",
  "common_sparse_array_expr", "subarray_array_expr",
  "unclosed_subarray_array_expr", "sparse_array_expr",
  "long_sparse_array_expr", "unclosed_long_sparse_array_expr",
  "double_sparse_array_expr", "unclosed_double_sparse_array_expr",
  "empty_array_expr", "curly_brackets_expr",
  "unclosed_curly_brackets_expr", "int_sparse_ele", "float_sparse_ele",
  "array_expr", "long_array_expr", "unclosed_long_array_expr",
  "double_array_expr", "unclosed_double_array_expr", "interval_expr",
  "copy_option_list", "copy_option", "file_path", "if_exists",
  "if_not_exists", "semicolon", "if_not_exists_info",
  "with_index_param_list", "optional_table_properties_list",
  "index_param_list", "index_param", "index_info", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-776)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-552)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    1013,    45,    52,   259,    96,    32,    96,   219,   345,  1042,
     140,   156,   162,   323,   226,   274,   360,    50,    94,   256,
      43,   167,   -37,   347,   170,  -776,  -776,  -776,  -776,  -776,
    -776,  -776,  -776,   436,  -776,  -776,   369,  -776,  -776,  -776,
    -776,  -776,  -776,  -776,  -776,   379,   379,   379,   379,   246,
     408,    96,   398,   398,   398,   398,   398,   412,   253,   511,
      96,   -27,   542,   546,   552,   243,  -776,  -776,  -776,  -776,
    -776,  -776,  -776,   436,  -776,  -776,  -776,  -776,  -776,   335,
     493,   497,   568,    96,  -776,  -776,  -776,  -776,  -776,    22,
    -776,   196,   422,  -776,   573,  -776,  -776,    61,   575,  -776,
     586,  -776,  -776,  -776,   417,    98,    84,    99,  -776,    96,
      96,   602,  -776,  -776,  -776,  -776,  -776,   -25,  -776,   577,
     409,  -776,   645,   465,   474,   -90,  1143,   476,   660,   508,
     610,   522,   538,    96,  -776,  -776,   482,   520,  -776,   936,
    -776,   729,  -776,  -776,     3,   688,  -776,   700,   703,   768,
      96,    96,    96,   773,   716,   723,   563,   718,   784,    96,
      96,    96,   786,  -776,   790,   792,   731,   793,   793,   672,
      69,   106,   126,  -776,   576,   668,   670,  -776,   470,  -776,
    -776,  -776,   798,  -776,   800,  -776,  -776,   793,  -776,  -776,
     801,  -776,  -776,  -776,  -776,  -776,   674,  -776,   676,   469,
    -776,   750,    96,   591,   360,   793,  -776,   811,  -776,   654,
    -776,   815,  -776,  -776,   814,  -776,   816,  -776,   819,   821,
    -776,   826,   771,   828,   640,   832,   833,  -776,  -776,  -776,
    -776,  -776,     3,  -776,  -776,  -776,   672,   785,   779,   765,
     707,    -1,  -776,   563,  -776,    96,   299,   844,    36,  -776,
    -776,  -776,  -776,  -776,   787,  -776,   642,   -26,  -776,   672,
    -776,  -776,   775,   780,   634,  -776,  -776,  1206,   777,   641,
     647,   486,   854,   858,   866,   867,  -776,  -776,   868,   651,
     653,   658,   659,   662,   663,   664,   428,   669,   671,   838,
     838,  -776,    20,   611,    15,   209,  -776,     2,   714,  -776,
    -776,  -776,  -776,  -776,  -776,  -776,  -776,  -776,  -776,  -776,
    -776,  -776,   666,  -776,  -776,  -776,   188,  -776,  -776,   207,
    -776,   278,  -776,  -776,   271,  -776,  -776,   289,  -776,   301,
    -776,  -776,  -776,  -776,  -776,  -776,  -776,  -776,  -776,  -776,
    -776,  -776,  -776,  -776,  -776,  -776,  -776,  -776,  -776,   873,
     876,  -776,  -776,  -776,  -776,  -776,  -776,  -776,  -776,  -776,
     849,   850,   822,    96,   820,   369,  -776,  -776,  -776,   895,
     275,  -776,   896,  -776,  -776,   825,   314,  -776,   899,  -776,
    -776,   681,   682,   -38,   672,   672,   839,  -776,   902,   -37,
      57,   855,   689,   905,   913,  -776,  -776,   285,   696,  -776,
      96,   672,   792,  -776,   456,   702,   713,   324,  -776,  -776,
    -776,  -776,  -776,  -776,  -776,  -776,  -776,  -776,  -776,  -776,
     838,   715,   521,   864,   672,   672,   -33,   447,  -776,  -776,
    -776,  -776,  1206,  -776,   672,   672,   672,   672,   672,   672,
     938,   732,   733,   735,   739,   959,   963,   269,   269,  -776,
     740,  -776,  -776,  -776,  -776,   745,    76,  -776,  -776,   894,
     672,   968,   672,   672,    12,   755,   152,   838,   838,   838,
     838,   838,   838,   838,   838,   838,   838,   838,   838,   838,
     838,    41,  -776,   752,  -776,   973,  -776,   975,    18,  -776,
    -776,   977,  -776,   980,   945,   636,   766,   767,   987,  -776,
     769,  -776,   778,  -776,   990,  -776,   164,   994,   835,   837,
    -776,  -776,  -776,   672,   928,   781,  -776,   157,   456,   672,
    -776,  -776,    30,  1165,   878,   782,   297,  -776,  -776,  -776,
     -37,  1005,   877,  -776,  -776,  -776,  1006,   672,   788,  -776,
     456,  -776,    67,    67,   672,  -776,   340,   864,   853,   789,
      38,   -28,   477,  -776,   672,   672,   112,   130,   155,   166,
     169,   190,   940,   672,    44,   672,  1012,   805,   354,   704,
    -776,  -776,   793,  -776,  -776,  -776,   863,   810,   838,   611,
     903,  -776,   948,   948,   185,   185,   229,   948,   948,   185,
     185,   269,   269,  -776,  -776,  -776,  -776,  -776,  -776,   806,
    -776,   808,  -776,  -776,  -776,  -776,  1028,  1032,  -776,   844,
    1037,  -776,  1041,  -776,  -776,  1040,  -776,  -776,  1043,  1044,
     823,    14,   872,   672,  -776,  -776,  -776,   456,  1047,  -776,
    -776,  -776,  -776,  -776,  -776,  -776,  -776,  -776,  -776,  -776,
     830,  -776,  -776,  -776,  -776,  -776,  -776,  -776,  -776,  -776,
    -776,  -776,  -776,   834,   840,   841,   842,   845,   846,   847,
     848,   290,   851,   844,  1026,    57,   436,   836,  1057,  -776,
     359,   856,  1065,  1070,  1076,  1078,  -776,  1079,   368,  -776,
     384,   410,  -776,   861,  -776,  1165,   672,  -776,   672,     6,
     178,  -776,  -776,  -776,  -776,  -776,  -776,   838,   -58,  -776,
    -158,   -52,   860,    83,   865,  -776,  1083,  -776,  -776,  1014,
     611,   948,   869,   440,  -776,   838,  1087,  1084,  1046,  1048,
     463,   464,  -776,   884,   468,  -776,  1091,  -776,  -776,   -37,
     881,   582,  -776,    82,  -776,   446,   731,  -776,  -776,  1096,
    1165,  1165,   557,  1020,  1045,  1082,  1201,  1218,   974,   986,
    -776,  -776,   344,  -776,   971,   844,   481,   886,   982,  -776,
     953,  -776,  -776,   672,  -776,  -776,  -776,  -776,  -776,  -776,
      67,  -776,  -776,  -776,   897,   456,   187,  -776,   672,   385,
     672,   752,   906,  1115,   901,   672,  -776,   907,   900,   908,
     492,  -776,  -776,   521,  1122,  1123,  -776,  -776,  1037,   652,
    -776,  1041,   498,    42,    14,  1090,  -776,  -776,  -776,  -776,
    -776,  -776,  1095,  -776,  1129,  -776,  -776,  -776,  -776,  -776,
    -776,  -776,  -776,   914,  1088,   501,   924,   502,  -776,   926,
     927,   929,   930,   931,   933,   942,   943,   950,  1063,   951,
     954,   955,   957,   958,   962,   964,   965,   966,   967,  1098,
     969,   970,   976,   985,   989,   992,   996,   999,  1000,  1001,
    1103,  1002,  1003,  1004,  1007,  1008,  1017,  1018,  1019,  1021,
    1022,  1106,  1023,  1051,  1052,  1054,  1055,  1069,  1073,  1074,
    1075,  1086,  1108,  1093,  1099,  1101,  1102,  1104,  1105,  1107,
    1110,  1113,  1114,  1109,  1116,  -776,  -776,    18,  -776,  1068,
    1137,   503,  -776,  1041,  1212,  1228,   535,  -776,  -776,  -776,
     456,  -776,   125,  1130,    26,  1131,  -776,  -776,  -776,  1132,
    1167,  1015,   456,  -776,    67,  -776,  -776,  -776,  -776,  -776,
    -776,  -776,  -776,  -776,  -776,  1233,  -776,    82,   582,    14,
      14,  1080,   446,  1244,  1271,  -776,  1323,  -776,  -776,  1165,
    1326,  1335,  1353,  1354,  1355,  1356,  1357,  1358,  1359,  1140,
    1361,  1362,  1363,  1364,  1365,  1366,  1367,  1368,  1369,  1370,
    1151,  1372,  1373,  1374,  1375,  1376,  1377,  1378,  1379,  1380,
    1381,  1162,  1383,  1384,  1385,  1386,  1387,  1388,  1389,  1390,
    1391,  1392,  1173,  1394,  1395,  1396,  1397,  1398,  1399,  1400,
    1401,  1402,  1403,  1184,  1405,  1406,  1407,  1408,  1409,  1410,
    1411,  1412,  1413,  1414,  1195,  1416,  -776,  1419,  1420,  -776,
     537,  -776,   820,  -776,  1421,  1422,  1423,   240,  1205,  -776,
     539,  1424,  -776,  -776,  1371,   844,  -776,   672,   672,  -776,
    1208,  -776,  1210,  1211,  1213,  1214,  1215,  1216,  1217,  1219,
    1220,  1429,  1221,  1222,  1223,  1224,  1225,  1226,  1227,  1229,
    1230,  1231,  1435,  1232,  1234,  1235,  1236,  1237,  1238,  1239,
    1240,  1241,  1242,  1445,  1243,  1245,  1246,  1247,  1248,  1249,
    1250,  1251,  1252,  1253,  1450,  1254,  1255,  1256,  1257,  1258,
    1259,  1260,  1261,  1262,  1263,  1461,  1264,  1265,  1266,  1267,
    1268,  1269,  1270,  1272,  1273,  1274,  1488,  1275,  -776,  -776,
    -776,  -776,  1203,  1276,  1277,   901,  1334,  -776,   536,   672,
     541,   823,   456,  -776,  -776,  -776,  -776,  -776,  -776,  -776,
    -776,  -776,  -776,  1278,  -776,  -776,  -776,  -776,  -776,  -776,
    -776,  -776,  -776,  -776,  1281,  -776,  -776,  -776,  -776,  -776,
    -776,  -776,  -776,  -776,  -776,  1282,  -776,  -776,  -776,  -776,
    -776,  -776,  -776,  -776,  -776,  -776,  1283,  -776,  -776,  -776,
    -776,  -776,  -776,  -776,  -776,  -776,  -776,  1284,  -776,  -776,
    -776,  -776,  -776,  -776,  -776,  -776,  -776,  -776,  1285,  -776,
    1496,  1505,    74,  1287,  1288,  1464,  1507,  1508,  -776,  -776,
    -776,   456,  -776,  -776,  -776,  -776,  -776,  -776,  -776,  1289,
    1290,   901,   820,  1352,  1296,  1513,   686,   137,  1297,  1517,
    1299,  -776,  1475,  1520,   699,  1519,  -776,   901,   820,   901,
     -32,  1304,  1305,  1521,  -776,  1482,  1307,  -776,  1308,  1493,
    1494,  -776,  1530,  -776,  -776,  -776,     4,   -71,  -776,  1313,
    1314,  1497,  1499,  -776,  1500,  1501,  1538,  -776,  -776,  1320,
    -776,  1321,  1322,  1541,  1543,   820,  1324,  1325,  -776,   820,
    -776,  -776
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     236,     0,     0,     0,     0,     0,     0,     0,   236,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   236,     0,   549,     3,     5,    10,    12,    13,
      11,     6,     7,     9,   181,   180,     0,     8,    14,    15,
      16,    17,    18,    19,    20,   547,   547,   547,   547,   547,
       0,     0,   545,   545,   545,   545,   545,     0,   229,     0,
       0,     0,     0,     0,     0,   236,   167,    21,    26,    28,
      27,    22,    23,    25,    24,    29,    30,    31,    32,     0,
     293,   298,     0,     0,   250,   251,   249,   255,   259,     0,
     256,     0,     0,   252,     0,   254,   278,   279,     0,   257,
       0,   289,   291,   292,     0,   285,   301,   295,   300,     0,
       0,     0,   304,   305,   306,   307,   310,   229,   308,     0,
     235,   237,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   375,   333,     0,     0,     1,   236,
       2,   219,   221,   222,     0,   204,   186,   192,     0,     0,
       0,     0,     0,     0,     0,     0,   165,     0,     0,     0,
       0,     0,     0,   330,     0,     0,   214,     0,     0,     0,
       0,     0,     0,   166,     0,     0,     0,   266,   267,   260,
     261,   262,     0,   263,     0,   253,   280,     0,   258,   290,
       0,   283,   282,   286,   287,   303,     0,   297,     0,     0,
     335,     0,     0,     0,     0,     0,   357,     0,   367,     0,
     368,     0,   354,   355,     0,   350,     0,   363,   365,     0,
     358,     0,     0,     0,     0,     0,     0,   376,   185,   184,
       4,   220,     0,   182,   183,   203,     0,     0,   200,     0,
      34,     0,    35,   165,   550,     0,     0,     0,   236,   544,
     172,   174,   173,   175,     0,   230,     0,   214,   169,     0,
     161,   543,     0,     0,   473,   477,   480,   481,     0,     0,
       0,     0,     0,     0,     0,     0,   478,   479,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   475,     0,   236,     0,     0,   377,   382,   383,   397,
     395,   398,   396,   399,   400,   392,   387,   386,   385,   393,
     394,   384,   391,   390,   488,   491,     0,   492,   500,     0,
     501,     0,   493,   489,     0,   490,   515,     0,   516,     0,
     487,   314,   316,   315,   312,   313,   319,   321,   320,   317,
     318,   324,   326,   325,   322,   323,   288,   294,   299,     0,
       0,   269,   268,   274,   264,   265,   281,   284,   302,   296,
       0,     0,     0,     0,   553,     0,   238,   311,   360,     0,
     351,   356,     0,   364,   359,     0,     0,   366,     0,   331,
     332,     0,     0,   206,     0,     0,   202,   546,     0,   236,
       0,     0,     0,     0,     0,   329,   159,     0,     0,   163,
       0,     0,     0,   168,   213,     0,     0,     0,   524,   523,
     526,   525,   528,   527,   530,   529,   532,   531,   534,   533,
       0,     0,   439,   236,     0,     0,     0,     0,   482,   483,
     484,   485,     0,   486,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   441,   440,   521,
     518,   508,   498,   503,   506,     0,     0,   510,   511,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   497,     0,   502,     0,   505,     0,     0,   509,
     517,     0,   520,     0,   275,   270,     0,     0,     0,   334,
       0,   309,     0,   369,     0,   352,     0,     0,     0,     0,
     362,   189,   188,     0,   208,   191,   193,   198,   199,     0,
     187,    33,    37,     0,     0,     0,     0,    43,    47,    48,
     236,     0,    41,   328,   327,   164,     0,     0,   162,   176,
     171,   170,     0,     0,     0,   428,     0,   236,     0,     0,
       0,     0,     0,   464,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   212,     0,     0,
     389,   388,     0,   378,   381,   457,   458,     0,     0,   236,
       0,   438,   448,   449,   452,   453,     0,   455,   447,   450,
     451,   443,   442,   444,   445,   446,   474,   476,   499,     0,
     504,     0,   507,   512,   519,   522,     0,     0,   271,     0,
       0,   372,     0,   239,   353,     0,   336,   361,     0,     0,
     205,     0,   210,     0,   196,   197,   195,   201,     0,    55,
      58,    59,    56,    57,    60,    61,    77,    62,    64,    63,
      80,    67,    68,    69,    65,    66,    70,    71,    72,    73,
      74,    75,    76,     0,     0,     0,     0,     0,     0,     0,
       0,   553,     0,     0,   555,     0,    40,     0,     0,   160,
       0,     0,     0,     0,     0,     0,   539,     0,     0,   535,
       0,     0,   429,     0,   469,     0,     0,   462,     0,     0,
       0,   436,   435,   434,   433,   432,   431,     0,     0,   473,
       0,     0,     0,     0,     0,   418,     0,   514,   513,     0,
     236,   456,     0,     0,   437,     0,     0,     0,   276,   272,
       0,     0,    45,   558,     0,   556,   337,   370,   371,   236,
     207,   223,   225,   234,   226,     0,   214,   194,    39,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     152,   153,   156,   149,   156,     0,     0,     0,    36,    44,
     564,    42,   379,     0,   541,   540,   538,   537,   542,   179,
       0,   177,   430,   470,     0,   466,     0,   465,     0,     0,
       0,     0,     0,     0,   212,     0,   416,     0,     0,     0,
       0,   471,   460,   459,     0,     0,   374,   373,     0,     0,
     552,     0,     0,     0,     0,     0,   243,   244,   245,   246,
     242,   247,     0,   232,     0,   227,   422,   420,   423,   421,
     424,   425,   426,   209,   218,     0,     0,     0,    53,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   154,   151,     0,   150,    50,
      49,     0,   158,     0,     0,     0,     0,   536,   468,   463,
     467,   454,     0,     0,     0,     0,   494,   496,   495,   212,
       0,     0,   211,   419,     0,   472,   461,   277,   273,    46,
     559,   560,   562,   561,   557,     0,   338,   234,   224,     0,
       0,   231,     0,     0,   216,    79,     0,   147,   148,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   155,     0,     0,   157,
       0,    38,   553,   380,     0,     0,     0,     0,     0,   417,
       0,   339,   228,   240,     0,     0,   427,     0,     0,   190,
       0,    54,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    52,    51,
     554,   563,     0,     0,   212,   212,   412,   178,     0,     0,
       0,   217,   215,    78,    84,    85,    82,    83,    86,    87,
      88,    89,    90,     0,    81,   128,   129,   126,   127,   130,
     131,   132,   133,   134,     0,   125,    95,    96,    93,    94,
      97,    98,    99,   100,   101,     0,    92,   106,   107,   104,
     105,   108,   109,   110,   111,   112,     0,   103,   139,   140,
     137,   138,   141,   142,   143,   144,   145,     0,   136,   117,
     118,   115,   116,   119,   120,   121,   122,   123,     0,   114,
       0,     0,     0,     0,     0,     0,     0,     0,   341,   340,
     346,   241,   233,    91,   135,   102,   113,   146,   124,   212,
       0,   212,   553,   413,     0,   347,   342,     0,     0,     0,
       0,   411,     0,     0,     0,     0,   343,   212,   553,   212,
     553,     0,     0,     0,   348,   344,     0,   407,     0,     0,
       0,   410,     0,   414,   349,   345,   553,   401,   409,     0,
       0,     0,     0,   406,     0,     0,     0,   415,   405,     0,
     403,     0,     0,     0,     0,   553,     0,     0,   408,   553,
     402,   404
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -776,  -776,  -776,  1415,  1484,   276,  -776,  -776,   885,  -592,
    -776,  -670,  -776,   799,   802,  -776,  -596,   300,   307,  1309,
    -776,   311,  -776,  1153,   328,   342,    -6,  1531,   -18,  1192,
    1327,   -83,  -776,  -776,   935,  -776,  -776,  -776,  -776,  -776,
    -776,  -776,  -775,  -251,  -776,  -776,  -776,  -776,   756,  -170,
      31,   624,  -776,  -776,  1360,  -776,  -776,   402,   413,   415,
     430,   441,  -776,  -776,  -776,  -235,  -776,  1111,  -259,  -260,
    -719,  -708,  -697,  -694,  -693,  -680,   620,  -776,  -776,  -776,
    -776,  -776,  -776,  1139,  -776,  -776,  1009,  -291,  -285,  -776,
    -776,  -776,   794,  -776,  -776,  -776,  -776,   795,  -776,  -776,
    1081,  1085,   796,  -776,  -776,  -776,  -776,  1291,  -533,   797,
    -154,   643,   673,  -776,  -776,  -656,  -776,   665,   762,  -776
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    23,    24,    25,    66,    26,   526,   721,   527,   528,
     827,   661,   752,   753,   899,   529,   397,    27,    28,   248,
      29,    30,   257,   258,    31,    32,    33,    34,    35,   146,
     233,   147,   238,   515,   516,   626,   386,   520,   236,   514,
     622,   736,   704,   260,  1039,   944,   144,   730,   731,   732,
     733,   815,    36,   120,   121,   734,   812,    37,    38,    39,
      40,    41,    42,    43,    44,   295,   538,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   822,   823,   305,   306,
     307,   308,   309,   427,   310,   311,   312,   313,   314,   915,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     453,   454,   325,   326,   327,   328,   329,   330,   678,   679,
     262,   158,   149,   140,   154,   501,   758,   724,   725,   532
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     404,   383,    73,   458,   137,   754,   403,   452,   422,   921,
     680,   145,   426,   720,   263,   774,   816,    58,   722,   265,
     266,   267,   265,   266,   267,   449,   450,   817,   179,   447,
     448,   449,   450,   356,   456,    59,   202,    61,   818,   259,
      16,   819,   820,   513,   596,   500,   118,   699,   167,   168,
     141,   367,   142,   389,   425,   821,   461,   143,   686,    73,
     523,   234,   141,  1254,   142,   186,   481,   756,   781,   143,
     826,   828,   331,   398,   332,   333,   133,    45,    46,    47,
    1211,   500,   156,    48,    49,   813,   577,   784,    51,   777,
     195,   166,   685,   134,  1255,   208,   209,   462,   463,    58,
     210,   123,  1239,   462,   463,   197,   124,    60,   125,   336,
     126,   337,   338,    16,   178,   272,   273,   274,   272,   273,
     274,   275,   462,   463,   275,   517,   518,   462,   463,   341,
     334,   342,   343,  1240,   672,   673,   814,   187,  1251,   785,
     199,   200,   540,  1227,  1028,   674,   675,   676,   785,   382,
     276,   277,   278,   276,   277,   278,   196,   462,   463,   901,
     422,   462,   463,   628,   227,   550,   551,   339,   780,  1252,
     578,   198,   546,   109,   782,   556,   557,   558,   559,   560,
     561,   241,   242,   243,   524,    22,   525,   344,   460,   110,
     251,   252,   253,   462,   463,   169,   615,   603,   598,   164,
     402,   111,   785,   575,   576,   616,   929,   582,   583,   584,
     585,   586,   587,   588,   589,   590,   591,   592,   593,   594,
     595,   390,   122,   816,   580,   232,   624,   625,   180,   116,
     399,   462,   463,   364,   817,   292,   729,   335,   292,    50,
     292,   451,   457,   294,  1115,   818,   294,   451,   819,   820,
       1,   677,     2,     3,     4,     5,     6,     7,   597,     9,
     627,   291,   821,   127,   778,   937,    11,   462,   463,    12,
     909,    13,    14,    15,   340,   455,   392,   117,   620,  1041,
     462,   463,   581,   128,    67,   462,   463,   129,   131,   132,
     130,    52,    53,    54,   345,   689,   690,    55,    56,   571,
     193,   548,   670,   194,   698,   785,   701,  -551,    68,   681,
     462,   463,   462,   463,   504,    69,   466,   148,   711,    70,
      16,   462,   463,   505,   462,   463,   135,   264,   265,   266,
     267,   393,   394,   462,   463,   691,    71,  -552,  -552,  1193,
    1194,    67,   462,   463,   713,   462,   463,   138,    65,   395,
      72,  1024,     1,   692,     2,     3,     4,     5,     6,     7,
     466,     9,   748,   119,   517,    68,  1111,   500,    11,   181,
     182,    12,    69,    13,    14,    15,    70,   145,   693,   467,
     468,   469,   470,   522,   715,    62,    63,   472,   544,   694,
      64,  1030,   695,    71,   499,   139,   268,   269,  -552,  -552,
     476,   477,   478,   479,   480,   549,   270,    72,   271,   482,
      74,   155,    17,   696,   483,   163,   748,   749,   709,   750,
     751,    75,    16,    76,   272,   273,   274,   775,   484,   776,
     275,   539,   459,   485,  1218,   460,  1220,   779,    77,  1120,
      19,   473,   474,   475,   476,   477,   478,   479,   480,    78,
     148,    21,  1236,    57,  1238,   793,   141,   548,   142,   276,
     277,   278,   279,   143,   280,    22,   281,    74,   282,   157,
     283,   749,   284,   750,   751,   790,   897,   164,    75,   360,
      76,   285,   112,   113,   114,   824,   478,   479,   480,   264,
     265,   266,   267,   361,   362,    77,   913,   488,   489,   486,
     115,   508,   509,   286,   487,   287,    78,   288,   535,   349,
     490,   536,   350,   351,    17,   491,   466,   352,   353,   910,
     664,   912,   492,   665,   666,   165,   922,   493,   906,   683,
     553,   935,   554,   936,   555,   467,   468,   469,   470,   289,
     290,   291,    19,   472,   292,   170,   293,   545,   441,   171,
     442,   443,   294,    21,   444,   172,  1221,   174,   268,   269,
     687,   712,   688,   682,   555,   175,   460,    22,   270,   176,
     271,   177,  1237,   425,  1241,  1196,   185,   705,  1197,  1198,
     706,   188,   762,  1199,  1200,   460,   272,   273,   274,   189,
    1253,   769,   275,   548,   770,   183,   184,   473,   474,   475,
     476,   477,   478,   479,   480,   201,  1016,   771,   911,  1268,
     770,   462,   463,  1271,   264,   265,   266,   267,   190,   191,
     192,   276,   277,   278,   279,   286,   280,   287,   281,   288,
     282,   203,   283,   772,   284,   204,   460,   805,  -248,   806,
     807,   808,   809,   285,   810,   811,   829,   830,   831,   832,
     833,   205,   466,   834,   835,   930,   931,   932,   933,   206,
     836,   837,   838,   792,   222,   286,   460,   287,   207,   288,
     221,   467,   468,   469,   470,   264,   265,   266,   267,   472,
     607,   608,   839,   268,   269,   224,   796,   797,    16,   536,
     798,   800,   789,   270,   801,   271,   159,   160,   161,   162,
     223,   289,   290,   291,   902,   228,   292,   536,   293,   707,
     708,   272,   273,   274,   294,   926,   225,   275,   460,   150,
     151,   152,   153,   803,   945,   948,  1019,   946,   949,   536,
    1225,  1226,   226,   473,   474,   475,   476,   477,   478,   479,
     480,  1233,  1234,   229,   268,   269,   276,   277,   278,   279,
     231,   280,   235,   281,   270,   282,   271,   283,  1023,   284,
    1110,   460,  1117,   801,  1202,   770,   237,   536,   285,  1033,
    1034,   240,   272,   273,   274,   239,   244,   245,   275,  1122,
     264,   265,   266,   267,   246,   247,   464,   250,   465,   254,
     286,   249,   287,   255,   288,   256,   259,   261,   347,   346,
     348,   354,  1121,   355,   358,   357,   359,   276,   277,   278,
     279,   363,   280,   365,   281,   368,   282,   371,   283,   369,
     284,   370,   372,   373,   374,   376,   289,   290,   291,   285,
     375,   292,   377,   293,   378,   379,   380,   384,   387,   294,
     388,   264,   265,   266,   267,   466,   385,   396,   400,   420,
     421,   286,   405,   287,   401,   288,   407,   406,   428,   270,
    1201,   271,   429,   423,   467,   468,   469,   470,   471,   424,
     430,   431,   472,   434,   432,   435,   494,   272,   273,   274,
     436,   437,   495,   275,   438,   439,   440,   289,   290,   291,
     481,   445,   292,   446,   293,   496,   497,   500,   498,   503,
     294,   507,   506,   510,   511,   512,   521,   519,   533,   530,
     420,   531,   276,   277,   278,   279,   534,   280,   537,   281,
     270,   282,   271,   283,   542,   284,   473,   474,   475,   476,
     477,   478,   479,   480,   285,   543,  -548,   547,   272,   273,
     274,    16,   562,     1,   275,     2,     3,     4,     5,     6,
       7,     8,     9,    10,   563,   564,   286,   565,   287,    11,
     288,   566,    12,   567,    13,    14,    15,   568,   570,   569,
     572,   574,   292,   276,   277,   278,   279,   579,   280,   599,
     281,   601,   282,   604,   283,   605,   284,   606,   609,   610,
     611,   612,   289,   290,   291,   285,   614,   292,   617,   293,
     618,   613,   619,   621,   663,   294,   662,   623,   667,   669,
     668,   578,   684,    16,   671,   697,   702,   286,   462,   287,
       1,   288,     2,     3,     4,     5,     6,     7,     8,     9,
      10,   703,   710,   714,   718,   716,    11,   717,   719,    12,
     523,    13,    14,    15,   723,    79,   726,   727,   728,   460,
     735,   738,   739,   289,   290,   291,   740,   757,   292,   760,
     293,   761,   741,   742,   743,    80,   294,   744,   745,   746,
     747,   764,    81,   755,    82,    83,   765,    84,   763,   466,
     766,   767,    85,    86,   773,   768,   783,   787,   786,   707,
      16,   788,   791,   708,   795,   794,   799,   802,  -552,  -552,
     469,   470,   825,   897,   895,    17,  -552,   804,   903,   840,
     841,   842,   843,   844,   896,   904,   845,   846,   905,   919,
     908,    18,   924,   847,   848,   849,   914,   920,   927,   928,
     923,   925,   941,    19,   851,   852,   853,   854,   855,   943,
     942,   856,   857,    20,    21,   850,   939,   947,   858,   859,
     860,   940,   950,   951,   959,   952,   953,   954,    22,   955,
    -552,   474,   475,   476,   477,   478,   479,   480,   956,   957,
     861,   862,   863,   864,   865,   866,   958,   960,   867,   868,
     961,   962,    17,   963,   964,   869,   870,   871,   965,   970,
     966,   967,   968,   969,   981,   971,   972,   992,    18,  1003,
    1014,  1017,   973,    87,    88,    89,    90,   872,    91,    92,
      19,   974,    93,    94,    95,   975,  1021,    96,   976,    97,
      20,    21,   977,    98,    99,   978,   979,   980,   982,   983,
     984,  1022,   785,   985,   986,    22,   100,   101,  1029,  1031,
     102,   103,   104,   987,   988,   989,   105,   990,   991,   993,
     106,   107,   108,   629,   630,   631,   632,   633,   634,   635,
     636,   637,   638,   639,   640,   641,   642,   643,   644,   645,
    1018,   646,   647,   648,   649,   650,   651,   994,   995,   652,
     996,   997,   653,   654,   655,   656,   657,   658,   659,   660,
     873,   874,   875,   876,   877,   998,  1037,   878,   879,   999,
    1000,  1001,  1035,   211,   880,   881,   882,   884,   885,   886,
     887,   888,  1002,   212,   889,   890,   213,   214,   215,  1004,
     216,   891,   892,   893,  1038,  1005,   883,  1006,  1007,  1040,
    1008,  1009,  1042,  1010,   217,   218,  1011,   219,   220,  1012,
    1013,  1043,  1015,   894,   408,   409,   410,   411,   412,   413,
     414,   415,   416,   417,   418,   419,  1025,  1026,  1027,  1044,
    1045,  1046,  1047,  1048,  1049,  1050,  1051,  1052,  1053,  1054,
    1055,  1056,  1057,  1058,  1059,  1060,  1061,  1062,  1063,  1064,
    1065,  1066,  1067,  1068,  1069,  1070,  1071,  1072,  1073,  1074,
    1075,  1076,  1077,  1078,  1079,  1080,  1081,  1082,  1083,  1084,
    1085,  1086,  1087,  1088,  1089,  1090,  1091,  1092,  1093,  1094,
    1095,  1096,  1097,  1098,  1099,  1100,  1101,  1102,  1103,  1104,
    1105,  1106,  1107,  1108,  1109,  1112,  1113,  1114,  1116,  1190,
    1118,  1123,  1119,  1124,  1125,  1133,  1126,  1127,  1128,  1129,
    1130,  1144,  1131,  1132,  1134,  1135,  1136,  1137,  1138,  1139,
    1140,  1155,  1141,  1142,  1143,  1145,  1166,  1146,  1147,  1148,
    1149,  1150,  1151,  1152,  1153,  1154,  1156,  1177,  1157,  1158,
    1159,  1160,  1161,  1162,  1163,  1164,  1165,  1167,  1168,  1169,
    1170,  1171,  1172,  1173,  1174,  1175,  1176,  1178,  1179,  1180,
    1181,  1182,  1183,  1184,  1188,  1185,  1186,  1187,  1189,  1195,
    1209,  1203,  1191,  1192,  1204,  1205,  1206,  1207,  1208,  1210,
    1212,  1213,  1214,  1215,  1216,  1217,  1219,  1222,  1223,  1224,
    1228,  1229,  1230,  1231,  1232,  1235,  1242,  1244,  1243,  1245,
    1246,  1247,  1248,  1249,  1250,  1256,  1258,  1257,  1259,  1260,
    1261,  1262,  1263,  1264,  1266,  1265,  1267,  1269,  1270,   173,
     759,   898,   391,   136,   230,   541,   900,   502,   737,   381,
     938,  1032,  1036,   934,   366,   552,   600,   907,  1020,   433,
       0,   573,   602,   700,     0,     0,   916,   917,   918
};

static const yytype_int16 yycheck[] =
{
     259,   236,     8,   294,    22,   661,   257,   292,   268,   784,
     543,     8,   271,   609,   168,   685,   735,     3,   610,     4,
       5,     6,     4,     5,     6,     5,     6,   735,     6,   289,
     290,     5,     6,   187,   293,     4,    61,     6,   735,    65,
      77,   735,   735,    81,     3,    77,    15,     3,    75,    76,
      20,   205,    22,    54,    87,   735,    54,    27,    86,    65,
       3,   144,    20,   134,    22,     4,   224,   663,   226,    27,
     740,   741,     3,    37,     5,     6,    33,    32,    33,    34,
       6,    77,    51,    38,    39,     3,    74,     4,    36,    83,
       6,    60,    54,    50,   165,   185,   186,   155,   156,     3,
     190,     7,   134,   155,   156,     6,    12,    75,    14,     3,
      16,     5,     6,    77,    83,   100,   101,   102,   100,   101,
     102,   106,   155,   156,   106,   384,   385,   155,   156,     3,
      61,     5,     6,   165,    67,    68,    54,    76,   134,    65,
     109,   110,   401,     6,   919,    78,    79,    80,    65,   232,
     135,   136,   137,   135,   136,   137,    72,   155,   156,   755,
     420,   155,   156,   133,   133,   424,   425,    61,   226,   165,
     158,    72,   407,    33,   226,   434,   435,   436,   437,   438,
     439,   150,   151,   152,   127,   222,   129,    61,   226,    33,
     159,   160,   161,   155,   156,   222,    32,   488,   483,   224,
     226,    39,    65,   462,   463,    41,   798,   467,   468,   469,
     470,   471,   472,   473,   474,   475,   476,   477,   478,   479,
     480,   222,   172,   942,    72,   222,    69,    70,   206,     3,
     248,   155,   156,   202,   942,   220,   222,   168,   220,   194,
     220,   221,   227,   228,     4,   942,   228,   221,   942,   942,
       7,   184,     9,    10,    11,    12,    13,    14,   217,    16,
     519,   217,   942,   169,    86,   223,    23,   155,   156,    26,
      83,    28,    29,    30,   168,   293,   245,     3,   513,   949,
     155,   156,   130,   189,     8,   155,   156,   193,    32,    33,
     196,    32,    33,    34,   168,   554,   555,    38,    39,   223,
     202,    72,   537,   205,   563,    65,   565,    61,     8,   544,
     155,   156,   155,   156,    39,     8,   131,    71,   578,     8,
      77,   155,   156,    48,   155,   156,   159,     3,     4,     5,
       6,    32,    33,   155,   156,   223,     8,   152,   153,  1114,
    1115,    65,   155,   156,   579,   155,   156,     0,     3,    50,
       8,   226,     7,   223,     9,    10,    11,    12,    13,    14,
     131,    16,    72,     3,   623,    65,  1022,    77,    23,   173,
     174,    26,    65,    28,    29,    30,    65,     8,   223,   150,
     151,   152,   153,   389,   155,   166,   167,   158,    64,   223,
     171,   924,   223,    65,   363,   225,    72,    73,   213,   214,
     215,   216,   217,   218,   219,   423,    82,    65,    84,   221,
       8,     3,   169,   223,   226,     3,    72,   127,   572,   129,
     130,     8,    77,     8,   100,   101,   102,   686,   221,   688,
     106,   400,   223,   226,  1209,   226,  1211,   697,     8,  1035,
     197,   212,   213,   214,   215,   216,   217,   218,   219,     8,
      71,   208,  1227,   194,  1229,   715,    20,    72,    22,   135,
     136,   137,   138,    27,   140,   222,   142,    65,   144,    71,
     146,   127,   148,   129,   130,   710,   132,   224,    65,    10,
      65,   157,   159,   160,   161,   736,   217,   218,   219,     3,
       4,     5,     6,    24,    25,    65,   781,   226,   227,   221,
     177,   187,   188,   179,   226,   181,    65,   183,   223,    39,
     221,   226,    42,    43,   169,   226,   131,    47,    48,   778,
     223,   780,   221,   226,   530,    14,   785,   226,   763,   547,
      83,    33,    85,    35,    87,   150,   151,   152,   153,   215,
     216,   217,   197,   158,   220,     3,   222,   223,   120,     3,
     122,   123,   228,   208,   126,     3,  1212,   222,    72,    73,
      83,   579,    85,   223,    87,    72,   226,   222,    82,    72,
      84,     3,  1228,    87,  1230,    39,     3,   223,    42,    43,
     226,     6,   223,    47,    48,   226,   100,   101,   102,     3,
    1246,   223,   106,    72,   226,   173,   174,   212,   213,   214,
     215,   216,   217,   218,   219,     3,   897,   223,   223,  1265,
     226,   155,   156,  1269,     3,     4,     5,     6,   201,   202,
     203,   135,   136,   137,   138,   179,   140,   181,   142,   183,
     144,    54,   146,   223,   148,   226,   226,    55,    56,    57,
      58,    59,    60,   157,    62,    63,    89,    90,    91,    92,
      93,     6,   131,    96,    97,     3,     4,     5,     6,   194,
     103,   104,   105,   223,     4,   179,   226,   181,   194,   183,
     194,   150,   151,   152,   153,     3,     4,     5,     6,   158,
      44,    45,   125,    72,    73,    75,   223,   223,    77,   226,
     226,   223,   710,    82,   226,    84,    53,    54,    55,    56,
     192,   215,   216,   217,   223,   223,   220,   226,   222,     5,
       6,   100,   101,   102,   228,   223,   194,   106,   226,    46,
      47,    48,    49,   729,   223,   223,   223,   226,   226,   226,
      44,    45,   194,   212,   213,   214,   215,   216,   217,   218,
     219,    42,    43,   223,    72,    73,   135,   136,   137,   138,
      21,   140,    64,   142,    82,   144,    84,   146,   223,   148,
     223,   226,   223,   226,   223,   226,    66,   226,   157,   939,
     940,     3,   100,   101,   102,    72,     3,    61,   106,  1038,
       3,     4,     5,     6,    61,   222,    72,     3,    74,     3,
     179,    73,   181,     3,   183,     3,    65,     4,   130,   223,
     130,     3,  1037,     3,   130,     4,   130,   135,   136,   137,
     138,    61,   140,   222,   142,     4,   144,     3,   146,   165,
     148,     6,     6,     4,     3,    54,   215,   216,   217,   157,
       4,   220,     4,   222,   194,     3,     3,    52,    73,   228,
     133,     3,     4,     5,     6,   131,    67,     3,    61,    72,
      73,   179,    77,   181,   212,   183,   222,    77,     4,    82,
    1119,    84,     4,   222,   150,   151,   152,   153,   154,   222,
       4,     4,   158,   222,     6,   222,     3,   100,   101,   102,
     222,   222,     6,   106,   222,   222,   222,   215,   216,   217,
     224,   222,   220,   222,   222,    46,    46,    77,    76,     4,
     228,    76,     6,     4,   223,   223,     4,    68,     3,    54,
      72,   222,   135,   136,   137,   138,     3,   140,   222,   142,
      82,   144,    84,   146,   222,   148,   212,   213,   214,   215,
     216,   217,   218,   219,   157,   222,     0,   222,   100,   101,
     102,    77,     4,     7,   106,     9,    10,    11,    12,    13,
      14,    15,    16,    17,   222,   222,   179,   222,   181,    23,
     183,   222,    26,     4,    28,    29,    30,     4,   223,   229,
      76,     3,   220,   135,   136,   137,   138,   222,   140,     6,
     142,     6,   144,     6,   146,     5,   148,    42,   222,   222,
       3,   222,   215,   216,   217,   157,     6,   220,     4,   222,
     165,   223,   165,    75,   222,   228,   128,   226,     3,     3,
     133,   158,   223,    77,   226,    75,     4,   179,   155,   181,
       7,   183,     9,    10,    11,    12,    13,    14,    15,    16,
      17,   226,   222,   130,     6,   229,    23,   229,     6,    26,
       3,    28,    29,    30,     3,     3,     6,     4,     4,   226,
     178,     4,   222,   215,   216,   217,   222,    31,   220,   223,
     222,     4,   222,   222,   222,    23,   228,   222,   222,   222,
     222,     6,    30,   222,    32,    33,     6,    35,   222,   131,
       4,     3,    40,    41,   223,     6,   226,     4,   223,     5,
      77,    77,   223,     6,    46,    49,   212,     6,   150,   151,
     152,   153,     6,   132,   130,   169,   158,   226,   222,    89,
      90,    91,    92,    93,   128,   133,    96,    97,   165,     4,
     223,   185,   222,   103,   104,   105,   220,   226,     6,     6,
     223,   223,     3,   197,    89,    90,    91,    92,    93,    51,
     226,    96,    97,   207,   208,   125,    56,   223,   103,   104,
     105,    56,   226,   226,    91,   226,   226,   226,   222,   226,
     212,   213,   214,   215,   216,   217,   218,   219,   226,   226,
     125,    89,    90,    91,    92,    93,   226,   226,    96,    97,
     226,   226,   169,   226,   226,   103,   104,   105,   226,    91,
     226,   226,   226,   226,    91,   226,   226,    91,   185,    91,
      91,   133,   226,   161,   162,   163,   164,   125,   166,   167,
     197,   226,   170,   171,   172,   226,     4,   175,   226,   177,
     207,   208,   226,   181,   182,   226,   226,   226,   226,   226,
     226,     3,    65,   226,   226,   222,   194,   195,   223,     6,
     198,   199,   200,   226,   226,   226,   204,   226,   226,   226,
     208,   209,   210,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     133,   106,   107,   108,   109,   110,   111,   226,   226,   114,
     226,   226,   117,   118,   119,   120,   121,   122,   123,   124,
      89,    90,    91,    92,    93,   226,    52,    96,    97,   226,
     226,   226,   222,   160,   103,   104,   105,    89,    90,    91,
      92,    93,   226,   170,    96,    97,   173,   174,   175,   226,
     177,   103,   104,   105,    53,   226,   125,   226,   226,     6,
     226,   226,     6,   226,   191,   192,   226,   194,   195,   226,
     226,     6,   226,   125,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   226,   226,   226,     6,
       6,     6,     6,     6,     6,     6,   226,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   226,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   226,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   226,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     226,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   226,     6,     4,     4,     4,     4,     4,   223,   226,
       6,   223,    61,   223,   223,     6,   223,   223,   223,   223,
     223,     6,   223,   223,   223,   223,   223,   223,   223,   223,
     223,     6,   223,   223,   223,   223,     6,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,     6,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,     6,   223,   223,   223,   223,   165,
       4,   223,   226,   226,   223,   223,   223,   223,   223,     4,
     223,   223,    48,     6,     6,   226,   226,   165,   222,     6,
     223,     4,   223,    48,     4,     6,   222,     6,   223,    47,
     223,   223,    39,    39,     4,   222,    39,   223,    39,    39,
      39,     3,   222,   222,     3,   223,     3,   223,   223,    65,
     665,   752,   243,    22,   139,   402,   754,   365,   623,   232,
     804,   937,   942,   801,   204,   426,   485,   770,   903,   278,
      -1,   460,   487,   564,    -1,    -1,   782,   782,   782
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    23,    26,    28,    29,    30,    77,   169,   185,   197,
     207,   208,   222,   231,   232,   233,   235,   247,   248,   250,
     251,   254,   255,   256,   257,   258,   282,   287,   288,   289,
     290,   291,   292,   293,   294,    32,    33,    34,    38,    39,
     194,    36,    32,    33,    34,    38,    39,   194,     3,   280,
      75,   280,   166,   167,   171,     3,   234,   235,   247,   248,
     251,   254,   255,   256,   287,   288,   289,   290,   291,     3,
      23,    30,    32,    33,    35,    40,    41,   161,   162,   163,
     164,   166,   167,   170,   171,   172,   175,   177,   181,   182,
     194,   195,   198,   199,   200,   204,   208,   209,   210,    33,
      33,    39,   159,   160,   161,   177,     3,     3,   280,     3,
     283,   284,   172,     7,    12,    14,    16,   169,   189,   193,
     196,    32,    33,    33,    50,   159,   257,   258,     0,   225,
     343,    20,    22,    27,   276,     8,   259,   261,    71,   342,
     342,   342,   342,   342,   344,     3,   280,    71,   341,   341,
     341,   341,   341,     3,   224,    14,   280,    75,    76,   222,
       3,     3,     3,   234,   222,    72,    72,     3,   280,     6,
     206,   173,   174,   173,   174,     3,     4,    76,     6,     3,
     201,   202,   203,   202,   205,     6,    72,     6,    72,   280,
     280,     3,    61,    54,   226,     6,   194,   194,   185,   186,
     190,   160,   170,   173,   174,   175,   177,   191,   192,   194,
     195,   194,     4,   192,    75,   194,   194,   280,   223,   223,
     233,    21,   222,   260,   261,    64,   268,    66,   262,    72,
       3,   280,   280,   280,     3,    61,    61,   222,   249,    73,
       3,   280,   280,   280,     3,     3,     3,   252,   253,    65,
     273,     4,   340,   340,     3,     4,     5,     6,    72,    73,
      82,    84,   100,   101,   102,   106,   135,   136,   137,   138,
     140,   142,   144,   146,   148,   157,   179,   181,   183,   215,
     216,   217,   220,   222,   228,   295,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   308,   309,   310,   311,   312,
     314,   315,   316,   317,   318,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   332,   333,   334,   335,   336,
     337,     3,     5,     6,    61,   168,     3,     5,     6,    61,
     168,     3,     5,     6,    61,   168,   223,   130,   130,    39,
      42,    43,    47,    48,     3,     3,   340,     4,   130,   130,
      10,    24,    25,    61,   280,   222,   284,   340,     4,   165,
       6,     3,     6,     4,     3,     4,    54,     4,   194,     3,
       3,   260,   261,   295,    52,    67,   266,    73,   133,    54,
     222,   249,   280,    32,    33,    50,     3,   246,    37,   258,
      61,   212,   226,   273,   298,    77,    77,   222,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
      72,    73,   299,   222,   222,    87,   298,   313,     4,     4,
       4,     4,     6,   337,   222,   222,   222,   222,   222,   222,
     222,   120,   122,   123,   126,   222,   222,   299,   299,     5,
       6,   221,   318,   330,   331,   258,   298,   227,   317,   223,
     226,    54,   155,   156,    72,    74,   131,   150,   151,   152,
     153,   154,   158,   212,   213,   214,   215,   216,   217,   218,
     219,   224,   221,   226,   221,   226,   221,   226,   226,   227,
     221,   226,   221,   226,     3,     6,    46,    46,    76,   280,
      77,   345,   259,     4,    39,    48,     6,    76,   187,   188,
       4,   223,   223,    81,   269,   263,   264,   298,   298,    68,
     267,     4,   256,     3,   127,   129,   236,   238,   239,   245,
      54,   222,   349,     3,     3,   223,   226,   222,   296,   280,
     298,   253,   222,   222,    64,   223,   295,   222,    72,   258,
     298,   298,   313,    83,    85,    87,   298,   298,   298,   298,
     298,   298,     4,   222,   222,   222,   222,     4,     4,   229,
     223,   223,    76,   297,     3,   298,   298,    74,   158,   222,
      72,   130,   299,   299,   299,   299,   299,   299,   299,   299,
     299,   299,   299,   299,   299,   299,     3,   217,   318,     6,
     330,     6,   331,   317,     6,     5,    42,    44,    45,   222,
     222,     3,   222,   223,     6,    32,    41,     4,   165,   165,
     295,    75,   270,   226,    69,    70,   265,   298,   133,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   106,   107,   108,   109,
     110,   111,   114,   117,   118,   119,   120,   121,   122,   123,
     124,   241,   128,   222,   223,   226,   256,     3,   133,     3,
     295,   226,    67,    68,    78,    79,    80,   184,   338,   339,
     338,   295,   223,   258,   223,    54,    86,    83,    85,   298,
     298,   223,   223,   223,   223,   223,   223,    75,   298,     3,
     316,   298,     4,   226,   272,   223,   226,     5,     6,   340,
     222,   299,   258,   295,   130,   155,   229,   229,     6,     6,
     246,   237,   239,     3,   347,   348,     6,     4,     4,   222,
     277,   278,   279,   280,   285,   178,   271,   264,     4,   222,
     222,   222,   222,   222,   222,   222,   222,   222,    72,   127,
     129,   130,   242,   243,   345,   222,   246,    31,   346,   238,
     223,     4,   223,   222,     6,     6,     4,     3,     6,   223,
     226,   223,   223,   223,   241,   298,   298,    83,    86,   299,
     226,   226,   226,   226,     4,    65,   223,     4,    77,   258,
     295,   223,   223,   299,    49,    46,   223,   223,   226,   212,
     223,   226,     6,   256,   226,    55,    57,    58,    59,    60,
      62,    63,   286,     3,    54,   281,   300,   301,   302,   303,
     304,   305,   306,   307,   273,     6,   241,   240,   241,    89,
      90,    91,    92,    93,    96,    97,   103,   104,   105,   125,
      89,    90,    91,    92,    93,    96,    97,   103,   104,   105,
     125,    89,    90,    91,    92,    93,    96,    97,   103,   104,
     105,   125,    89,    90,    91,    92,    93,    96,    97,   103,
     104,   105,   125,    89,    90,    91,    92,    93,    96,    97,
     103,   104,   105,   125,    89,    90,    91,    92,    93,    96,
      97,   103,   104,   105,   125,   130,   128,   132,   243,   244,
     244,   246,   223,   222,   133,   165,   295,   339,   223,    83,
     298,   223,   298,   318,   220,   319,   322,   327,   332,     4,
     226,   272,   298,   223,   222,   223,   223,     6,     6,   239,
       3,     4,     5,     6,   348,    33,    35,   223,   278,    56,
      56,     3,   226,    51,   275,   223,   226,   223,   223,   226,
     226,   226,   226,   226,   226,   226,   226,   226,   226,    91,
     226,   226,   226,   226,   226,   226,   226,   226,   226,   226,
      91,   226,   226,   226,   226,   226,   226,   226,   226,   226,
     226,    91,   226,   226,   226,   226,   226,   226,   226,   226,
     226,   226,    91,   226,   226,   226,   226,   226,   226,   226,
     226,   226,   226,    91,   226,   226,   226,   226,   226,   226,
     226,   226,   226,   226,    91,   226,   317,   133,   133,   223,
     347,     4,     3,   223,   226,   226,   226,   226,   272,   223,
     338,     6,   281,   279,   279,   222,   306,    52,    53,   274,
       6,   241,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   226,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   226,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   226,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   226,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   226,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   226,     6,     4,     4,
     223,   345,     4,     4,     4,     4,   223,   223,     6,    61,
     246,   295,   298,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,     6,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,     6,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,     6,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,     6,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,     6,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,     6,   223,
     226,   226,   226,   272,   272,   165,    39,    42,    43,    47,
      48,   298,   223,   223,   223,   223,   223,   223,   223,     4,
       4,     6,   223,   223,    48,     6,     6,   226,   272,   226,
     272,   345,   165,   222,     6,    44,    45,     6,   223,     4,
     223,    48,     4,    42,    43,     6,   272,   345,   272,   134,
     165,   345,   222,   223,     6,    47,   223,   223,    39,    39,
       4,   134,   165,   345,   134,   165,   222,   223,    39,    39,
      39,    39,     3,   222,   222,   223,     3,     3,   345,   223,
     223,   345
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   230,   231,   232,   232,   233,   233,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   234,   235,   235,   235,   235,   235,   235,   235,
     235,   235,   235,   236,   236,   237,   237,   238,   238,   239,
     239,   239,   239,   240,   240,   241,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   242,
     242,   243,   243,   243,   243,   244,   244,   245,   245,   246,
     246,   247,   248,   248,   249,   249,   250,   250,   251,   252,
     252,   253,   254,   254,   254,   254,   254,   255,   255,   255,
     256,   256,   256,   256,   257,   257,   258,   259,   260,   260,
     261,   262,   262,   263,   263,   264,   265,   265,   265,   266,
     266,   267,   267,   268,   268,   269,   269,   270,   270,   271,
     271,   272,   272,   273,   273,   274,   274,   275,   275,   276,
     276,   276,   276,   277,   277,   278,   278,   279,   279,   280,
     280,   281,   281,   281,   281,   282,   282,   283,   283,   284,
     285,   285,   286,   286,   286,   286,   286,   286,   286,   287,
     287,   287,   287,   287,   287,   287,   287,   287,   287,   287,
     287,   287,   287,   287,   287,   287,   287,   287,   287,   287,
     287,   287,   287,   287,   287,   287,   287,   287,   287,   287,
     287,   287,   287,   287,   287,   287,   287,   287,   287,   287,
     287,   287,   287,   287,   287,   287,   287,   287,   287,   287,
     287,   287,   287,   287,   288,   288,   288,   288,   289,   289,
     290,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     290,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     290,   290,   290,   290,   290,   291,   292,   292,   292,   292,
     292,   292,   292,   292,   292,   292,   292,   292,   292,   292,
     292,   292,   292,   292,   292,   292,   292,   292,   292,   292,
     292,   292,   292,   292,   292,   292,   292,   292,   292,   292,
     292,   292,   293,   293,   293,   294,   294,   295,   295,   296,
     296,   297,   297,   298,   298,   298,   298,   298,   299,   299,
     299,   299,   299,   299,   299,   299,   299,   299,   299,   299,
     299,   300,   300,   300,   301,   301,   301,   301,   302,   302,
     302,   302,   303,   303,   303,   303,   304,   304,   305,   305,
     306,   306,   306,   306,   306,   306,   307,   307,   308,   308,
     308,   308,   308,   308,   308,   308,   308,   308,   308,   308,
     308,   308,   308,   308,   308,   308,   308,   308,   308,   308,
     308,   308,   308,   308,   308,   308,   308,   309,   309,   310,
     311,   311,   312,   312,   312,   312,   313,   313,   314,   315,
     315,   315,   315,   316,   316,   316,   316,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   317,
     318,   318,   318,   318,   319,   319,   319,   320,   321,   321,
     322,   322,   323,   324,   324,   325,   326,   326,   327,   328,
     328,   329,   329,   330,   331,   332,   332,   333,   334,   334,
     335,   336,   336,   337,   337,   337,   337,   337,   337,   337,
     337,   337,   337,   337,   337,   338,   338,   339,   339,   339,
     339,   339,   339,   340,   341,   341,   342,   342,   343,   343,
     344,   344,   345,   345,   346,   346,   347,   347,   348,   348,
     348,   348,   348,   349,   349
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     6,     4,     4,     8,     6,    10,     8,
       7,     6,     8,     1,     3,     1,     3,     1,     1,     4,
       4,     6,     6,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     6,     4,
       1,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     7,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     7,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     7,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     7,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     7,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     7,     4,     4,     1,
       2,     2,     1,     1,     2,     2,     0,     5,     4,     1,
       3,     4,     6,     5,     3,     0,     3,     2,     5,     1,
       3,     3,     4,     4,     4,     4,     6,     8,    11,     8,
       1,     1,     3,     3,     3,     3,     2,     4,     3,     3,
       9,     3,     0,     1,     3,     2,     1,     1,     0,     2,
       0,     2,     0,     1,     0,     2,     0,     2,     0,     2,
       0,     3,     0,     2,     0,     2,     0,     3,     0,     1,
       2,     1,     1,     1,     3,     1,     1,     2,     4,     1,
       3,     2,     1,     5,     0,     2,     0,     1,     3,     5,
       4,     6,     1,     1,     1,     1,     1,     1,     0,     2,
       2,     2,     2,     3,     2,     2,     2,     2,     3,     2,
       3,     3,     3,     3,     4,     4,     3,     3,     4,     4,
       5,     6,     7,     9,     4,     5,     7,     9,     2,     2,
       3,     4,     3,     3,     4,     2,     3,     3,     4,     2,
       3,     2,     2,     2,     4,     2,     4,     3,     2,     4,
       2,     2,     4,     3,     2,     2,     2,     2,     2,     5,
       2,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     6,     6,     5,
       3,     4,     4,     2,     5,     3,     6,     7,     9,    10,
      12,    12,    13,    14,    15,    16,    12,    13,    15,    16,
       3,     4,     5,     6,     3,     3,     4,     3,     3,     4,
       4,     6,     5,     3,     4,     3,     4,     3,     3,     5,
       7,     7,     6,     8,     8,     2,     3,     1,     3,     3,
       5,     3,     1,     1,     1,     1,     1,     1,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,    14,    19,    16,    20,    16,    15,    13,    18,    14,
      13,    11,     8,    10,    13,    15,     5,     7,     4,     6,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     4,
       5,     4,     4,     4,     4,     4,     4,     4,     3,     2,
       2,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     6,     3,     4,     3,     3,     5,
       5,     6,     4,     6,     3,     5,     4,     5,     6,     4,
       5,     5,     6,     1,     3,     1,     3,     1,     1,     1,
       1,     1,     2,     2,     2,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     2,     3,
       1,     1,     2,     2,     3,     2,     2,     3,     2,     2,
       2,     2,     3,     3,     3,     1,     1,     2,     2,     3,
       2,     2,     3,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     1,     3,     2,     2,     1,
       2,     2,     2,     1,     2,     0,     3,     0,     1,     0,
       2,     0,     4,     0,     4,     0,     1,     3,     1,     3,
       3,     3,     3,     6,     3
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
#line 331 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2495 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 331 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2503 "parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 248 "parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 2517 "parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 228 "parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 2531 "parser.cpp"
        break;

    case YYSYMBOL_column_def_array: /* column_def_array  */
#line 238 "parser.y"
            {
    fprintf(stderr, "destroy column def array\n");
    if ((((*yyvaluep).column_def_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).column_def_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).column_def_array_t));
    }
}
#line 2545 "parser.cpp"
        break;

    case YYSYMBOL_column_type_array: /* column_type_array  */
#line 192 "parser.y"
            {
    fprintf(stderr, "destroy column_type_array\n");
    delete (((*yyvaluep).column_type_array_t));
}
#line 2554 "parser.cpp"
        break;

    case YYSYMBOL_column_type: /* column_type  */
#line 187 "parser.y"
            {
    fprintf(stderr, "destroy column_type\n");
    delete (((*yyvaluep).column_type_t));
}
#line 2563 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2574 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2582 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 335 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2591 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 335 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2600 "parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 295 "parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 2614 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 288 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2625 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 370 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2635 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 370 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2645 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 370 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2655 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 370 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2665 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 370 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2675 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 370 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2685 "parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 278 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2699 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 278 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2713 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 358 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2723 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2731 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2739 "parser.cpp"
        break;

    case YYSYMBOL_highlight_clause: /* highlight_clause  */
#line 258 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2753 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 353 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2762 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2770 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2778 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2786 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2794 "parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 258 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2808 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 353 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2817 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 353 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2826 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 353 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2835 "parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 315 "parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2848 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 348 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2857 "parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 305 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2871 "parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 305 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2885 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 364 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2895 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 353 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2904 "parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 258 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2918 "parser.cpp"
        break;

    case YYSYMBOL_insert_row_list: /* insert_row_list  */
#line 268 "parser.y"
            {
    fprintf(stderr, "destroy insert row list\n");
    if ((((*yyvaluep).insert_row_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).insert_row_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).insert_row_list_t));
    }
}
#line 2932 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2940 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2948 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2956 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2964 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2972 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2980 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2988 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2996 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3004 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3012 "parser.cpp"
        break;

    case YYSYMBOL_sub_search_array: /* sub_search_array  */
#line 258 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 3026 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3034 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3042 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3050 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3058 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3066 "parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 376 "parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 3079 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3087 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3095 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3103 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3111 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3119 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3127 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3135 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3143 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3151 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3159 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3167 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3175 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3183 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3191 "parser.cpp"
        break;

    case YYSYMBOL_curly_brackets_expr: /* curly_brackets_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3199 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_curly_brackets_expr: /* unclosed_curly_brackets_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3207 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 385 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3215 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 389 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3223 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3231 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3239 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3247 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3255 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3263 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3271 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 331 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3279 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 221 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3290 "parser.cpp"
        break;

    case YYSYMBOL_with_index_param_list: /* with_index_param_list  */
#line 204 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 3304 "parser.cpp"
        break;

    case YYSYMBOL_optional_table_properties_list: /* optional_table_properties_list  */
#line 204 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 3318 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 197 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3329 "parser.cpp"
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
#line 89 "parser.y"
{
  // Initialize
  yylloc.first_column = 0;
  yylloc.last_column = 0;
  yylloc.first_line = 0;
  yylloc.last_line = 0;
  yylloc.total_column = 0;
  yylloc.string_length = 0;
}

#line 3437 "parser.cpp"

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
#line 521 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3652 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 525 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3663 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 531 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3674 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 538 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3680 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 539 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3686 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 540 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3692 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 541 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3698 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 542 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3704 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 543 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3710 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 544 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3716 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 545 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3722 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 546 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3728 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 547 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3734 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 548 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3740 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 549 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3746 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 550 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3752 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 551 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3758 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 552 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3764 "parser.cpp"
    break;

  case 20: /* statement: check_statement  */
#line 553 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].check_stmt); }
#line 3770 "parser.cpp"
    break;

  case 21: /* explainable_statement: create_statement  */
#line 555 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3776 "parser.cpp"
    break;

  case 22: /* explainable_statement: drop_statement  */
#line 556 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3782 "parser.cpp"
    break;

  case 23: /* explainable_statement: copy_statement  */
#line 557 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3788 "parser.cpp"
    break;

  case 24: /* explainable_statement: show_statement  */
#line 558 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3794 "parser.cpp"
    break;

  case 25: /* explainable_statement: select_statement  */
#line 559 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3800 "parser.cpp"
    break;

  case 26: /* explainable_statement: delete_statement  */
#line 560 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3806 "parser.cpp"
    break;

  case 27: /* explainable_statement: update_statement  */
#line 561 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3812 "parser.cpp"
    break;

  case 28: /* explainable_statement: insert_statement  */
#line 562 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3818 "parser.cpp"
    break;

  case 29: /* explainable_statement: flush_statement  */
#line 563 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3824 "parser.cpp"
    break;

  case 30: /* explainable_statement: optimize_statement  */
#line 564 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3830 "parser.cpp"
    break;

  case 31: /* explainable_statement: command_statement  */
#line 565 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3836 "parser.cpp"
    break;

  case 32: /* explainable_statement: compact_statement  */
#line 566 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3842 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER COMMENT STRING  */
#line 573 "parser.y"
                                                                           {
    (yyval.create_stmt) = new infinity::CreateStatement();
    std::shared_ptr<infinity::CreateSchemaInfo> create_schema_info = std::make_shared<infinity::CreateSchemaInfo>();

    ParserHelper::ToLower((yyvsp[-2].str_value));
    create_schema_info->schema_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    if(create_schema_info->schema_name_.empty()) {
        yyerror(&yyloc, scanner, result, "Empty database name is given.");
        YYERROR;
    }

    (yyval.create_stmt)->create_info_ = create_schema_info;
    (yyval.create_stmt)->create_info_->conflict_type_ = (yyvsp[-3].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
    (yyval.create_stmt)->create_info_->comment_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 3864 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 590 "parser.y"
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
#line 3884 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 607 "parser.y"
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
#line 3902 "parser.cpp"
    break;

  case 36: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 623 "parser.y"
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
#line 3935 "parser.cpp"
    break;

  case 37: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 652 "parser.y"
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
#line 3955 "parser.cpp"
    break;

  case 38: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list COMMENT STRING  */
#line 667 "parser.y"
                                                                                                                  {
    (yyval.create_stmt) = new infinity::CreateStatement();
    std::shared_ptr<infinity::CreateTableInfo> create_table_info = std::make_shared<infinity::CreateTableInfo>();
    if((yyvsp[-6].table_name_t)->schema_name_ptr_ != nullptr) {
        create_table_info->schema_name_ = (yyvsp[-6].table_name_t)->schema_name_ptr_;
        free((yyvsp[-6].table_name_t)->schema_name_ptr_);
    }
    create_table_info->table_name_ = (yyvsp[-6].table_name_t)->table_name_ptr_;
    free((yyvsp[-6].table_name_t)->table_name_ptr_);
    delete (yyvsp[-6].table_name_t);

    for (infinity::TableElement*& element : *(yyvsp[-4].table_element_array_t)) {
        if(element->type_ == infinity::TableElementType::kColumn) {
            create_table_info->column_defs_.emplace_back((infinity::ColumnDef*)element);
        } else {
            create_table_info->constraints_.emplace_back((infinity::TableConstraint*)element);
        }
    }
    delete (yyvsp[-4].table_element_array_t);

    if ((yyvsp[-2].with_index_param_list_t) != nullptr) {
        create_table_info->properties_ = std::move(*(yyvsp[-2].with_index_param_list_t));
        delete (yyvsp[-2].with_index_param_list_t);
    }

    create_table_info->comment_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.create_stmt)->create_info_ = create_table_info;
    (yyval.create_stmt)->create_info_->conflict_type_ = (yyvsp[-7].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 3991 "parser.cpp"
    break;

  case 39: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement COMMENT STRING  */
#line 699 "parser.y"
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

    create_table_info->conflict_type_ = (yyvsp[-5].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
    create_table_info->select_ = (yyvsp[-2].select_stmt);
    create_table_info->comment_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    (yyval.create_stmt)->create_info_ = create_table_info;
}
#line 4013 "parser.cpp"
    break;

  case 40: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 717 "parser.y"
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
#line 4034 "parser.cpp"
    break;

  case 41: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info  */
#line 735 "parser.y"
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
#line 4067 "parser.cpp"
    break;

  case 42: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info COMMENT STRING  */
#line 763 "parser.y"
                                                                          {
    std::shared_ptr<infinity::CreateIndexInfo> create_index_info = std::make_shared<infinity::CreateIndexInfo>();
    if((yyvsp[-3].table_name_t)->schema_name_ptr_ != nullptr) {
        create_index_info->schema_name_ = (yyvsp[-3].table_name_t)->schema_name_ptr_;
        free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    }
    create_index_info->table_name_ = (yyvsp[-3].table_name_t)->table_name_ptr_;
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);

    create_index_info->index_name_ = (yyvsp[-5].if_not_exists_info_t)->info_;
    if ((yyvsp[-5].if_not_exists_info_t)->exists_) {
        create_index_info->conflict_type_ = (yyvsp[-5].if_not_exists_info_t)->if_not_exists_ ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
    } else {
        create_index_info->conflict_type_ = infinity::ConflictType::kIgnore;
    }
    delete (yyvsp[-5].if_not_exists_info_t);

    create_index_info->index_info_ = (yyvsp[-2].index_info_t);
    create_index_info->comment_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    if(create_index_info->index_name_.empty()) {
        yyerror(&yyloc, scanner, result, "No index name");
        YYERROR;
    }

    (yyval.create_stmt) = new infinity::CreateStatement();
    (yyval.create_stmt)->create_info_ = create_index_info;
}
#line 4102 "parser.cpp"
    break;

  case 43: /* table_element_array: table_element  */
#line 794 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 4111 "parser.cpp"
    break;

  case 44: /* table_element_array: table_element_array ',' table_element  */
#line 798 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 4120 "parser.cpp"
    break;

  case 45: /* column_def_array: table_column  */
#line 803 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 4129 "parser.cpp"
    break;

  case 46: /* column_def_array: column_def_array ',' table_column  */
#line 807 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 4138 "parser.cpp"
    break;

  case 47: /* table_element: table_column  */
#line 813 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 4146 "parser.cpp"
    break;

  case 48: /* table_element: table_constraint  */
#line 816 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 4154 "parser.cpp"
    break;

  case 49: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 823 "parser.y"
                                                          {
    std::shared_ptr<infinity::ParsedExpr> default_expr((yyvsp[0].const_expr_t));
    std::vector<std::unique_ptr<infinity::InitParameter>> index_param_list = infinity::InitParameter::MakeInitParameterList((yyvsp[-1].with_index_param_list_t));
    auto [data_type_result, fail_reason] = infinity::ColumnType::GetDataTypeFromColumnType(*((yyvsp[-2].column_type_t)), index_param_list);
    delete (yyvsp[-2].column_type_t);
    if (!data_type_result) {
        yyerror(&yyloc, scanner, result, fail_reason.c_str());
        free((yyvsp[-3].str_value));
        YYERROR;
    }
    (yyval.table_column_t) = new infinity::ColumnDef(std::move(data_type_result), "", std::move(default_expr));
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 4179 "parser.cpp"
    break;

  case 50: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 843 "parser.y"
                                                         {
    std::shared_ptr<infinity::ParsedExpr> default_expr((yyvsp[0].const_expr_t));
    auto [data_type_result, fail_reason] = infinity::ColumnType::GetDataTypeFromColumnType(*((yyvsp[-2].column_type_t)), std::vector<std::unique_ptr<infinity::InitParameter>>{});
    delete (yyvsp[-2].column_type_t);
    if (!data_type_result) {
        yyerror(&yyloc, scanner, result, fail_reason.c_str());
        free((yyvsp[-3].str_value));
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyval.table_column_t) = new infinity::ColumnDef(std::move(data_type_result), "", std::move(default_expr));
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    (yyval.table_column_t)->constraints_ = std::move(*(yyvsp[-1].column_constraints_t));
    delete (yyvsp[-1].column_constraints_t);
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 4206 "parser.cpp"
    break;

  case 51: /* table_column: IDENTIFIER column_type with_index_param_list default_expr COMMENT STRING  */
#line 865 "parser.y"
                                                                           {
    std::shared_ptr<infinity::ParsedExpr> default_expr((yyvsp[-2].const_expr_t));
    std::vector<std::unique_ptr<infinity::InitParameter>> index_param_list = infinity::InitParameter::MakeInitParameterList((yyvsp[-3].with_index_param_list_t));
    auto [data_type_result, fail_reason] = infinity::ColumnType::GetDataTypeFromColumnType(*((yyvsp[-4].column_type_t)), index_param_list);
    delete (yyvsp[-4].column_type_t);
    if (!data_type_result) {
        yyerror(&yyloc, scanner, result, fail_reason.c_str());
        free((yyvsp[-5].str_value));
        free((yyvsp[0].str_value));
        YYERROR;
    }

    (yyval.table_column_t) = new infinity::ColumnDef(std::move(data_type_result), (yyvsp[0].str_value), std::move(default_expr));
    free((yyvsp[0].str_value));

    ParserHelper::ToLower((yyvsp[-5].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-5].str_value);
    free((yyvsp[-5].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 4235 "parser.cpp"
    break;

  case 52: /* table_column: IDENTIFIER column_type column_constraints default_expr COMMENT STRING  */
#line 889 "parser.y"
                                                                        {
    std::shared_ptr<infinity::ParsedExpr> default_expr((yyvsp[-2].const_expr_t));
    auto [data_type_result, fail_reason] = infinity::ColumnType::GetDataTypeFromColumnType(*((yyvsp[-4].column_type_t)), std::vector<std::unique_ptr<infinity::InitParameter>>{});
    delete (yyvsp[-4].column_type_t);
    if (!data_type_result) {
        yyerror(&yyloc, scanner, result, fail_reason.c_str());
        free((yyvsp[-5].str_value));
        free((yyvsp[0].str_value));
        delete (yyvsp[-3].column_constraints_t);
        YYERROR;
    }
    (yyval.table_column_t) = new infinity::ColumnDef(std::move(data_type_result), (yyvsp[0].str_value), std::move(default_expr));
    free((yyvsp[0].str_value));

    ParserHelper::ToLower((yyvsp[-5].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-5].str_value);
    (yyval.table_column_t)->constraints_ = std::move(*(yyvsp[-3].column_constraints_t));
    delete (yyvsp[-3].column_constraints_t);
    free((yyvsp[-5].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 4265 "parser.cpp"
    break;

  case 53: /* column_type_array: column_type  */
#line 915 "parser.y"
                                {
    (yyval.column_type_array_t) = new std::vector<std::unique_ptr<infinity::ColumnType>>();
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4274 "parser.cpp"
    break;

  case 54: /* column_type_array: column_type_array ',' column_type  */
#line 919 "parser.y"
                                    {
    (yyval.column_type_array_t) = (yyvsp[-2].column_type_array_t);
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4283 "parser.cpp"
    break;

  case 55: /* column_type: BOOLEAN  */
#line 925 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4289 "parser.cpp"
    break;

  case 56: /* column_type: TINYINT  */
#line 926 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4295 "parser.cpp"
    break;

  case 57: /* column_type: SMALLINT  */
#line 927 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4301 "parser.cpp"
    break;

  case 58: /* column_type: INTEGER  */
#line 928 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4307 "parser.cpp"
    break;

  case 59: /* column_type: INT  */
#line 929 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4313 "parser.cpp"
    break;

  case 60: /* column_type: BIGINT  */
#line 930 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4319 "parser.cpp"
    break;

  case 61: /* column_type: HUGEINT  */
#line 931 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4325 "parser.cpp"
    break;

  case 62: /* column_type: FLOAT  */
#line 932 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4331 "parser.cpp"
    break;

  case 63: /* column_type: REAL  */
#line 933 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4337 "parser.cpp"
    break;

  case 64: /* column_type: DOUBLE  */
#line 934 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4343 "parser.cpp"
    break;

  case 65: /* column_type: FLOAT16  */
#line 935 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4349 "parser.cpp"
    break;

  case 66: /* column_type: BFLOAT16  */
#line 936 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4355 "parser.cpp"
    break;

  case 67: /* column_type: DATE  */
#line 937 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4361 "parser.cpp"
    break;

  case 68: /* column_type: TIME  */
#line 938 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4367 "parser.cpp"
    break;

  case 69: /* column_type: DATETIME  */
#line 939 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4373 "parser.cpp"
    break;

  case 70: /* column_type: TIMESTAMP  */
#line 940 "parser.y"
            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4379 "parser.cpp"
    break;

  case 71: /* column_type: UUID  */
#line 941 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4385 "parser.cpp"
    break;

  case 72: /* column_type: POINT  */
#line 942 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4391 "parser.cpp"
    break;

  case 73: /* column_type: LINE  */
#line 943 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4397 "parser.cpp"
    break;

  case 74: /* column_type: LSEG  */
#line 944 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4403 "parser.cpp"
    break;

  case 75: /* column_type: BOX  */
#line 945 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4409 "parser.cpp"
    break;

  case 76: /* column_type: CIRCLE  */
#line 948 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4415 "parser.cpp"
    break;

  case 77: /* column_type: VARCHAR  */
#line 950 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4421 "parser.cpp"
    break;

  case 78: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 951 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4427 "parser.cpp"
    break;

  case 79: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 952 "parser.y"
                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4433 "parser.cpp"
    break;

  case 80: /* column_type: DECIMAL  */
#line 953 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4439 "parser.cpp"
    break;

  case 81: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 956 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4445 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 957 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4451 "parser.cpp"
    break;

  case 83: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 958 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4457 "parser.cpp"
    break;

  case 84: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 959 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4463 "parser.cpp"
    break;

  case 85: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 960 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4469 "parser.cpp"
    break;

  case 86: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 961 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4475 "parser.cpp"
    break;

  case 87: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 962 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4481 "parser.cpp"
    break;

  case 88: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 963 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4487 "parser.cpp"
    break;

  case 89: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 964 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4493 "parser.cpp"
    break;

  case 90: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 965 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4499 "parser.cpp"
    break;

  case 91: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 966 "parser.y"
                                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4505 "parser.cpp"
    break;

  case 92: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 967 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4511 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 968 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4517 "parser.cpp"
    break;

  case 94: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 969 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4523 "parser.cpp"
    break;

  case 95: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 970 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4529 "parser.cpp"
    break;

  case 96: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 971 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4535 "parser.cpp"
    break;

  case 97: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 972 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4541 "parser.cpp"
    break;

  case 98: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 973 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4547 "parser.cpp"
    break;

  case 99: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 974 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4553 "parser.cpp"
    break;

  case 100: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 975 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4559 "parser.cpp"
    break;

  case 101: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 976 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4565 "parser.cpp"
    break;

  case 102: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 977 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4571 "parser.cpp"
    break;

  case 103: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 978 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4577 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 979 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4583 "parser.cpp"
    break;

  case 105: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 980 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4589 "parser.cpp"
    break;

  case 106: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 981 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4595 "parser.cpp"
    break;

  case 107: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 982 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4601 "parser.cpp"
    break;

  case 108: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 983 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4607 "parser.cpp"
    break;

  case 109: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 984 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4613 "parser.cpp"
    break;

  case 110: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 985 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4619 "parser.cpp"
    break;

  case 111: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 986 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4625 "parser.cpp"
    break;

  case 112: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 987 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4631 "parser.cpp"
    break;

  case 113: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 988 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4637 "parser.cpp"
    break;

  case 114: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 989 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4643 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 990 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4649 "parser.cpp"
    break;

  case 116: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 991 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4655 "parser.cpp"
    break;

  case 117: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 992 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4661 "parser.cpp"
    break;

  case 118: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 993 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4667 "parser.cpp"
    break;

  case 119: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 994 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4673 "parser.cpp"
    break;

  case 120: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 995 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4679 "parser.cpp"
    break;

  case 121: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 996 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4685 "parser.cpp"
    break;

  case 122: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 997 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4691 "parser.cpp"
    break;

  case 123: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 998 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4697 "parser.cpp"
    break;

  case 124: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 999 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4703 "parser.cpp"
    break;

  case 125: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 1000 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4709 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1001 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4715 "parser.cpp"
    break;

  case 127: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1002 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4721 "parser.cpp"
    break;

  case 128: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1003 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4727 "parser.cpp"
    break;

  case 129: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1004 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4733 "parser.cpp"
    break;

  case 130: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1005 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4739 "parser.cpp"
    break;

  case 131: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1006 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4745 "parser.cpp"
    break;

  case 132: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1007 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4751 "parser.cpp"
    break;

  case 133: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1008 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4757 "parser.cpp"
    break;

  case 134: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1009 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4763 "parser.cpp"
    break;

  case 135: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4769 "parser.cpp"
    break;

  case 136: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1011 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4775 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1012 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4781 "parser.cpp"
    break;

  case 138: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1013 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4787 "parser.cpp"
    break;

  case 139: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1014 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4793 "parser.cpp"
    break;

  case 140: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4799 "parser.cpp"
    break;

  case 141: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4805 "parser.cpp"
    break;

  case 142: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1017 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4811 "parser.cpp"
    break;

  case 143: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1018 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4817 "parser.cpp"
    break;

  case 144: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1019 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4823 "parser.cpp"
    break;

  case 145: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1020 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4829 "parser.cpp"
    break;

  case 146: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1021 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4835 "parser.cpp"
    break;

  case 147: /* column_type: ARRAY '(' column_type ')'  */
#line 1022 "parser.y"
                            {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kArray, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_.emplace_back((yyvsp[-1].column_type_t));
}
#line 4844 "parser.cpp"
    break;

  case 148: /* column_type: TUPLE '(' column_type_array ')'  */
#line 1026 "parser.y"
                                  {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTuple, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_ = std::move(*((yyvsp[-1].column_type_array_t)));
  delete (yyvsp[-1].column_type_array_t);
}
#line 4854 "parser.cpp"
    break;

  case 149: /* column_constraints: column_constraint  */
#line 1049 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4863 "parser.cpp"
    break;

  case 150: /* column_constraints: column_constraints column_constraint  */
#line 1053 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4877 "parser.cpp"
    break;

  case 151: /* column_constraint: PRIMARY KEY  */
#line 1063 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4885 "parser.cpp"
    break;

  case 152: /* column_constraint: UNIQUE  */
#line 1066 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4893 "parser.cpp"
    break;

  case 153: /* column_constraint: NULLABLE  */
#line 1069 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4901 "parser.cpp"
    break;

  case 154: /* column_constraint: NOT NULLABLE  */
#line 1072 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4909 "parser.cpp"
    break;

  case 155: /* default_expr: DEFAULT constant_expr  */
#line 1076 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4917 "parser.cpp"
    break;

  case 156: /* default_expr: %empty  */
#line 1079 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4925 "parser.cpp"
    break;

  case 157: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1084 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4935 "parser.cpp"
    break;

  case 158: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1089 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4945 "parser.cpp"
    break;

  case 159: /* identifier_array: IDENTIFIER  */
#line 1096 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4956 "parser.cpp"
    break;

  case 160: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1102 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4967 "parser.cpp"
    break;

  case 161: /* delete_statement: DELETE FROM table_name where_clause  */
#line 1112 "parser.y"
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
#line 4984 "parser.cpp"
    break;

  case 162: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES insert_row_list  */
#line 1128 "parser.y"
                                                                                          {
    bool is_error{false};
    for (auto expr_array : *(yyvsp[0].insert_row_list_t)) {
        for (const auto &expr : expr_array->values_) {
            if(expr->type_ != infinity::ParsedExprType::kConstant) {
                yyerror(&yyloc, scanner, result, ("Value list has non-constant expression: " + expr->ToString()).c_str());
                is_error = true;
            }
        }
    }
    if(is_error) {
        for (auto expr_array : *(yyvsp[0].insert_row_list_t)) {
            delete (expr_array);
        }
        delete (yyvsp[0].insert_row_list_t);
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
    for (infinity::InsertRowExpr* &expr_ptr : *(yyvsp[0].insert_row_list_t)) {
        if ((yyvsp[-2].identifier_array_t)) {
            expr_ptr->columns_ = *((yyvsp[-2].identifier_array_t));
        }
        (yyval.insert_stmt)->insert_rows_.emplace_back(expr_ptr);
        expr_ptr = nullptr;
    }
    delete (yyvsp[-2].identifier_array_t);
    delete (yyvsp[0].insert_row_list_t);
}
#line 5027 "parser.cpp"
    break;

  case 163: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1166 "parser.y"
                                                                        {
    (yyval.insert_stmt) = new infinity::InsertStatement();
    if((yyvsp[-2].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.insert_stmt)->schema_name_ = (yyvsp[-2].table_name_t)->schema_name_ptr_;
        free((yyvsp[-2].table_name_t)->schema_name_ptr_);
    }
    (yyval.insert_stmt)->table_name_ = (yyvsp[-2].table_name_t)->table_name_ptr_;
    free((yyvsp[-2].table_name_t)->table_name_ptr_);
    delete (yyvsp[-2].table_name_t);
    if ((yyvsp[-1].identifier_array_t)) {
        (yyval.insert_stmt)->columns_for_select_ = std::move(*((yyvsp[-1].identifier_array_t)));
        delete (yyvsp[-1].identifier_array_t);
    }
    (yyval.insert_stmt)->select_.reset((yyvsp[0].select_stmt));
}
#line 5047 "parser.cpp"
    break;

  case 164: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1182 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 5055 "parser.cpp"
    break;

  case 165: /* optional_identifier_array: %empty  */
#line 1185 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 5063 "parser.cpp"
    break;

  case 166: /* explain_statement: EXPLAIN IDENTIFIER explainable_statement  */
#line 1192 "parser.y"
                                                             {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    ParserHelper::ToLower((yyvsp[-1].str_value));
    if(!strcmp((yyvsp[-1].str_value), "analyze")) (yyval.explain_stmt)->type_ = infinity::ExplainType::kAnalyze;
    else if(!strcmp((yyvsp[-1].str_value), "ast")) (yyval.explain_stmt)->type_ =infinity::ExplainType::kAst;
    else if(!strcmp((yyvsp[-1].str_value), "raw")) (yyval.explain_stmt)->type_ =infinity::ExplainType::kUnOpt;
    else if(!strcmp((yyvsp[-1].str_value), "logical")) (yyval.explain_stmt)->type_ =infinity::ExplainType::kOpt;
    else if(!strcmp((yyvsp[-1].str_value), "physical")) (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    else if(!strcmp((yyvsp[-1].str_value), "pipeline")) (yyval.explain_stmt)->type_ =infinity::ExplainType::kPipeline;
    else if(!strcmp((yyvsp[-1].str_value), "fragment")) (yyval.explain_stmt)->type_ =infinity::ExplainType::kFragment;
    else {
        free (yyvsp[-1].str_value);
        yyerror(&yyloc, scanner, result, "invalid explain option");
        YYERROR;
    }
    free((yyvsp[-1].str_value));
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 5086 "parser.cpp"
    break;

  case 167: /* explain_statement: EXPLAIN explainable_statement  */
#line 1209 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 5096 "parser.cpp"
    break;

  case 168: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1218 "parser.y"
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
#line 5113 "parser.cpp"
    break;

  case 169: /* update_expr_array: update_expr  */
#line 1231 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 5122 "parser.cpp"
    break;

  case 170: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1235 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 5131 "parser.cpp"
    break;

  case 171: /* update_expr: IDENTIFIER '=' expr  */
#line 1240 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 5143 "parser.cpp"
    break;

  case 172: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1253 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 5159 "parser.cpp"
    break;

  case 173: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1266 "parser.y"
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
#line 5177 "parser.cpp"
    break;

  case 174: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1281 "parser.y"
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
#line 5195 "parser.cpp"
    break;

  case 175: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1296 "parser.y"
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
#line 5213 "parser.cpp"
    break;

  case 176: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1311 "parser.y"
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
#line 5236 "parser.cpp"
    break;

  case 177: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1334 "parser.y"
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
#line 5294 "parser.cpp"
    break;

  case 178: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1387 "parser.y"
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
#line 5354 "parser.cpp"
    break;

  case 179: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1442 "parser.y"
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
#line 5406 "parser.cpp"
    break;

  case 180: /* select_statement: select_without_paren  */
#line 1493 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5414 "parser.cpp"
    break;

  case 181: /* select_statement: select_with_paren  */
#line 1496 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5422 "parser.cpp"
    break;

  case 182: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1499 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5436 "parser.cpp"
    break;

  case 183: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1508 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5450 "parser.cpp"
    break;

  case 184: /* select_with_paren: '(' select_without_paren ')'  */
#line 1518 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5458 "parser.cpp"
    break;

  case 185: /* select_with_paren: '(' select_with_paren ')'  */
#line 1521 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5466 "parser.cpp"
    break;

  case 186: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1525 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5475 "parser.cpp"
    break;

  case 187: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1530 "parser.y"
                                                                                                   {
    if((yyvsp[-1].expr_t) == nullptr and (yyvsp[0].expr_t) != nullptr) {
        delete (yyvsp[-3].select_stmt);
        delete (yyvsp[-2].order_by_expr_list_t);
        delete (yyvsp[0].expr_t);
        yyerror(&yyloc, scanner, result, "Offset expression isn't valid without Limit expression");
        YYERROR;
    }
    if((yyvsp[-3].select_stmt)->search_expr_ != nullptr and ((yyvsp[-2].order_by_expr_list_t) != nullptr /*or $3 != nullptr or $4 != nullptr*/)) {
        delete (yyvsp[-3].select_stmt);
        if ((yyvsp[-2].order_by_expr_list_t)) {
            for (auto ptr : *((yyvsp[-2].order_by_expr_list_t))) {
                delete ptr;
            }
            delete (yyvsp[-2].order_by_expr_list_t);
        }
        delete (yyvsp[-1].expr_t);
        delete (yyvsp[0].expr_t);
        yyerror(&yyloc, scanner, result, "Result modifier(ORDER BY) is conflict with SEARCH expression.");
        YYERROR;
    }
    (yyvsp[-3].select_stmt)->order_by_list_ = (yyvsp[-2].order_by_expr_list_t);
    (yyvsp[-3].select_stmt)->limit_expr_ = (yyvsp[-1].expr_t);
    (yyvsp[-3].select_stmt)->offset_expr_ = (yyvsp[0].expr_t);
    (yyval.select_stmt) = (yyvsp[-3].select_stmt);
}
#line 5506 "parser.cpp"
    break;

  case 188: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1557 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5514 "parser.cpp"
    break;

  case 189: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1560 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5522 "parser.cpp"
    break;

  case 190: /* select_clause_without_modifier: SELECT distinct expr_array highlight_clause from_clause search_clause where_clause group_by_clause having_clause  */
#line 1565 "parser.y"
                                                                                                                 {
    (yyval.select_stmt) = new infinity::SelectStatement();
    (yyval.select_stmt)->select_distinct_ = (yyvsp[-7].bool_value);
    (yyval.select_stmt)->select_list_ = (yyvsp[-6].expr_array_t);
    (yyval.select_stmt)->highlight_list_ = (yyvsp[-5].expr_array_t);
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
#line 5543 "parser.cpp"
    break;

  case 191: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1582 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5551 "parser.cpp"
    break;

  case 192: /* order_by_clause: %empty  */
#line 1585 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5559 "parser.cpp"
    break;

  case 193: /* order_by_expr_list: order_by_expr  */
#line 1589 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5568 "parser.cpp"
    break;

  case 194: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1593 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5577 "parser.cpp"
    break;

  case 195: /* order_by_expr: expr order_by_type  */
#line 1598 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5587 "parser.cpp"
    break;

  case 196: /* order_by_type: ASC  */
#line 1604 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5595 "parser.cpp"
    break;

  case 197: /* order_by_type: DESC  */
#line 1607 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5603 "parser.cpp"
    break;

  case 198: /* order_by_type: %empty  */
#line 1610 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5611 "parser.cpp"
    break;

  case 199: /* limit_expr: LIMIT expr  */
#line 1614 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5619 "parser.cpp"
    break;

  case 200: /* limit_expr: %empty  */
#line 1618 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5625 "parser.cpp"
    break;

  case 201: /* offset_expr: OFFSET expr  */
#line 1620 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5633 "parser.cpp"
    break;

  case 202: /* offset_expr: %empty  */
#line 1624 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5639 "parser.cpp"
    break;

  case 203: /* distinct: DISTINCT  */
#line 1626 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5647 "parser.cpp"
    break;

  case 204: /* distinct: %empty  */
#line 1629 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5655 "parser.cpp"
    break;

  case 205: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1633 "parser.y"
                                       {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5663 "parser.cpp"
    break;

  case 206: /* highlight_clause: %empty  */
#line 1636 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5671 "parser.cpp"
    break;

  case 207: /* from_clause: FROM table_reference  */
#line 1640 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5679 "parser.cpp"
    break;

  case 208: /* from_clause: %empty  */
#line 1643 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5687 "parser.cpp"
    break;

  case 209: /* search_clause: SEARCH sub_search_array  */
#line 1647 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5697 "parser.cpp"
    break;

  case 210: /* search_clause: %empty  */
#line 1652 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5705 "parser.cpp"
    break;

  case 211: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1656 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5713 "parser.cpp"
    break;

  case 212: /* optional_search_filter_expr: %empty  */
#line 1659 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5721 "parser.cpp"
    break;

  case 213: /* where_clause: WHERE expr  */
#line 1663 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5729 "parser.cpp"
    break;

  case 214: /* where_clause: %empty  */
#line 1666 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5737 "parser.cpp"
    break;

  case 215: /* having_clause: HAVING expr  */
#line 1670 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5745 "parser.cpp"
    break;

  case 216: /* having_clause: %empty  */
#line 1673 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5753 "parser.cpp"
    break;

  case 217: /* group_by_clause: GROUP BY expr_array  */
#line 1677 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5761 "parser.cpp"
    break;

  case 218: /* group_by_clause: %empty  */
#line 1680 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5769 "parser.cpp"
    break;

  case 219: /* set_operator: UNION  */
#line 1684 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5777 "parser.cpp"
    break;

  case 220: /* set_operator: UNION ALL  */
#line 1687 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5785 "parser.cpp"
    break;

  case 221: /* set_operator: INTERSECT  */
#line 1690 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5793 "parser.cpp"
    break;

  case 222: /* set_operator: EXCEPT  */
#line 1693 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5801 "parser.cpp"
    break;

  case 223: /* table_reference: table_reference_unit  */
#line 1701 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5809 "parser.cpp"
    break;

  case 224: /* table_reference: table_reference ',' table_reference_unit  */
#line 1704 "parser.y"
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
#line 5827 "parser.cpp"
    break;

  case 227: /* table_reference_name: table_name table_alias  */
#line 1721 "parser.y"
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
#line 5845 "parser.cpp"
    break;

  case 228: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1735 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5856 "parser.cpp"
    break;

  case 229: /* table_name: IDENTIFIER  */
#line 1744 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5866 "parser.cpp"
    break;

  case 230: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1749 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5878 "parser.cpp"
    break;

  case 231: /* table_alias: AS IDENTIFIER  */
#line 1758 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5888 "parser.cpp"
    break;

  case 232: /* table_alias: IDENTIFIER  */
#line 1763 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5898 "parser.cpp"
    break;

  case 233: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1768 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5909 "parser.cpp"
    break;

  case 234: /* table_alias: %empty  */
#line 1774 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5917 "parser.cpp"
    break;

  case 235: /* with_clause: WITH with_expr_list  */
#line 1781 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5925 "parser.cpp"
    break;

  case 236: /* with_clause: %empty  */
#line 1784 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5933 "parser.cpp"
    break;

  case 237: /* with_expr_list: with_expr  */
#line 1788 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5942 "parser.cpp"
    break;

  case 238: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1791 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5951 "parser.cpp"
    break;

  case 239: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1796 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5963 "parser.cpp"
    break;

  case 240: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1808 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5975 "parser.cpp"
    break;

  case 241: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1815 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5988 "parser.cpp"
    break;

  case 242: /* join_type: INNER  */
#line 1829 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5996 "parser.cpp"
    break;

  case 243: /* join_type: LEFT  */
#line 1832 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 6004 "parser.cpp"
    break;

  case 244: /* join_type: RIGHT  */
#line 1835 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 6012 "parser.cpp"
    break;

  case 245: /* join_type: OUTER  */
#line 1838 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6020 "parser.cpp"
    break;

  case 246: /* join_type: FULL  */
#line 1841 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6028 "parser.cpp"
    break;

  case 247: /* join_type: CROSS  */
#line 1844 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 6036 "parser.cpp"
    break;

  case 248: /* join_type: %empty  */
#line 1847 "parser.y"
                {
}
#line 6043 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW DATABASES  */
#line 1853 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 6052 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW TABLES  */
#line 1857 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 6061 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW TASKS  */
#line 1861 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTasks;
}
#line 6070 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW CONFIGS  */
#line 1865 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 6079 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1869 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6091 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW PROFILES  */
#line 1876 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 6100 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW BUFFER  */
#line 1880 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 6109 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW MEMINDEX  */
#line 1884 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 6118 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW QUERIES  */
#line 1888 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 6127 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1892 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 6137 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW TRANSACTIONS  */
#line 1897 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 6146 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1901 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6156 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW TRANSACTION HISTORY  */
#line 1906 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactionHistory;
}
#line 6165 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW SESSION VARIABLES  */
#line 1910 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 6174 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1914 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 6183 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1918 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6194 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1924 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6205 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1930 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6217 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW TABLE table_name  */
#line 1937 "parser.y"
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
#line 6233 "parser.cpp"
    break;

  case 268: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1948 "parser.y"
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
#line 6249 "parser.cpp"
    break;

  case 269: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1959 "parser.y"
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
#line 6265 "parser.cpp"
    break;

  case 270: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1970 "parser.y"
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
#line 6282 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1982 "parser.y"
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
#line 6299 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1994 "parser.y"
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
#line 6317 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 2007 "parser.y"
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
#line 6336 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 2021 "parser.y"
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
#line 6352 "parser.cpp"
    break;

  case 275: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 2032 "parser.y"
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

    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->index_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6372 "parser.cpp"
    break;

  case 276: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 2047 "parser.y"
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

    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.show_stmt)->index_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.show_stmt)->segment_id_ = (yyvsp[0].long_value);
}
#line 6394 "parser.cpp"
    break;

  case 277: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 2064 "parser.y"
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

      ParserHelper::ToLower((yyvsp[-4].str_value));
      (yyval.show_stmt)->index_name_ = (yyvsp[-4].str_value);
      free((yyvsp[-4].str_value));

      (yyval.show_stmt)->segment_id_ = (yyvsp[-2].long_value);
      (yyval.show_stmt)->chunk_id_ = (yyvsp[0].long_value);
}
#line 6417 "parser.cpp"
    break;

  case 278: /* show_statement: SHOW LOGS  */
#line 2082 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6426 "parser.cpp"
    break;

  case 279: /* show_statement: SHOW CATALOG  */
#line 2086 "parser.y"
               {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalog;
}
#line 6435 "parser.cpp"
    break;

  case 280: /* show_statement: SHOW CATALOG STRING  */
#line 2090 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCatalogKey;
      ParserHelper::ToLower((yyvsp[0].str_value));
      (yyval.show_stmt)->var_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6447 "parser.cpp"
    break;

  case 281: /* show_statement: SHOW CATALOG TO file_path  */
#line 2097 "parser.y"
                            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogToFile;
      (yyval.show_stmt)->file_path_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6458 "parser.cpp"
    break;

  case 282: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2103 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6467 "parser.cpp"
    break;

  case 283: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2107 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6476 "parser.cpp"
    break;

  case 284: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2111 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6487 "parser.cpp"
    break;

  case 285: /* show_statement: SHOW MEMORY  */
#line 2117 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6496 "parser.cpp"
    break;

  case 286: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2121 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6505 "parser.cpp"
    break;

  case 287: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2125 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6514 "parser.cpp"
    break;

  case 288: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2129 "parser.y"
                          {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
      ParserHelper::ToLower((yyvsp[-2].str_value));
      (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
      free((yyvsp[-2].str_value));
}
#line 6526 "parser.cpp"
    break;

  case 289: /* show_statement: SHOW SNAPSHOTS  */
#line 2136 "parser.y"
                 {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListSnapshots;
}
#line 6535 "parser.cpp"
    break;

  case 290: /* show_statement: SHOW SNAPSHOT IDENTIFIER  */
#line 2140 "parser.y"
                           {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowSnapshot;
     ParserHelper::ToLower((yyvsp[0].str_value));
     (yyval.show_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6547 "parser.cpp"
    break;

  case 291: /* show_statement: SHOW CACHES  */
#line 2147 "parser.y"
              {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCaches;
}
#line 6556 "parser.cpp"
    break;

  case 292: /* show_statement: SHOW CACHE  */
#line 2151 "parser.y"
             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowCache;
}
#line 6565 "parser.cpp"
    break;

  case 293: /* show_statement: SHOW COMPACT  */
#line 2155 "parser.y"
               {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCompact;
}
#line 6574 "parser.cpp"
    break;

  case 294: /* show_statement: SHOW COMPACT NOT NULLABLE  */
#line 2159 "parser.y"
                            {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCompact;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6584 "parser.cpp"
    break;

  case 295: /* show_statement: SHOW CHECKPOINT  */
#line 2164 "parser.y"
                  {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCheckpoint;
}
#line 6593 "parser.cpp"
    break;

  case 296: /* show_statement: SHOW CHECKPOINT NOT NULLABLE  */
#line 2168 "parser.y"
                               {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCheckpoint;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6603 "parser.cpp"
    break;

  case 297: /* show_statement: SHOW CHECKPOINT LONG_VALUE  */
#line 2173 "parser.y"
                             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowCheckpoint;
     (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6613 "parser.cpp"
    break;

  case 298: /* show_statement: SHOW OPTIMIZE  */
#line 2178 "parser.y"
                {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListOptimize;
}
#line 6622 "parser.cpp"
    break;

  case 299: /* show_statement: SHOW OPTIMIZE NOT NULLABLE  */
#line 2182 "parser.y"
                             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListOptimize;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6632 "parser.cpp"
    break;

  case 300: /* show_statement: SHOW IMPORT  */
#line 2187 "parser.y"
              {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListImport;
}
#line 6641 "parser.cpp"
    break;

  case 301: /* show_statement: SHOW CLEAN  */
#line 2191 "parser.y"
             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListClean;
}
#line 6650 "parser.cpp"
    break;

  case 302: /* show_statement: SHOW CLEAN NOT NULLABLE  */
#line 2195 "parser.y"
                          {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListClean;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6660 "parser.cpp"
    break;

  case 303: /* show_statement: SHOW CLEAN LONG_VALUE  */
#line 2200 "parser.y"
                        {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowClean;
     (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6670 "parser.cpp"
    break;

  case 304: /* flush_statement: FLUSH DATA  */
#line 2209 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6679 "parser.cpp"
    break;

  case 305: /* flush_statement: FLUSH LOG  */
#line 2213 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6688 "parser.cpp"
    break;

  case 306: /* flush_statement: FLUSH BUFFER  */
#line 2217 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6697 "parser.cpp"
    break;

  case 307: /* flush_statement: FLUSH CATALOG  */
#line 2221 "parser.y"
                {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kCatalog;
}
#line 6706 "parser.cpp"
    break;

  case 308: /* optimize_statement: OPTIMIZE table_name  */
#line 2229 "parser.y"
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
#line 6721 "parser.cpp"
    break;

  case 309: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 2240 "parser.y"
                                                         {
    (yyval.optimize_stmt) = new infinity::OptimizeStatement();
    if((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.optimize_stmt)->schema_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
        free((yyvsp[-1].table_name_t)->schema_name_ptr_);
    }
    (yyval.optimize_stmt)->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
    free((yyvsp[-1].table_name_t)->table_name_ptr_);
    delete (yyvsp[-1].table_name_t);

    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.optimize_stmt)->index_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));

    for (auto *&index_param : *(yyvsp[0].with_index_param_list_t)) {
        (yyval.optimize_stmt)->opt_params_.emplace_back(std::unique_ptr<infinity::InitParameter>(index_param));
        index_param = nullptr;
    }
    delete (yyvsp[0].with_index_param_list_t);
}
#line 6746 "parser.cpp"
    break;

  case 310: /* command_statement: USE IDENTIFIER  */
#line 2264 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6757 "parser.cpp"
    break;

  case 311: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2270 "parser.y"
                                       {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6767 "parser.cpp"
    break;

  case 312: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2275 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6778 "parser.cpp"
    break;

  case 313: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2281 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6789 "parser.cpp"
    break;

  case 314: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2287 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6802 "parser.cpp"
    break;

  case 315: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2295 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6813 "parser.cpp"
    break;

  case 316: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2301 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6824 "parser.cpp"
    break;

  case 317: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2307 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6835 "parser.cpp"
    break;

  case 318: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2313 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6846 "parser.cpp"
    break;

  case 319: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2319 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6859 "parser.cpp"
    break;

  case 320: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2327 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6870 "parser.cpp"
    break;

  case 321: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2333 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6881 "parser.cpp"
    break;

  case 322: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2339 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6892 "parser.cpp"
    break;

  case 323: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2345 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6903 "parser.cpp"
    break;

  case 324: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2351 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6916 "parser.cpp"
    break;

  case 325: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2359 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6927 "parser.cpp"
    break;

  case 326: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2365 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6938 "parser.cpp"
    break;

  case 327: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON TABLE IDENTIFIER  */
#line 2371 "parser.y"
                                                 {
    ParserHelper::ToLower((yyvsp[-3].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kTable, (yyvsp[0].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[0].str_value));
}
#line 6951 "parser.cpp"
    break;

  case 328: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON DATABASE IDENTIFIER  */
#line 2379 "parser.y"
                                                    {
    ParserHelper::ToLower((yyvsp[-3].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kDatabase, (yyvsp[0].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[0].str_value));
}
#line 6964 "parser.cpp"
    break;

  case 329: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON SYSTEM  */
#line 2387 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-2].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kSystem);
    free((yyvsp[-2].str_value));
}
#line 6975 "parser.cpp"
    break;

  case 330: /* command_statement: DROP SNAPSHOT IDENTIFIER  */
#line 2393 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kDrop, infinity::SnapshotScope::kIgnore);
    free((yyvsp[0].str_value));
}
#line 6986 "parser.cpp"
    break;

  case 331: /* command_statement: RESTORE DATABASE SNAPSHOT IDENTIFIER  */
#line 2399 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kDatabase);
    free((yyvsp[0].str_value));
}
#line 6997 "parser.cpp"
    break;

  case 332: /* command_statement: RESTORE TABLE SNAPSHOT IDENTIFIER  */
#line 2405 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kTable);
    free((yyvsp[0].str_value));
}
#line 7008 "parser.cpp"
    break;

  case 333: /* command_statement: CLEAN DATA  */
#line 2411 "parser.y"
             {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::CleanupCmd>();
}
#line 7017 "parser.cpp"
    break;

  case 334: /* command_statement: DUMP INDEX IDENTIFIER ON table_name  */
#line 2415 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::DumpIndexCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_, (yyvsp[-2].str_value));
    free((yyvsp[-2].str_value));
    if ((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 7033 "parser.cpp"
    break;

  case 335: /* compact_statement: COMPACT TABLE table_name  */
#line 2427 "parser.y"
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
#line 7050 "parser.cpp"
    break;

  case 336: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2440 "parser.y"
                                                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 7061 "parser.cpp"
    break;

  case 337: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2446 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 7073 "parser.cpp"
    break;

  case 338: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2453 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 7086 "parser.cpp"
    break;

  case 339: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2461 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 7100 "parser.cpp"
    break;

  case 340: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2470 "parser.y"
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
#line 7115 "parser.cpp"
    break;

  case 341: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2480 "parser.y"
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
#line 7130 "parser.cpp"
    break;

  case 342: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2490 "parser.y"
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
#line 7146 "parser.cpp"
    break;

  case 343: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2501 "parser.y"
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
#line 7162 "parser.cpp"
    break;

  case 344: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2512 "parser.y"
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
#line 7179 "parser.cpp"
    break;

  case 345: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2524 "parser.y"
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
#line 7196 "parser.cpp"
    break;

  case 346: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2536 "parser.y"
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
#line 7211 "parser.cpp"
    break;

  case 347: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2546 "parser.y"
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
#line 7227 "parser.cpp"
    break;

  case 348: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2557 "parser.y"
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
#line 7244 "parser.cpp"
    break;

  case 349: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2569 "parser.y"
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
#line 7262 "parser.cpp"
    break;

  case 350: /* admin_statement: ADMIN SHOW LOGS  */
#line 2582 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 7271 "parser.cpp"
    break;

  case 351: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2586 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 7281 "parser.cpp"
    break;

  case 352: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2591 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 7291 "parser.cpp"
    break;

  case 353: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2596 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 7302 "parser.cpp"
    break;

  case 354: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2602 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 7311 "parser.cpp"
    break;

  case 355: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2606 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 7320 "parser.cpp"
    break;

  case 356: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2610 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     ParserHelper::ToLower((yyvsp[0].str_value));
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7332 "parser.cpp"
    break;

  case 357: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2617 "parser.y"
                        {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
}
#line 7341 "parser.cpp"
    break;

  case 358: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2621 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
}
#line 7350 "parser.cpp"
    break;

  case 359: /* admin_statement: ADMIN SHOW SNAPSHOT IDENTIFIER  */
#line 2625 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSnapshot;
     ParserHelper::ToLower((yyvsp[0].str_value));
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7362 "parser.cpp"
    break;

  case 360: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2632 "parser.y"
                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7373 "parser.cpp"
    break;

  case 361: /* admin_statement: ADMIN EXPORT SNAPSHOT STRING TO STRING  */
#line 2638 "parser.y"
                                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kExportSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[-2].str_value);
     (yyval.admin_stmt)->export_path_ = (yyvsp[0].str_value);
     free((yyvsp[-2].str_value));
     free((yyvsp[0].str_value));
}
#line 7386 "parser.cpp"
    break;

  case 362: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2646 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7397 "parser.cpp"
    break;

  case 363: /* admin_statement: ADMIN SHOW NODES  */
#line 2652 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 7406 "parser.cpp"
    break;

  case 364: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2656 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7417 "parser.cpp"
    break;

  case 365: /* admin_statement: ADMIN SHOW NODE  */
#line 2662 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 7426 "parser.cpp"
    break;

  case 366: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2666 "parser.y"
                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7437 "parser.cpp"
    break;

  case 367: /* admin_statement: ADMIN SET ADMIN  */
#line 2672 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
}
#line 7447 "parser.cpp"
    break;

  case 368: /* admin_statement: ADMIN SET STANDALONE  */
#line 2677 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
}
#line 7457 "parser.cpp"
    break;

  case 369: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2682 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7469 "parser.cpp"
    break;

  case 370: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2689 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7483 "parser.cpp"
    break;

  case 371: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2698 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7497 "parser.cpp"
    break;

  case 372: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2708 "parser.y"
                                                              {
    auto *ret = new infinity::RenameTableStatement((yyvsp[-3].table_name_t)->schema_name_ptr_, (yyvsp[-3].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;
    ParserHelper::ToLower((yyvsp[0].str_value));
    ret->new_table_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);
}
#line 7512 "parser.cpp"
    break;

  case 373: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2718 "parser.y"
                                                             {
    auto *ret = new infinity::AddColumnsStatement((yyvsp[-5].table_name_t)->schema_name_ptr_, (yyvsp[-5].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;

    for (infinity::ColumnDef*& column_def : *(yyvsp[-1].column_def_array_t)) {
        ret->column_defs_.emplace_back(column_def);
    }
    delete (yyvsp[-1].column_def_array_t);
    free((yyvsp[-5].table_name_t)->schema_name_ptr_);
    free((yyvsp[-5].table_name_t)->table_name_ptr_);
    delete (yyvsp[-5].table_name_t);
}
#line 7529 "parser.cpp"
    break;

  case 374: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2730 "parser.y"
                                                              {
    auto *ret = new infinity::DropColumnsStatement((yyvsp[-5].table_name_t)->schema_name_ptr_, (yyvsp[-5].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;
    for (std::string &column_name : *(yyvsp[-1].identifier_array_t)) {
        ret->column_names_.emplace_back(std::move(column_name));
    }
    delete (yyvsp[-1].identifier_array_t);
    free((yyvsp[-5].table_name_t)->schema_name_ptr_);
    free((yyvsp[-5].table_name_t)->table_name_ptr_);
    delete (yyvsp[-5].table_name_t);
}
#line 7545 "parser.cpp"
    break;

  case 375: /* check_statement: CHECK SYSTEM  */
#line 2742 "parser.y"
                               {
    (yyval.check_stmt) = new infinity::CheckStatement();
    (yyval.check_stmt)->check_type_ = infinity::CheckStmtType::kSystem;
}
#line 7554 "parser.cpp"
    break;

  case 376: /* check_statement: CHECK TABLE table_name  */
#line 2746 "parser.y"
                         {
    (yyval.check_stmt) = new infinity::CheckStatement();
    (yyval.check_stmt)->check_type_ = infinity::CheckStmtType::kTable;
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.check_stmt)->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    (yyval.check_stmt)->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 7570 "parser.cpp"
    break;

  case 377: /* expr_array: expr_alias  */
#line 2762 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7579 "parser.cpp"
    break;

  case 378: /* expr_array: expr_array ',' expr_alias  */
#line 2766 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7588 "parser.cpp"
    break;

  case 379: /* insert_row_list: '(' expr_array ')'  */
#line 2771 "parser.y"
                                     {
    auto res = std::make_unique<infinity::InsertRowExpr>();
    for (auto* &expr : *(yyvsp[-1].expr_array_t)) {
        res->values_.emplace_back(expr);
        expr = nullptr;
    }
    delete (yyvsp[-1].expr_array_t);
    (yyval.insert_row_list_t) = new std::vector<infinity::InsertRowExpr*>();
    (yyval.insert_row_list_t)->emplace_back(res.release());
}
#line 7603 "parser.cpp"
    break;

  case 380: /* insert_row_list: insert_row_list ',' '(' expr_array ')'  */
#line 2781 "parser.y"
                                         {
    (yyval.insert_row_list_t) = (yyvsp[-4].insert_row_list_t);
    auto res = std::make_unique<infinity::InsertRowExpr>();
    for (auto* &expr : *(yyvsp[-1].expr_array_t)) {
        res->values_.emplace_back(expr);
        expr = nullptr;
    }
    delete (yyvsp[-1].expr_array_t);
    (yyval.insert_row_list_t)->emplace_back(res.release());
}
#line 7618 "parser.cpp"
    break;

  case 381: /* expr_alias: expr AS IDENTIFIER  */
#line 2803 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7629 "parser.cpp"
    break;

  case 382: /* expr_alias: expr  */
#line 2809 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7637 "parser.cpp"
    break;

  case 388: /* operand: '(' expr ')'  */
#line 2819 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 7645 "parser.cpp"
    break;

  case 389: /* operand: '(' select_without_paren ')'  */
#line 2822 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7656 "parser.cpp"
    break;

  case 390: /* operand: constant_expr  */
#line 2828 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7664 "parser.cpp"
    break;

  case 401: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2844 "parser.y"
                                                                                                                                        {
    auto match_tensor_expr = std::make_unique<infinity::MatchTensorExpr>();
    // search column
    match_tensor_expr->SetSearchColumn((yyvsp[-10].expr_t));
    // search tensor
    ParserHelper::ToLower((yyvsp[-6].str_value));
    match_tensor_expr->SetQueryTensor((yyvsp[-6].str_value), (yyvsp[-8].const_expr_t));
    // search method
    ParserHelper::ToLower((yyvsp[-4].str_value));
    match_tensor_expr->SetSearchMethod((yyvsp[-4].str_value));
    // search options
    match_tensor_expr->SetExtraOptions((yyvsp[-2].str_value));
    match_tensor_expr->SetOptionalFilter((yyvsp[-1].expr_t));
    (yyval.expr_t) = match_tensor_expr.release();
}
#line 7684 "parser.cpp"
    break;

  case 402: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')'  */
#line 2860 "parser.y"
                                                                                                                                                   {
    auto match_tensor_expr = std::make_unique<infinity::MatchTensorExpr>();
    // search column
    match_tensor_expr->SetSearchColumn((yyvsp[-15].expr_t));
    // search tensor
    ParserHelper::ToLower((yyvsp[-11].str_value));
    match_tensor_expr->SetQueryTensor((yyvsp[-11].str_value), (yyvsp[-13].const_expr_t));
    // search method
    ParserHelper::ToLower((yyvsp[-9].str_value));
    match_tensor_expr->SetSearchMethod((yyvsp[-9].str_value));
    // search options
    match_tensor_expr->SetExtraOptions((yyvsp[-7].str_value));
    match_tensor_expr->SetOptionalFilter((yyvsp[-6].expr_t));
    ParserHelper::ToLower((yyvsp[-1].str_value));
    match_tensor_expr->index_name_ = (yyvsp[-1].str_value);
    (yyval.expr_t) = match_tensor_expr.release();
}
#line 7706 "parser.cpp"
    break;

  case 403: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' IGNORE INDEX  */
#line 2878 "parser.y"
                                                                                                                                 {
    auto match_tensor_expr = std::make_unique<infinity::MatchTensorExpr>();
    // search column
    match_tensor_expr->SetSearchColumn((yyvsp[-12].expr_t));
    // search tensor
    ParserHelper::ToLower((yyvsp[-8].str_value));
    match_tensor_expr->SetQueryTensor((yyvsp[-8].str_value), (yyvsp[-10].const_expr_t));
    // search method
    ParserHelper::ToLower((yyvsp[-6].str_value));
    match_tensor_expr->SetSearchMethod((yyvsp[-6].str_value));
    // search options
    match_tensor_expr->ignore_index_ = true;
    match_tensor_expr->SetExtraOptions((yyvsp[-4].str_value));
    match_tensor_expr->SetOptionalFilter((yyvsp[-3].expr_t));
    (yyval.expr_t) = match_tensor_expr.release();
}
#line 7727 "parser.cpp"
    break;

  case 404: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2896 "parser.y"
                                                                                                                                                                             {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search column
    match_vector_expr->column_expr_ = (yyvsp[-16].expr_t);

    // vector distance type
    ParserHelper::ToLower((yyvsp[-10].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-10].str_value));
    if (!check) {
        goto Error1;
    }

    // vector data type and query embedding expression
    ParserHelper::ToLower((yyvsp[-12].str_value));
    // For backward compatibility, try to handle as array_expr first, then as general expr
    if (auto* const_expr = dynamic_cast<infinity::ConstantExpr*>((yyvsp[-14].expr_t))) {
        check = match_vector_expr->InitEmbedding((yyvsp[-12].str_value), const_expr);
        if (!check) {
            goto Error1;
        }
        delete (yyvsp[-14].expr_t);
    } else {
        // Store the expression for later evaluation during execution
        match_vector_expr->query_embedding_expr_.reset((yyvsp[-14].expr_t));
        match_vector_expr->embedding_data_type_str_ = (yyvsp[-12].str_value);
    }
    free((yyvsp[-12].str_value));
    free((yyvsp[-10].str_value));

    ParserHelper::ToLower((yyvsp[-2].str_value));
    match_vector_expr->index_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    match_vector_expr->topn_ = (yyvsp[-8].long_value);
    match_vector_expr->filter_expr_.reset((yyvsp[-7].expr_t));
    match_vector_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
    goto Return1;
Error1:
    for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
        delete param_ptr;
    }
    delete (yyvsp[0].with_index_param_list_t);
    free((yyvsp[-12].str_value));
    free((yyvsp[-10].str_value));
    free((yyvsp[-2].str_value));
    if (auto* const_expr = dynamic_cast<infinity::ConstantExpr*>((yyvsp[-14].expr_t))) {
        delete (yyvsp[-14].expr_t);
    } else {
        delete (yyvsp[-14].expr_t);
    }
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return1:
    ;
}
#line 7789 "parser.cpp"
    break;

  case 405: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2954 "parser.y"
                                                                                                                 {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search column
    match_vector_expr->column_expr_ = (yyvsp[-12].expr_t);

    // vector distance type
    ParserHelper::ToLower((yyvsp[-6].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-6].str_value));
    if (!check) {
        goto Error2;
    }

    // vector data type and query embedding expression
    ParserHelper::ToLower((yyvsp[-8].str_value));
    // For backward compatibility, try to handle as array_expr first, then as general expr
    if (auto* const_expr = dynamic_cast<infinity::ConstantExpr*>((yyvsp[-10].expr_t))) {
        check = match_vector_expr->InitEmbedding((yyvsp[-8].str_value), const_expr);
        if (!check) {
            goto Error2;
        }
        delete (yyvsp[-10].expr_t);
    } else {
        // Store the expression for later evaluation during execution
        match_vector_expr->query_embedding_expr_.reset((yyvsp[-10].expr_t));
        match_vector_expr->embedding_data_type_str_ = (yyvsp[-8].str_value);
    }
    free((yyvsp[-8].str_value));
    free((yyvsp[-6].str_value));

    match_vector_expr->topn_ = (yyvsp[-4].long_value);
    match_vector_expr->filter_expr_.reset((yyvsp[-3].expr_t));
    match_vector_expr->ignore_index_ = true;
    goto Return2;
Error2:
    free((yyvsp[-8].str_value));
    free((yyvsp[-6].str_value));
    if (auto* const_expr = dynamic_cast<infinity::ConstantExpr*>((yyvsp[-10].expr_t))) {
        delete (yyvsp[-10].expr_t);
    } else {
        delete (yyvsp[-10].expr_t);
    }
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return2:
    ;
}
#line 7843 "parser.cpp"
    break;

  case 406: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 3004 "parser.y"
                                                                                                                          {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search column
    match_vector_expr->column_expr_ = (yyvsp[-11].expr_t);

    // vector distance type
    ParserHelper::ToLower((yyvsp[-5].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-5].str_value));
    if (!check) {
        goto Error3;
    }

    // vector data type and query embedding expression
    ParserHelper::ToLower((yyvsp[-7].str_value));
    // For backward compatibility, try to handle as array_expr first, then as general expr
    if (auto* const_expr = dynamic_cast<infinity::ConstantExpr*>((yyvsp[-9].expr_t))) {
        check = match_vector_expr->InitEmbedding((yyvsp[-7].str_value), const_expr);
        if (!check) {
            goto Error3;
        }
        delete (yyvsp[-9].expr_t);
    } else {
        // Store the expression for later evaluation during execution
        match_vector_expr->query_embedding_expr_.reset((yyvsp[-9].expr_t));
        match_vector_expr->embedding_data_type_str_ = (yyvsp[-7].str_value);
    }
    free((yyvsp[-7].str_value));
    free((yyvsp[-5].str_value));

    match_vector_expr->topn_ = (yyvsp[-3].long_value);
    match_vector_expr->filter_expr_.reset((yyvsp[-2].expr_t));
    match_vector_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
    goto Return3;
Error3:
    for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
        delete param_ptr;
    }
    delete (yyvsp[0].with_index_param_list_t);
    free((yyvsp[-7].str_value));
    free((yyvsp[-5].str_value));
    if (auto* const_expr = dynamic_cast<infinity::ConstantExpr*>((yyvsp[-9].expr_t))) {
        delete (yyvsp[-9].expr_t);
    } else {
        delete (yyvsp[-9].expr_t);
    }
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return3:
    ;
}
#line 7901 "parser.cpp"
    break;

  case 407: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3058 "parser.y"
                                                                                                           {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search search column
    match_vector_expr->column_expr_ = (yyvsp[-9].expr_t);

    // vector search distance type
    ParserHelper::ToLower((yyvsp[-3].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-3].str_value));
    if (!check) {
        goto Error4;
    }

    // vector search data type and query embedding expression
    ParserHelper::ToLower((yyvsp[-5].str_value));
    // For backward compatibility, try to handle as array_expr first, then as general expr
    if (auto* const_expr = dynamic_cast<infinity::ConstantExpr*>((yyvsp[-7].expr_t))) {
        check = match_vector_expr->InitEmbedding((yyvsp[-5].str_value), const_expr);
        if (!check) {
            goto Error4;
        }
        delete (yyvsp[-7].expr_t);
    } else {
        // Store the expression for later evaluation during execution
        match_vector_expr->query_embedding_expr_.reset((yyvsp[-7].expr_t));
        match_vector_expr->embedding_data_type_str_ = (yyvsp[-5].str_value);
    }
    free((yyvsp[-5].str_value));
    free((yyvsp[-3].str_value));

    match_vector_expr->topn_ = infinity::DEFAULT_MATCH_VECTOR_TOP_N;
    match_vector_expr->filter_expr_.reset((yyvsp[-2].expr_t));
    match_vector_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
    goto Return4;

Error4:
    for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
        delete param_ptr;
    }
    delete (yyvsp[0].with_index_param_list_t);
    free((yyvsp[-5].str_value));
    free((yyvsp[-3].str_value));
    if (auto* const_expr = dynamic_cast<infinity::ConstantExpr*>((yyvsp[-7].expr_t))) {
        delete (yyvsp[-7].expr_t);
    } else {
        delete (yyvsp[-7].expr_t);
    }
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return4:
    ;
}
#line 7960 "parser.cpp"
    break;

  case 408: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 3116 "parser.y"
                                                                                                                                                                                     {
    auto match_sparse_expr = new infinity::MatchSparseExpr();
    (yyval.expr_t) = match_sparse_expr;

    // search column
    match_sparse_expr->SetSearchColumn((yyvsp[-14].expr_t));

    // search sparse and data type
    match_sparse_expr->SetQuerySparse((yyvsp[-12].const_expr_t));

    // metric type
    ParserHelper::ToLower((yyvsp[-10].str_value));
    match_sparse_expr->SetMetricType((yyvsp[-10].str_value));

    // topn and options
    match_sparse_expr->SetOptParams((yyvsp[-8].long_value), (yyvsp[0].with_index_param_list_t));

    // optional filter
    match_sparse_expr->SetOptionalFilter((yyvsp[-7].expr_t));

    ParserHelper::ToLower((yyvsp[-2].str_value));
    match_sparse_expr->index_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
}
#line 7989 "parser.cpp"
    break;

  case 409: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 3141 "parser.y"
                                                                                                                          {
    auto match_sparse_expr = new infinity::MatchSparseExpr();
    (yyval.expr_t) = match_sparse_expr;

    // search column
    match_sparse_expr->SetSearchColumn((yyvsp[-10].expr_t));

    // search sparse and data type
    match_sparse_expr->SetQuerySparse((yyvsp[-8].const_expr_t));

    // metric type
    ParserHelper::ToLower((yyvsp[-6].str_value));
    match_sparse_expr->SetMetricType((yyvsp[-6].str_value));

    // topn and options
    match_sparse_expr->topn_ = (yyvsp[-4].long_value);

    // optional filter
    match_sparse_expr->SetOptionalFilter((yyvsp[-3].expr_t));

    match_sparse_expr->ignore_index_ = true;
}
#line 8016 "parser.cpp"
    break;

  case 410: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 3164 "parser.y"
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

    // optional filter
    match_sparse_expr->SetOptionalFilter((yyvsp[-2].expr_t));

    // topn and options
    match_sparse_expr->SetOptParams((yyvsp[-3].long_value), (yyvsp[0].with_index_param_list_t));
}
#line 8041 "parser.cpp"
    break;

  case 411: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3185 "parser.y"
                                                                                                                    {
    auto match_sparse_expr = new infinity::MatchSparseExpr();
    (yyval.expr_t) = match_sparse_expr;

    // search column
    match_sparse_expr->SetSearchColumn((yyvsp[-7].expr_t));

    // search sparse and data type
    match_sparse_expr->SetQuerySparse((yyvsp[-5].const_expr_t));

    // metric type
    ParserHelper::ToLower((yyvsp[-3].str_value));
    match_sparse_expr->SetMetricType((yyvsp[-3].str_value));

    // optional filter
    match_sparse_expr->SetOptionalFilter((yyvsp[-2].expr_t));

    // topn and options
    match_sparse_expr->SetOptParams(infinity::DEFAULT_MATCH_SPARSE_TOP_N, (yyvsp[0].with_index_param_list_t));
}
#line 8066 "parser.cpp"
    break;

  case 412: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3206 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8080 "parser.cpp"
    break;

  case 413: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3215 "parser.y"
                                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-6].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-6].str_value));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8096 "parser.cpp"
    break;

  case 414: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3226 "parser.y"
                                                                                                {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-9].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-7].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-6].expr_t));
    match_text_expr->index_names_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-9].str_value));
    free((yyvsp[-7].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8112 "parser.cpp"
    break;

  case 415: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3237 "parser.y"
                                                                                                           {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-11].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-9].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-7].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-6].expr_t));
    match_text_expr->index_names_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-11].str_value));
    free((yyvsp[-9].str_value));
    free((yyvsp[-7].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8130 "parser.cpp"
    break;

  case 416: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 3251 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8142 "parser.cpp"
    break;

  case 417: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3258 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8156 "parser.cpp"
    break;

  case 418: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3268 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 8167 "parser.cpp"
    break;

  case 419: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 3274 "parser.y"
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
#line 8183 "parser.cpp"
    break;

  case 420: /* sub_search: match_vector_expr  */
#line 3286 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8191 "parser.cpp"
    break;

  case 421: /* sub_search: match_text_expr  */
#line 3289 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8199 "parser.cpp"
    break;

  case 422: /* sub_search: match_tensor_expr  */
#line 3292 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8207 "parser.cpp"
    break;

  case 423: /* sub_search: match_sparse_expr  */
#line 3295 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8215 "parser.cpp"
    break;

  case 424: /* sub_search: query_expr  */
#line 3298 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8223 "parser.cpp"
    break;

  case 425: /* sub_search: fusion_expr  */
#line 3301 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8231 "parser.cpp"
    break;

  case 426: /* sub_search_array: sub_search  */
#line 3305 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 8240 "parser.cpp"
    break;

  case 427: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3309 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 8249 "parser.cpp"
    break;

  case 428: /* function_expr: IDENTIFIER '(' ')'  */
#line 3314 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 8262 "parser.cpp"
    break;

  case 429: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3322 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 8275 "parser.cpp"
    break;

  case 430: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 3330 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 8289 "parser.cpp"
    break;

  case 431: /* function_expr: YEAR '(' expr ')'  */
#line 3339 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "year";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8301 "parser.cpp"
    break;

  case 432: /* function_expr: MONTH '(' expr ')'  */
#line 3346 "parser.y"
                     {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "month";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8313 "parser.cpp"
    break;

  case 433: /* function_expr: DAY '(' expr ')'  */
#line 3353 "parser.y"
                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "day";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8325 "parser.cpp"
    break;

  case 434: /* function_expr: HOUR '(' expr ')'  */
#line 3360 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "hour";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8337 "parser.cpp"
    break;

  case 435: /* function_expr: MINUTE '(' expr ')'  */
#line 3367 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "minute";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8349 "parser.cpp"
    break;

  case 436: /* function_expr: SECOND '(' expr ')'  */
#line 3374 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "second";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8361 "parser.cpp"
    break;

  case 437: /* function_expr: operand IS NOT NULLABLE  */
#line 3381 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8373 "parser.cpp"
    break;

  case 438: /* function_expr: operand IS NULLABLE  */
#line 3388 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8385 "parser.cpp"
    break;

  case 439: /* function_expr: NOT operand  */
#line 3395 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8397 "parser.cpp"
    break;

  case 440: /* function_expr: '-' operand  */
#line 3402 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8409 "parser.cpp"
    break;

  case 441: /* function_expr: '+' operand  */
#line 3409 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8421 "parser.cpp"
    break;

  case 442: /* function_expr: operand '-' operand  */
#line 3416 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8434 "parser.cpp"
    break;

  case 443: /* function_expr: operand '+' operand  */
#line 3424 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8447 "parser.cpp"
    break;

  case 444: /* function_expr: operand '*' operand  */
#line 3432 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8460 "parser.cpp"
    break;

  case 445: /* function_expr: operand '/' operand  */
#line 3440 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8473 "parser.cpp"
    break;

  case 446: /* function_expr: operand '%' operand  */
#line 3448 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8486 "parser.cpp"
    break;

  case 447: /* function_expr: operand '=' operand  */
#line 3456 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8499 "parser.cpp"
    break;

  case 448: /* function_expr: operand EQUAL operand  */
#line 3464 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8512 "parser.cpp"
    break;

  case 449: /* function_expr: operand NOT_EQ operand  */
#line 3472 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8525 "parser.cpp"
    break;

  case 450: /* function_expr: operand '<' operand  */
#line 3480 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8538 "parser.cpp"
    break;

  case 451: /* function_expr: operand '>' operand  */
#line 3488 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8551 "parser.cpp"
    break;

  case 452: /* function_expr: operand LESS_EQ operand  */
#line 3496 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8564 "parser.cpp"
    break;

  case 453: /* function_expr: operand GREATER_EQ operand  */
#line 3504 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8577 "parser.cpp"
    break;

  case 454: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3512 "parser.y"
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
#line 8612 "parser.cpp"
    break;

  case 455: /* function_expr: operand LIKE operand  */
#line 3542 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8625 "parser.cpp"
    break;

  case 456: /* function_expr: operand NOT LIKE operand  */
#line 3550 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8638 "parser.cpp"
    break;

  case 457: /* conjunction_expr: expr AND expr  */
#line 3559 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8651 "parser.cpp"
    break;

  case 458: /* conjunction_expr: expr OR expr  */
#line 3567 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8664 "parser.cpp"
    break;

  case 459: /* between_expr: operand BETWEEN operand AND operand  */
#line 3576 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 8676 "parser.cpp"
    break;

  case 460: /* in_expr: operand IN '(' expr_array ')'  */
#line 3584 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8687 "parser.cpp"
    break;

  case 461: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3590 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8698 "parser.cpp"
    break;

  case 462: /* case_expr: CASE expr case_check_array END  */
#line 3597 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8709 "parser.cpp"
    break;

  case 463: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3603 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8721 "parser.cpp"
    break;

  case 464: /* case_expr: CASE case_check_array END  */
#line 3610 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8731 "parser.cpp"
    break;

  case 465: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3615 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8742 "parser.cpp"
    break;

  case 466: /* case_check_array: WHEN expr THEN expr  */
#line 3622 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 8754 "parser.cpp"
    break;

  case 467: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3629 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 8766 "parser.cpp"
    break;

  case 468: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3637 "parser.y"
                                            {
    auto [data_type_result, fail_reason] = infinity::ColumnType::GetDataTypeFromColumnType(*((yyvsp[-1].column_type_t)), std::vector<std::unique_ptr<infinity::InitParameter>>{});
    delete (yyvsp[-1].column_type_t);
    if (!data_type_result) {
        yyerror(&yyloc, scanner, result, fail_reason.c_str());
        delete (yyvsp[-3].expr_t);
        YYERROR;
    }
    infinity::CastExpr* cast_expr = new infinity::CastExpr(std::move(*data_type_result));
    cast_expr->expr_ = (yyvsp[-3].expr_t);
    (yyval.expr_t) = cast_expr;
}
#line 8783 "parser.cpp"
    break;

  case 469: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3650 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8794 "parser.cpp"
    break;

  case 470: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3656 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8805 "parser.cpp"
    break;

  case 471: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3662 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8817 "parser.cpp"
    break;

  case 472: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3669 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8829 "parser.cpp"
    break;

  case 473: /* column_expr: IDENTIFIER  */
#line 3677 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8841 "parser.cpp"
    break;

  case 474: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3684 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8853 "parser.cpp"
    break;

  case 475: /* column_expr: '*'  */
#line 3691 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8863 "parser.cpp"
    break;

  case 476: /* column_expr: column_expr '.' '*'  */
#line 3696 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8877 "parser.cpp"
    break;

  case 477: /* constant_expr: STRING  */
#line 3706 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8887 "parser.cpp"
    break;

  case 478: /* constant_expr: TRUE  */
#line 3711 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8897 "parser.cpp"
    break;

  case 479: /* constant_expr: FALSE  */
#line 3716 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8907 "parser.cpp"
    break;

  case 480: /* constant_expr: DOUBLE_VALUE  */
#line 3721 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8917 "parser.cpp"
    break;

  case 481: /* constant_expr: LONG_VALUE  */
#line 3726 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8927 "parser.cpp"
    break;

  case 482: /* constant_expr: DATE STRING  */
#line 3731 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8937 "parser.cpp"
    break;

  case 483: /* constant_expr: TIME STRING  */
#line 3736 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8947 "parser.cpp"
    break;

  case 484: /* constant_expr: DATETIME STRING  */
#line 3741 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8957 "parser.cpp"
    break;

  case 485: /* constant_expr: TIMESTAMP STRING  */
#line 3746 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8967 "parser.cpp"
    break;

  case 486: /* constant_expr: INTERVAL interval_expr  */
#line 3751 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8975 "parser.cpp"
    break;

  case 487: /* constant_expr: interval_expr  */
#line 3754 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8983 "parser.cpp"
    break;

  case 488: /* constant_expr: common_array_expr  */
#line 3757 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8991 "parser.cpp"
    break;

  case 489: /* constant_expr: curly_brackets_expr  */
#line 3760 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8999 "parser.cpp"
    break;

  case 490: /* common_array_expr: array_expr  */
#line 3764 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9007 "parser.cpp"
    break;

  case 491: /* common_array_expr: subarray_array_expr  */
#line 3767 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9015 "parser.cpp"
    break;

  case 492: /* common_array_expr: sparse_array_expr  */
#line 3770 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9023 "parser.cpp"
    break;

  case 493: /* common_array_expr: empty_array_expr  */
#line 3773 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9031 "parser.cpp"
    break;

  case 494: /* common_sparse_array_expr: sparse_array_expr  */
#line 3777 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9039 "parser.cpp"
    break;

  case 495: /* common_sparse_array_expr: array_expr  */
#line 3780 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9047 "parser.cpp"
    break;

  case 496: /* common_sparse_array_expr: empty_array_expr  */
#line 3783 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9055 "parser.cpp"
    break;

  case 497: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3787 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9063 "parser.cpp"
    break;

  case 498: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3791 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 9073 "parser.cpp"
    break;

  case 499: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3796 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9082 "parser.cpp"
    break;

  case 500: /* sparse_array_expr: long_sparse_array_expr  */
#line 3801 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9090 "parser.cpp"
    break;

  case 501: /* sparse_array_expr: double_sparse_array_expr  */
#line 3804 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9098 "parser.cpp"
    break;

  case 502: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3808 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9106 "parser.cpp"
    break;

  case 503: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3812 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 9118 "parser.cpp"
    break;

  case 504: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3819 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9129 "parser.cpp"
    break;

  case 505: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3826 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9137 "parser.cpp"
    break;

  case 506: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3830 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 9149 "parser.cpp"
    break;

  case 507: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3837 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9160 "parser.cpp"
    break;

  case 508: /* empty_array_expr: '[' ']'  */
#line 3844 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 9168 "parser.cpp"
    break;

  case 509: /* curly_brackets_expr: unclosed_curly_brackets_expr '}'  */
#line 3848 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9176 "parser.cpp"
    break;

  case 510: /* curly_brackets_expr: '{' '}'  */
#line 3851 "parser.y"
          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
}
#line 9184 "parser.cpp"
    break;

  case 511: /* unclosed_curly_brackets_expr: '{' constant_expr  */
#line 3855 "parser.y"
                                                {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
    (yyval.const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
}
#line 9193 "parser.cpp"
    break;

  case 512: /* unclosed_curly_brackets_expr: unclosed_curly_brackets_expr ',' constant_expr  */
#line 3859 "parser.y"
                                                 {
    (yyvsp[-2].const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9202 "parser.cpp"
    break;

  case 513: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3864 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 9210 "parser.cpp"
    break;

  case 514: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3868 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 9218 "parser.cpp"
    break;

  case 515: /* array_expr: long_array_expr  */
#line 3872 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9226 "parser.cpp"
    break;

  case 516: /* array_expr: double_array_expr  */
#line 3875 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9234 "parser.cpp"
    break;

  case 517: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3879 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9242 "parser.cpp"
    break;

  case 518: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3883 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9252 "parser.cpp"
    break;

  case 519: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3888 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9261 "parser.cpp"
    break;

  case 520: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3893 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9269 "parser.cpp"
    break;

  case 521: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3897 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9279 "parser.cpp"
    break;

  case 522: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3902 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9288 "parser.cpp"
    break;

  case 523: /* interval_expr: LONG_VALUE SECONDS  */
#line 3907 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9299 "parser.cpp"
    break;

  case 524: /* interval_expr: LONG_VALUE SECOND  */
#line 3913 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9310 "parser.cpp"
    break;

  case 525: /* interval_expr: LONG_VALUE MINUTES  */
#line 3919 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9321 "parser.cpp"
    break;

  case 526: /* interval_expr: LONG_VALUE MINUTE  */
#line 3925 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9332 "parser.cpp"
    break;

  case 527: /* interval_expr: LONG_VALUE HOURS  */
#line 3931 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9343 "parser.cpp"
    break;

  case 528: /* interval_expr: LONG_VALUE HOUR  */
#line 3937 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9354 "parser.cpp"
    break;

  case 529: /* interval_expr: LONG_VALUE DAYS  */
#line 3943 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9365 "parser.cpp"
    break;

  case 530: /* interval_expr: LONG_VALUE DAY  */
#line 3949 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9376 "parser.cpp"
    break;

  case 531: /* interval_expr: LONG_VALUE MONTHS  */
#line 3955 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9387 "parser.cpp"
    break;

  case 532: /* interval_expr: LONG_VALUE MONTH  */
#line 3961 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9398 "parser.cpp"
    break;

  case 533: /* interval_expr: LONG_VALUE YEARS  */
#line 3967 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9409 "parser.cpp"
    break;

  case 534: /* interval_expr: LONG_VALUE YEAR  */
#line 3973 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9420 "parser.cpp"
    break;

  case 535: /* copy_option_list: copy_option  */
#line 3984 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 9429 "parser.cpp"
    break;

  case 536: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3988 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 9438 "parser.cpp"
    break;

  case 537: /* copy_option: FORMAT IDENTIFIER  */
#line 3993 "parser.y"
                                {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kFormat;
    ParserHelper::ToLower((yyvsp[0].str_value));
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
#line 9475 "parser.cpp"
    break;

  case 538: /* copy_option: DELIMITER STRING  */
#line 4025 "parser.y"
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
#line 9490 "parser.cpp"
    break;

  case 539: /* copy_option: HEADER  */
#line 4035 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 9500 "parser.cpp"
    break;

  case 540: /* copy_option: OFFSET LONG_VALUE  */
#line 4040 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 9510 "parser.cpp"
    break;

  case 541: /* copy_option: LIMIT LONG_VALUE  */
#line 4045 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 9520 "parser.cpp"
    break;

  case 542: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 4050 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 9530 "parser.cpp"
    break;

  case 543: /* file_path: STRING  */
#line 4056 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 9538 "parser.cpp"
    break;

  case 544: /* if_exists: IF EXISTS  */
#line 4060 "parser.y"
                     { (yyval.bool_value) = true; }
#line 9544 "parser.cpp"
    break;

  case 545: /* if_exists: %empty  */
#line 4061 "parser.y"
  { (yyval.bool_value) = false; }
#line 9550 "parser.cpp"
    break;

  case 546: /* if_not_exists: IF NOT EXISTS  */
#line 4063 "parser.y"
                              { (yyval.bool_value) = true; }
#line 9556 "parser.cpp"
    break;

  case 547: /* if_not_exists: %empty  */
#line 4064 "parser.y"
  { (yyval.bool_value) = false; }
#line 9562 "parser.cpp"
    break;

  case 550: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 4079 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9575 "parser.cpp"
    break;

  case 551: /* if_not_exists_info: %empty  */
#line 4087 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 9583 "parser.cpp"
    break;

  case 552: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 4091 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9591 "parser.cpp"
    break;

  case 553: /* with_index_param_list: %empty  */
#line 4094 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 9599 "parser.cpp"
    break;

  case 554: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 4098 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9607 "parser.cpp"
    break;

  case 555: /* optional_table_properties_list: %empty  */
#line 4101 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 9615 "parser.cpp"
    break;

  case 556: /* index_param_list: index_param  */
#line 4105 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 9624 "parser.cpp"
    break;

  case 557: /* index_param_list: index_param_list ',' index_param  */
#line 4109 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 9633 "parser.cpp"
    break;

  case 558: /* index_param: IDENTIFIER  */
#line 4114 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9644 "parser.cpp"
    break;

  case 559: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 4120 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9659 "parser.cpp"
    break;

  case 560: /* index_param: IDENTIFIER '=' STRING  */
#line 4130 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9674 "parser.cpp"
    break;

  case 561: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 4140 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 9687 "parser.cpp"
    break;

  case 562: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 4148 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 9700 "parser.cpp"
    break;

  case 563: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 4159 "parser.y"
                                                                       {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    infinity::IndexType index_type = infinity::IndexType::kInvalid;
    if(strcmp((yyvsp[-1].str_value), "fulltext") == 0) {
        index_type = infinity::IndexType::kFullText;
    } else if (strcmp((yyvsp[-1].str_value), "hnsw") == 0) {
        index_type = infinity::IndexType::kHnsw;
    } else if (strcmp((yyvsp[-1].str_value), "bmp") == 0) {
        index_type = infinity::IndexType::kBMP;
    } else if (strcmp((yyvsp[-1].str_value), "ivf") == 0) {
        index_type = infinity::IndexType::kIVF;
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
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.index_info_t)->column_name_ = (yyvsp[-4].str_value);
    (yyval.index_info_t)->index_param_list_ = (yyvsp[0].with_index_param_list_t);
    free((yyvsp[-4].str_value));
}
#line 9737 "parser.cpp"
    break;

  case 564: /* index_info: '(' IDENTIFIER ')'  */
#line 4191 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 9749 "parser.cpp"
    break;


#line 9753 "parser.cpp"

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

#line 4199 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

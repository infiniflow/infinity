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
#define YYLAST   1600

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  230
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  120
/* YYNRULES -- Number of rules.  */
#define YYNRULES  563
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1271

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
    2090,  2096,  2100,  2104,  2110,  2114,  2118,  2122,  2129,  2133,
    2140,  2144,  2148,  2152,  2157,  2161,  2166,  2171,  2175,  2180,
    2184,  2188,  2193,  2202,  2206,  2210,  2214,  2222,  2233,  2257,
    2263,  2268,  2274,  2280,  2288,  2294,  2300,  2306,  2312,  2320,
    2326,  2332,  2338,  2344,  2352,  2358,  2364,  2372,  2380,  2386,
    2392,  2398,  2404,  2408,  2420,  2433,  2439,  2446,  2454,  2463,
    2473,  2483,  2494,  2505,  2517,  2529,  2539,  2550,  2562,  2575,
    2579,  2584,  2589,  2595,  2599,  2603,  2610,  2614,  2618,  2625,
    2631,  2639,  2645,  2649,  2655,  2659,  2665,  2670,  2675,  2682,
    2691,  2701,  2711,  2723,  2735,  2739,  2755,  2759,  2764,  2774,
    2796,  2802,  2806,  2807,  2808,  2809,  2810,  2812,  2815,  2821,
    2824,  2825,  2826,  2827,  2828,  2829,  2830,  2831,  2832,  2833,
    2837,  2853,  2871,  2889,  2947,  2997,  3051,  3109,  3134,  3157,
    3178,  3199,  3208,  3219,  3230,  3244,  3251,  3261,  3267,  3279,
    3282,  3285,  3288,  3291,  3294,  3298,  3302,  3307,  3315,  3323,
    3332,  3339,  3346,  3353,  3360,  3367,  3374,  3381,  3388,  3395,
    3402,  3409,  3417,  3425,  3433,  3441,  3449,  3457,  3465,  3473,
    3481,  3489,  3497,  3505,  3535,  3543,  3552,  3560,  3569,  3577,
    3583,  3590,  3596,  3603,  3608,  3615,  3622,  3630,  3643,  3649,
    3655,  3662,  3670,  3677,  3684,  3689,  3699,  3704,  3709,  3714,
    3719,  3724,  3729,  3734,  3739,  3744,  3747,  3750,  3753,  3757,
    3760,  3763,  3766,  3770,  3773,  3776,  3780,  3784,  3789,  3794,
    3797,  3801,  3805,  3812,  3819,  3823,  3830,  3837,  3841,  3844,
    3848,  3852,  3857,  3861,  3865,  3868,  3872,  3876,  3881,  3886,
    3890,  3895,  3900,  3906,  3912,  3918,  3924,  3930,  3936,  3942,
    3948,  3954,  3960,  3966,  3977,  3981,  3986,  4018,  4028,  4033,
    4038,  4043,  4049,  4053,  4054,  4056,  4057,  4059,  4060,  4072,
    4080,  4084,  4087,  4091,  4094,  4098,  4102,  4107,  4113,  4123,
    4133,  4141,  4152,  4184
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

#define YYPACT_NINF (-775)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-551)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     973,    64,    53,   159,    98,    36,    98,   299,   254,  1026,
      86,   106,   122,   255,   170,   182,   226,   127,   131,   385,
      62,   144,   -41,   322,   111,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,  -775,   399,  -775,  -775,   318,  -775,  -775,  -775,
    -775,  -775,  -775,  -775,  -775,   309,   309,   309,   309,   250,
     398,    98,   319,   319,   319,   319,   319,   405,   175,   426,
      98,   -27,   407,   422,   441,   997,  -775,  -775,  -775,  -775,
    -775,  -775,  -775,   399,  -775,  -775,  -775,  -775,  -775,   231,
     443,   470,   514,    98,  -775,  -775,  -775,  -775,  -775,    20,
    -775,   286,   351,  -775,   519,  -775,  -775,   403,   499,  -775,
     541,  -775,  -775,  -775,   328,   161,    27,    74,  -775,    98,
      98,   553,  -775,  -775,  -775,  -775,  -775,   -30,  -775,   516,
     348,  -775,   573,   389,   416,   314,   476,   419,   577,   423,
     518,   430,   449,    98,  -775,  -775,   397,   434,  -775,   914,
    -775,   618,  -775,  -775,     8,   597,  -775,   603,   605,   688,
      98,    98,    98,   694,   638,   640,   481,   632,   708,    98,
      98,    98,   710,  -775,   714,   716,   668,   732,   732,   572,
      59,    87,   101,  -775,   515,   610,   620,  -775,   448,  -775,
    -775,  -775,   749,  -775,   751,  -775,   732,  -775,  -775,   768,
    -775,  -775,  -775,  -775,  -775,   653,  -775,   654,   387,  -775,
     724,    98,   568,   226,   732,  -775,   787,  -775,   628,  -775,
     789,  -775,  -775,   793,  -775,   791,  -775,   794,   796,  -775,
     797,   750,   799,   611,   803,   804,  -775,  -775,  -775,  -775,
    -775,     8,  -775,  -775,  -775,   572,   756,   742,   737,   680,
     -36,  -775,   481,  -775,    98,   417,   811,    41,  -775,  -775,
    -775,  -775,  -775,   754,  -775,   604,    -4,  -775,   572,  -775,
    -775,   740,   743,   596,  -775,  -775,   622,   739,   600,   602,
     401,   815,   822,   823,   824,  -775,  -775,   825,   607,   608,
     612,   614,   616,   621,   625,   358,   626,   627,   753,   753,
    -775,    17,   558,    15,   245,  -775,     2,   998,  -775,  -775,
    -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,
    -775,   609,  -775,  -775,  -775,   212,  -775,  -775,   221,  -775,
     256,  -775,  -775,   400,  -775,  -775,   285,  -775,   288,  -775,
    -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,   829,   836,
    -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,   798,
     800,   774,    98,   775,   318,  -775,  -775,  -775,   847,    52,
    -775,   850,  -775,  -775,   781,   445,  -775,   854,  -775,  -775,
     639,   641,   -39,   572,   572,   795,  -775,   857,   -41,    72,
     812,   643,   864,   865,  -775,  -775,   334,   647,  -775,    98,
     572,   716,  -775,   492,   648,   649,   241,  -775,  -775,  -775,
    -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,   753,
     650,  1096,   801,   572,   572,   -24,   425,  -775,  -775,  -775,
    -775,   622,  -775,   572,   572,   572,   572,   572,   572,   869,
     658,   660,   662,   664,   888,   894,   373,   373,  -775,   665,
    -775,  -775,  -775,  -775,   677,   -74,  -775,  -775,   826,   572,
     900,   572,   572,    -5,   682,    33,   753,   753,   753,   753,
     753,   753,   753,   753,   753,   753,   753,   753,   753,   753,
      24,  -775,   685,  -775,   901,  -775,   902,    34,  -775,  -775,
     903,  -775,   906,   870,   631,   684,   691,   912,  -775,   695,
    -775,   693,  -775,   913,  -775,   269,   929,   770,   773,  -775,
    -775,  -775,   572,   866,   713,  -775,   206,   492,   572,  -775,
    -775,    31,  1167,   817,   725,   346,  -775,  -775,  -775,   -41,
     943,   816,  -775,  -775,  -775,   945,   572,   726,  -775,   492,
    -775,     5,     5,   572,  -775,   379,   801,   792,   728,    12,
     -26,   433,  -775,   572,   572,   -69,   -33,   152,   173,   177,
     179,   878,   572,    32,   572,   953,   738,   380,   741,  -775,
    -775,   732,  -775,  -775,  -775,   805,   736,   753,   558,   833,
    -775,   382,   382,   141,   141,  1087,   382,   382,   141,   141,
     373,   373,  -775,  -775,  -775,  -775,  -775,  -775,   745,  -775,
     747,  -775,  -775,  -775,  -775,   959,   960,  -775,   811,   968,
    -775,   969,  -775,  -775,   971,  -775,  -775,   974,   975,   766,
      14,   819,   572,  -775,  -775,  -775,   492,   990,  -775,  -775,
    -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,   771,
    -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,   776,   778,   783,   790,   802,   806,   808,   809,
     316,   810,   811,   964,    72,   399,   813,  1010,  -775,   381,
     830,  1009,  1011,  1012,  1015,  -775,  1013,   388,  -775,   396,
     402,  -775,   814,  -775,  1167,   572,  -775,   572,   -46,   195,
    -775,  -775,  -775,  -775,  -775,  -775,   753,   -42,  -775,  -150,
     142,   807,    43,   821,  -775,  1018,  -775,  -775,   957,   558,
     382,   828,   411,  -775,   753,  1029,  1033,   996,   993,   415,
     429,  -775,   834,   439,  -775,  1041,  -775,  -775,   -41,   827,
     624,  -775,   134,  -775,   367,   668,  -775,  -775,  1048,  1167,
    1167,   989,  1035,  1227,  1244,  1267,  1284,   925,   932,  -775,
    -775,   240,  -775,   930,   811,   440,   835,   931,  -775,   898,
    -775,  -775,   572,  -775,  -775,  -775,  -775,  -775,  -775,     5,
    -775,  -775,  -775,   842,   492,   -28,  -775,   572,   890,   572,
     685,   848,  1065,   845,   572,  -775,   852,   851,   853,   462,
    -775,  -775,  1096,  1071,  1078,  -775,  -775,   968,   548,  -775,
     969,   252,    30,    14,  1031,  -775,  -775,  -775,  -775,  -775,
    -775,  1032,  -775,  1086,  -775,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,   871,  1039,   466,   868,   498,  -775,   872,   874,
     875,   884,   886,   889,   891,   892,   893,  1004,   897,   904,
     907,   908,   909,   911,   915,   917,   918,   919,  1005,   920,
     921,   927,   928,   935,   936,   937,   938,   939,   941,  1025,
     946,   947,   948,   949,   950,   951,   952,   956,   958,   965,
    1064,   976,   978,   980,   983,  1002,  1003,  1006,  1007,  1017,
    1024,  1066,  1027,  1046,  1053,  1054,  1056,  1057,  1067,  1068,
    1069,  1070,  1080,  1072,  -775,  -775,    34,  -775,   987,  1036,
     500,  -775,   969,  1175,  1180,   502,  -775,  -775,  -775,   492,
    -775,   160,  1081,    19,  1095,  -775,  -775,  -775,  1099,  1120,
     963,   492,  -775,     5,  -775,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,  -775,  -775,  1193,  -775,   134,   624,    14,    14,
    1000,   367,  1148,  1170,  -775,  1225,  -775,  -775,  1167,  1235,
    1238,  1245,  1246,  1286,  1291,  1316,  1320,  1321,  1102,  1323,
    1332,  1333,  1336,  1337,  1338,  1339,  1340,  1344,  1345,  1127,
    1348,  1349,  1355,  1356,  1359,  1360,  1361,  1362,  1372,  1373,
    1156,  1377,  1378,  1379,  1380,  1384,  1385,  1387,  1388,  1389,
    1390,  1171,  1392,  1393,  1394,  1395,  1396,  1397,  1398,  1399,
    1400,  1401,  1182,  1404,  1405,  1406,  1407,  1408,  1409,  1410,
    1411,  1412,  1413,  1194,  1415,  -775,  1418,  1419,  -775,   504,
    -775,   775,  -775,  1420,  1421,  1422,    90,  1204,  -775,   508,
    1423,  -775,  -775,  1367,   811,  -775,   572,   572,  -775,  1207,
    -775,  1208,  1209,  1210,  1211,  1212,  1213,  1214,  1215,  1216,
    1434,  1218,  1219,  1220,  1221,  1222,  1223,  1224,  1226,  1228,
    1229,  1442,  1230,  1231,  1232,  1233,  1234,  1236,  1237,  1239,
    1240,  1241,  1444,  1242,  1243,  1247,  1248,  1249,  1250,  1251,
    1252,  1253,  1254,  1452,  1255,  1256,  1257,  1258,  1259,  1260,
    1261,  1262,  1263,  1264,  1455,  1265,  1266,  1268,  1269,  1270,
    1271,  1272,  1273,  1274,  1275,  1461,  1276,  -775,  -775,  -775,
    -775,  1277,  1278,  1279,   845,  1303,  -775,   450,   572,   509,
     766,   492,  -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,  1283,  -775,  -775,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,  -775,  1285,  -775,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,  -775,  -775,  1287,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,  -775,  -775,  -775,  1288,  -775,  -775,  -775,  -775,
    -775,  -775,  -775,  -775,  -775,  -775,  1289,  -775,  -775,  -775,
    -775,  -775,  -775,  -775,  -775,  -775,  -775,  1290,  -775,  1465,
    1486,   265,  1292,  1293,  1453,  1494,  1496,  -775,  -775,  -775,
     492,  -775,  -775,  -775,  -775,  -775,  -775,  -775,  1281,  1294,
     845,   775,  1352,  1296,  1503,   704,   280,  1298,  1510,  1299,
    -775,  1471,  1519,   734,  1518,  -775,   845,   775,   845,   125,
    1304,  1302,  1521,  -775,  1481,  1306,  -775,  1307,  1492,  1493,
    -775,  1529,  -775,  -775,  -775,   154,    40,  -775,  1312,  1313,
    1498,  1499,  -775,  1500,  1501,  1532,  -775,  -775,  1319,  -775,
    1322,  1324,  1539,  1540,   775,  1325,  1326,  -775,   775,  -775,
    -775
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     236,     0,     0,     0,     0,     0,     0,     0,   236,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   236,     0,   548,     3,     5,    10,    12,    13,
      11,     6,     7,     9,   181,   180,     0,     8,    14,    15,
      16,    17,    18,    19,    20,   546,   546,   546,   546,   546,
       0,     0,   544,   544,   544,   544,   544,     0,   229,     0,
       0,     0,     0,     0,     0,   236,   167,    21,    26,    28,
      27,    22,    23,    25,    24,    29,    30,    31,    32,     0,
     292,   297,     0,     0,   250,   251,   249,   255,   259,     0,
     256,     0,     0,   252,     0,   254,   278,   279,     0,   257,
       0,   288,   290,   291,     0,   284,   300,   294,   299,     0,
       0,     0,   303,   304,   305,   306,   309,   229,   307,     0,
     235,   237,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   374,   332,     0,     0,     1,   236,
       2,   219,   221,   222,     0,   204,   186,   192,     0,     0,
       0,     0,     0,     0,     0,     0,   165,     0,     0,     0,
       0,     0,     0,   329,     0,     0,   214,     0,     0,     0,
       0,     0,     0,   166,     0,     0,     0,   266,   267,   260,
     261,   262,     0,   263,     0,   253,     0,   258,   289,     0,
     282,   281,   285,   286,   302,     0,   296,     0,     0,   334,
       0,     0,     0,     0,     0,   356,     0,   366,     0,   367,
       0,   353,   354,     0,   349,     0,   362,   364,     0,   357,
       0,     0,     0,     0,     0,     0,   375,   185,   184,     4,
     220,     0,   182,   183,   203,     0,     0,   200,     0,    34,
       0,    35,   165,   549,     0,     0,     0,   236,   543,   172,
     174,   173,   175,     0,   230,     0,   214,   169,     0,   161,
     542,     0,     0,   472,   476,   479,   480,     0,     0,     0,
       0,     0,     0,     0,     0,   477,   478,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     474,     0,   236,     0,     0,   376,   381,   382,   396,   394,
     397,   395,   398,   399,   391,   386,   385,   384,   392,   393,
     383,   390,   389,   487,   490,     0,   491,   499,     0,   500,
       0,   492,   488,     0,   489,   514,     0,   515,     0,   486,
     313,   315,   314,   311,   312,   318,   320,   319,   316,   317,
     323,   325,   324,   321,   322,   287,   293,   298,     0,     0,
     269,   268,   274,   264,   265,   280,   283,   301,   295,     0,
       0,     0,     0,   552,     0,   238,   310,   359,     0,   350,
     355,     0,   363,   358,     0,     0,   365,     0,   330,   331,
       0,     0,   206,     0,     0,   202,   545,     0,   236,     0,
       0,     0,     0,     0,   328,   159,     0,     0,   163,     0,
       0,     0,   168,   213,     0,     0,     0,   523,   522,   525,
     524,   527,   526,   529,   528,   531,   530,   533,   532,     0,
       0,   438,   236,     0,     0,     0,     0,   481,   482,   483,
     484,     0,   485,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   440,   439,   520,   517,
     507,   497,   502,   505,     0,     0,   509,   510,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   496,     0,   501,     0,   504,     0,     0,   508,   516,
       0,   519,     0,   275,   270,     0,     0,     0,   333,     0,
     308,     0,   368,     0,   351,     0,     0,     0,     0,   361,
     189,   188,     0,   208,   191,   193,   198,   199,     0,   187,
      33,    37,     0,     0,     0,     0,    43,    47,    48,   236,
       0,    41,   327,   326,   164,     0,     0,   162,   176,   171,
     170,     0,     0,     0,   427,     0,   236,     0,     0,     0,
       0,     0,   463,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   212,     0,     0,   388,
     387,     0,   377,   380,   456,   457,     0,     0,   236,     0,
     437,   447,   448,   451,   452,     0,   454,   446,   449,   450,
     442,   441,   443,   444,   445,   473,   475,   498,     0,   503,
       0,   506,   511,   518,   521,     0,     0,   271,     0,     0,
     371,     0,   239,   352,     0,   335,   360,     0,     0,   205,
       0,   210,     0,   196,   197,   195,   201,     0,    55,    58,
      59,    56,    57,    60,    61,    77,    62,    64,    63,    80,
      67,    68,    69,    65,    66,    70,    71,    72,    73,    74,
      75,    76,     0,     0,     0,     0,     0,     0,     0,     0,
     552,     0,     0,   554,     0,    40,     0,     0,   160,     0,
       0,     0,     0,     0,     0,   538,     0,     0,   534,     0,
       0,   428,     0,   468,     0,     0,   461,     0,     0,     0,
     435,   434,   433,   432,   431,   430,     0,     0,   472,     0,
       0,     0,     0,     0,   417,     0,   513,   512,     0,   236,
     455,     0,     0,   436,     0,     0,     0,   276,   272,     0,
       0,    45,   557,     0,   555,   336,   369,   370,   236,   207,
     223,   225,   234,   226,     0,   214,   194,    39,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   152,
     153,   156,   149,   156,     0,     0,     0,    36,    44,   563,
      42,   378,     0,   540,   539,   537,   536,   541,   179,     0,
     177,   429,   469,     0,   465,     0,   464,     0,     0,     0,
       0,     0,     0,   212,     0,   415,     0,     0,     0,     0,
     470,   459,   458,     0,     0,   373,   372,     0,     0,   551,
       0,     0,     0,     0,     0,   243,   244,   245,   246,   242,
     247,     0,   232,     0,   227,   421,   419,   422,   420,   423,
     424,   425,   209,   218,     0,     0,     0,    53,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   154,   151,     0,   150,    50,    49,
       0,   158,     0,     0,     0,     0,   535,   467,   462,   466,
     453,     0,     0,     0,     0,   493,   495,   494,   212,     0,
       0,   211,   418,     0,   471,   460,   277,   273,    46,   558,
     559,   561,   560,   556,     0,   337,   234,   224,     0,     0,
     231,     0,     0,   216,    79,     0,   147,   148,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   155,     0,     0,   157,     0,
      38,   552,   379,     0,     0,     0,     0,     0,   416,     0,
     338,   228,   240,     0,     0,   426,     0,     0,   190,     0,
      54,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    52,    51,   553,
     562,     0,     0,   212,   212,   411,   178,     0,     0,     0,
     217,   215,    78,    84,    85,    82,    83,    86,    87,    88,
      89,    90,     0,    81,   128,   129,   126,   127,   130,   131,
     132,   133,   134,     0,   125,    95,    96,    93,    94,    97,
      98,    99,   100,   101,     0,    92,   106,   107,   104,   105,
     108,   109,   110,   111,   112,     0,   103,   139,   140,   137,
     138,   141,   142,   143,   144,   145,     0,   136,   117,   118,
     115,   116,   119,   120,   121,   122,   123,     0,   114,     0,
       0,     0,     0,     0,     0,     0,     0,   340,   339,   345,
     241,   233,    91,   135,   102,   113,   146,   124,   212,     0,
     212,   552,   412,     0,   346,   341,     0,     0,     0,     0,
     410,     0,     0,     0,     0,   342,   212,   552,   212,   552,
       0,     0,     0,   347,   343,     0,   406,     0,     0,     0,
     409,     0,   413,   348,   344,   552,   400,   408,     0,     0,
       0,     0,   405,     0,     0,     0,   414,   404,     0,   402,
       0,     0,     0,     0,   552,     0,     0,   407,   552,   401,
     403
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -775,  -775,  -775,  1414,  1480,   281,  -775,  -775,   882,  -541,
    -775,  -669,  -775,   818,   820,  -775,  -595,   283,   284,  1308,
    -775,   329,  -775,  1150,   330,   344,    -6,  1530,   -19,  1190,
    1327,   -90,  -775,  -775,   933,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,  -774,  -249,  -775,  -775,  -775,  -775,   757,  -157,
      73,   623,  -775,  -775,  1353,  -775,  -775,   363,   365,   366,
     376,   383,  -775,  -775,  -775,  -234,  -775,  1098,  -258,  -259,
    -720,  -702,  -693,  -691,  -690,  -689,   629,  -775,  -775,  -775,
    -775,  -775,  -775,  1136,  -775,  -775,   999,  -287,  -286,  -775,
    -775,  -775,   782,  -775,  -775,  -775,  -775,   784,  -775,  -775,
    1082,  1085,   786,  -775,  -775,  -775,  -775,  1295,  -531,   831,
    -158,   512,   539,  -775,  -775,  -656,  -775,   666,   764,  -775
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    23,    24,    25,    66,    26,   525,   720,   526,   527,
     826,   660,   751,   752,   898,   528,   396,    27,    28,   247,
      29,    30,   256,   257,    31,    32,    33,    34,    35,   146,
     232,   147,   237,   514,   515,   625,   385,   519,   235,   513,
     621,   735,   703,   259,  1038,   943,   144,   729,   730,   731,
     732,   814,    36,   120,   121,   733,   811,    37,    38,    39,
      40,    41,    42,    43,    44,   294,   537,   295,   296,   297,
     298,   299,   300,   301,   302,   303,   821,   822,   304,   305,
     306,   307,   308,   426,   309,   310,   311,   312,   313,   914,
     314,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     452,   453,   324,   325,   326,   327,   328,   329,   677,   678,
     261,   158,   149,   140,   154,   500,   757,   723,   724,   531
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     403,   382,    73,   137,   753,   451,   457,   402,   421,   920,
     262,   679,   425,   719,   815,   773,   145,    58,   388,   264,
     265,   266,   448,   449,   448,   449,   179,   595,   355,   446,
     447,   201,   816,   194,   455,   698,    16,   776,   264,   265,
     266,   817,   512,   818,   819,   820,   366,   783,   167,   168,
     141,   141,   142,   142,   233,   908,   460,   143,   143,    73,
     685,   258,   330,   424,   331,   332,   684,   755,   721,   576,
     825,   827,   671,   672,   480,   522,   780,    59,   397,    61,
     196,   461,   462,   673,   674,   675,   461,   462,   118,    51,
     335,   503,   336,   337,  1114,   133,    45,    46,    47,   195,
     504,    58,    48,    49,   340,   579,   341,   342,   784,   461,
     462,    60,   134,   461,   462,   271,   272,   273,    16,   109,
     333,   274,   461,   462,   156,   516,   517,   461,   462,   461,
     462,   461,   462,   166,   271,   272,   273,   812,   123,   110,
     274,   381,   539,   124,  1027,   125,   197,   126,   338,   570,
     275,   276,   277,   577,   690,   784,   178,   461,   462,   900,
     421,   111,   343,   580,   627,   549,   550,   461,   462,   275,
     276,   277,   545,   116,  1253,   555,   556,   557,   558,   559,
     560,    22,   198,   199,   779,   117,   389,   459,   813,   676,
     691,    52,    53,    54,   164,   169,   597,    55,    56,   523,
     602,   524,   499,   574,   575,  1254,   226,   581,   582,   583,
     584,   585,   586,   587,   588,   589,   590,   591,   592,   593,
     594,   815,   401,   240,   241,   242,   180,   334,   398,   119,
     231,   499,   250,   251,   252,   291,   728,   291,   450,   816,
     450,   596,   456,   293,   263,   264,   265,   266,   817,   290,
     818,   819,   820,   936,   291,   339,   928,    65,    50,  1238,
     626,     1,   293,     2,     3,     4,     5,     6,     7,   344,
       9,  1210,   465,   454,   363,   623,   624,    11,   619,  1040,
      12,   777,    13,    14,    15,   934,  1226,   935,  1250,    67,
    1239,    68,    69,  -551,  -551,   688,   689,   461,   462,   122,
     127,   614,   669,   135,   697,   543,   700,   461,   462,   680,
     615,  -550,   747,   267,   268,   461,   462,   391,   710,  1251,
     128,   148,   138,   269,   129,   270,   145,   130,   461,   462,
     784,    16,   461,   462,   461,   462,   139,    70,    71,  1192,
    1193,   271,   272,   273,   712,   784,    67,   274,    68,    69,
     461,   462,    72,    57,  -551,  -551,   475,   476,   477,   478,
     479,   461,   462,   192,   516,  1110,   193,   748,   781,   749,
     750,    74,   896,    75,    76,   692,   275,   276,   277,   278,
     148,   279,   521,   280,    77,   281,  1023,   282,   747,   283,
     157,    78,  1029,   499,    70,    71,   693,   359,   284,   164,
     694,   155,   695,   548,   263,   264,   265,   266,   163,    72,
     170,   360,   361,   708,   112,   113,   114,   131,   132,   141,
     285,   142,   286,    17,   287,   171,   143,   774,    74,   775,
      75,    76,   115,   481,  1217,   498,  1219,   778,   482,  1119,
     165,    77,   483,   748,   172,   749,   750,   484,    78,   392,
     393,    19,  1235,   174,  1237,   792,   288,   289,   290,   181,
     182,   291,    21,   292,   544,    62,    63,   394,   458,   293,
      64,   459,   538,   267,   268,   789,    22,   485,   440,   186,
     441,   442,   486,   269,   443,   270,   823,   348,   424,  1195,
     349,   350,  1196,  1197,   912,   351,   352,  1198,  1199,   207,
     208,   271,   272,   273,   209,   187,   489,   274,   552,   491,
     553,   490,   554,   465,   492,   175,   686,   177,   687,   909,
     554,   911,   185,   665,   183,   184,   921,   682,   905,   189,
     190,   191,  -551,  -551,   468,   469,   275,   276,   277,   278,
    -551,   279,   176,   280,   188,   281,   285,   282,   286,   283,
     287,   929,   930,   931,   932,  1220,   200,   534,   284,   711,
     535,   263,   264,   265,   266,   159,   160,   161,   162,   663,
     202,  1236,   664,  1240,   203,   263,   264,   265,   266,   204,
     285,   221,   286,   205,   287,   150,   151,   152,   153,  1252,
     477,   478,   479,   223,  -551,   473,   474,   475,   476,   477,
     478,   479,   681,   704,   761,   459,   705,   459,  1267,  1015,
     206,   768,  1270,   220,   769,   222,   288,   289,   290,   770,
     227,   291,   769,   292,   224,   771,   487,   488,   459,   293,
     267,   268,   507,   508,   791,    16,   210,   459,   795,   230,
     269,   535,   270,   225,   267,   268,   211,   461,   462,   212,
     213,   214,   796,   215,   269,   797,   270,   228,   271,   272,
     273,   234,   799,   901,   274,   800,   535,   216,   217,   236,
     218,   219,   271,   272,   273,   606,   607,   238,   274,   804,
    -248,   805,   806,   807,   808,   925,   809,   810,   459,   944,
     788,   239,   945,   275,   276,   277,   278,   243,   279,   244,
     280,   245,   281,   246,   282,   248,   283,   275,   276,   277,
     278,   249,   279,   253,   280,   284,   281,   254,   282,   255,
     283,   947,   802,  1018,   948,  1022,   535,  1109,   459,   284,
     800,  1116,  1201,   258,   769,   535,   260,   285,   345,   286,
     346,   287,   263,   264,   265,   266,   706,   707,  1224,  1225,
     347,   285,   353,   286,   354,   287,   263,   264,   265,   266,
     407,   408,   409,   410,   411,   412,   413,   414,   415,   416,
     417,   418,   356,   288,   289,   290,  1232,  1233,   291,  1121,
     292,  1032,  1033,   357,   358,   362,   293,   288,   289,   290,
     364,   367,   291,   368,   292,   369,   370,   371,   372,   373,
     293,   374,  1120,   376,   375,   377,   378,   379,   383,   384,
     386,   419,   420,   387,   395,   399,   400,   404,   406,   427,
     405,   269,   422,   270,   423,   419,   428,   429,   430,   433,
     434,   431,   493,   480,   435,   269,   436,   270,   437,   271,
     272,   273,   494,   438,   495,   274,   496,   439,   444,   445,
     497,   502,   499,   271,   272,   273,   505,   506,   509,   274,
    1200,   520,   510,   518,   511,   530,   529,   532,   533,   536,
     541,   542,   546,   561,   275,   276,   277,   278,    16,   279,
     562,   280,   563,   281,   564,   282,   565,   283,   275,   276,
     277,   278,   566,   279,   568,   280,   284,   281,   567,   282,
     569,   283,   571,   573,   578,   291,   608,   598,   600,   603,
     284,   604,   605,   609,  -547,   610,   612,   611,   285,   613,
     286,     1,   287,     2,     3,     4,     5,     6,     7,     8,
       9,    10,   285,   616,   286,   617,   287,    11,   618,   622,
      12,   620,    13,    14,    15,   661,   666,   662,   668,   667,
     577,   683,   670,   696,   288,   289,   290,   701,   709,   291,
     461,   292,   547,   713,   702,   717,   718,   293,   288,   289,
     290,   522,   722,   291,   715,   292,   716,   725,   726,   727,
       1,   293,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    16,   459,   738,   737,   756,    11,   734,   739,    12,
     740,    13,    14,    15,     1,   741,     2,     3,     4,     5,
       6,     7,   742,     9,   760,   763,   765,   764,   766,   767,
      11,   465,   786,    12,   743,    13,    14,    15,   744,    79,
     745,   746,   754,   782,   787,   707,   759,   772,   706,   794,
     466,   467,   468,   469,   785,   793,   798,   801,   471,    80,
      16,   790,   762,   803,   824,   894,    81,   902,    82,    83,
     895,    84,   896,   904,   903,   907,    85,    86,   913,   918,
     463,   919,   464,   923,    16,   922,   924,   926,   828,   829,
     830,   831,   832,    17,   927,   833,   834,   938,   939,   940,
     942,   946,   835,   836,   837,   958,   969,   941,   949,    18,
     950,   951,   472,   473,   474,   475,   476,   477,   478,   479,
     952,    19,   953,   910,   838,   954,   980,   955,   956,   957,
    1016,    20,    21,   959,   839,   840,   841,   842,   843,   465,
     960,   844,   845,   961,   962,   963,    22,   964,   846,   847,
     848,   965,    17,   966,   967,   968,   970,   971,   466,   467,
     468,   469,   470,   972,   973,   991,   471,  1002,    18,   547,
     849,   974,   975,   976,   977,   978,    17,   979,   547,  1017,
      19,  1013,   981,   982,   983,   984,   985,   986,   987,  1020,
      20,    21,   988,  1021,   989,   784,  1028,    87,    88,    89,
      90,   990,    91,    92,    19,    22,    93,    94,    95,  1030,
    1036,    96,   992,    97,   993,    21,   994,    98,    99,   995,
     472,   473,   474,   475,   476,   477,   478,   479,   465,    22,
     100,   101,  1034,  1037,   102,   103,   104,   465,   996,   997,
     105,  1039,   998,   999,   106,   107,   108,   466,   467,   468,
     469,  1041,   714,  1000,  1042,   471,   466,   467,   468,   469,
    1001,  1043,  1044,  1003,   471,   628,   629,   630,   631,   632,
     633,   634,   635,   636,   637,   638,   639,   640,   641,   642,
     643,   644,  1004,   645,   646,   647,   648,   649,   650,  1005,
    1006,   651,  1007,  1008,   652,   653,   654,   655,   656,   657,
     658,   659,  1045,  1009,  1010,  1011,  1012,  1046,  1014,   472,
     473,   474,   475,   476,   477,   478,   479,  1024,   472,   473,
     474,   475,   476,   477,   478,   479,   850,   851,   852,   853,
     854,  1025,  1047,   855,   856,  1026,  1048,  1049,  1050,  1051,
     857,   858,   859,   861,   862,   863,   864,   865,  1052,  1053,
     866,   867,  1054,  1055,  1056,  1057,  1058,   868,   869,   870,
    1059,  1060,   860,  1061,  1062,  1063,   872,   873,   874,   875,
     876,  1064,  1065,   877,   878,  1066,  1067,  1068,  1069,   871,
     879,   880,   881,   883,   884,   885,   886,   887,  1070,  1071,
     888,   889,  1072,  1073,  1074,  1075,  1076,   890,   891,   892,
    1077,  1078,   882,  1079,  1080,  1081,  1082,  1083,  1084,  1085,
    1086,  1087,  1088,  1089,  1090,  1091,  1092,  1093,  1094,   893,
    1095,  1096,  1097,  1098,  1099,  1100,  1101,  1102,  1103,  1104,
    1105,  1106,  1107,  1108,  1111,  1112,  1113,  1115,  1118,  1117,
    1122,  1123,  1124,  1125,  1126,  1127,  1128,  1129,  1130,  1131,
    1132,  1133,  1134,  1135,  1136,  1137,  1138,  1139,  1143,  1140,
    1154,  1141,  1142,  1144,  1145,  1146,  1147,  1148,  1165,  1149,
    1150,  1176,  1151,  1152,  1153,  1155,  1156,  1187,  1194,  1208,
    1157,  1158,  1159,  1160,  1161,  1162,  1163,  1164,  1166,  1167,
    1168,  1169,  1170,  1171,  1172,  1173,  1174,  1175,  1177,  1178,
    1209,  1179,  1180,  1181,  1182,  1183,  1184,  1185,  1186,  1188,
    1214,  1213,  1215,  1189,  1190,  1191,  1202,  1216,  1203,  1223,
    1204,  1205,  1206,  1207,  1228,  1211,  1212,  1221,  1222,  1230,
    1218,  1227,  1229,  1231,  1234,  1242,  1241,  1243,  1244,  1245,
    1246,  1247,  1248,  1249,  1255,  1261,  1256,  1257,  1258,  1259,
    1260,  1262,  1265,  1266,  1263,   173,   758,  1264,  1268,  1269,
     390,   540,   136,   229,   501,   736,   365,   572,   380,  1031,
     937,   551,   699,   915,   933,   916,   599,   917,  1019,   897,
    1035,   601,   432,   899,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     906
};

static const yytype_int16 yycheck[] =
{
     258,   235,     8,    22,   660,   291,   293,   256,   267,   783,
     168,   542,   270,   608,   734,   684,     8,     3,    54,     4,
       5,     6,     5,     6,     5,     6,     6,     3,   186,   288,
     289,    61,   734,     6,   292,     3,    77,    83,     4,     5,
       6,   734,    81,   734,   734,   734,   204,     4,    75,    76,
      20,    20,    22,    22,   144,    83,    54,    27,    27,    65,
      86,    65,     3,    87,     5,     6,    54,   662,   609,    74,
     739,   740,    67,    68,   224,     3,   226,     4,    37,     6,
       6,   155,   156,    78,    79,    80,   155,   156,    15,    36,
       3,    39,     5,     6,     4,    33,    32,    33,    34,    72,
      48,     3,    38,    39,     3,    72,     5,     6,    65,   155,
     156,    75,    50,   155,   156,   100,   101,   102,    77,    33,
      61,   106,   155,   156,    51,   383,   384,   155,   156,   155,
     156,   155,   156,    60,   100,   101,   102,     3,     7,    33,
     106,   231,   400,    12,   918,    14,    72,    16,    61,   223,
     135,   136,   137,   158,   223,    65,    83,   155,   156,   754,
     419,    39,    61,   130,   133,   423,   424,   155,   156,   135,
     136,   137,   406,     3,   134,   433,   434,   435,   436,   437,
     438,   222,   109,   110,   226,     3,   222,   226,    54,   184,
     223,    32,    33,    34,   224,   222,   482,    38,    39,   127,
     487,   129,    77,   461,   462,   165,   133,   466,   467,   468,
     469,   470,   471,   472,   473,   474,   475,   476,   477,   478,
     479,   941,   226,   150,   151,   152,   206,   168,   247,     3,
     222,    77,   159,   160,   161,   220,   222,   220,   221,   941,
     221,   217,   227,   228,     3,     4,     5,     6,   941,   217,
     941,   941,   941,   223,   220,   168,   797,     3,   194,   134,
     518,     7,   228,     9,    10,    11,    12,    13,    14,   168,
      16,     6,   131,   292,   201,    69,    70,    23,   512,   948,
      26,    86,    28,    29,    30,    33,     6,    35,   134,     8,
     165,     8,     8,   152,   153,   553,   554,   155,   156,   172,
     169,    32,   536,   159,   562,    64,   564,   155,   156,   543,
      41,    61,    72,    72,    73,   155,   156,   244,   577,   165,
     189,    71,     0,    82,   193,    84,     8,   196,   155,   156,
      65,    77,   155,   156,   155,   156,   225,     8,     8,  1113,
    1114,   100,   101,   102,   578,    65,    65,   106,    65,    65,
     155,   156,     8,   194,   213,   214,   215,   216,   217,   218,
     219,   155,   156,   202,   622,  1021,   205,   127,   226,   129,
     130,     8,   132,     8,     8,   223,   135,   136,   137,   138,
      71,   140,   388,   142,     8,   144,   226,   146,    72,   148,
      71,     8,   923,    77,    65,    65,   223,    10,   157,   224,
     223,     3,   223,   422,     3,     4,     5,     6,     3,    65,
       3,    24,    25,   571,   159,   160,   161,    32,    33,    20,
     179,    22,   181,   169,   183,     3,    27,   685,    65,   687,
      65,    65,   177,   221,  1208,   362,  1210,   696,   226,  1034,
      14,    65,   221,   127,     3,   129,   130,   226,    65,    32,
      33,   197,  1226,   222,  1228,   714,   215,   216,   217,   173,
     174,   220,   208,   222,   223,   166,   167,    50,   223,   228,
     171,   226,   399,    72,    73,   709,   222,   221,   120,    76,
     122,   123,   226,    82,   126,    84,   735,    39,    87,    39,
      42,    43,    42,    43,   780,    47,    48,    47,    48,   185,
     186,   100,   101,   102,   190,     6,   221,   106,    83,   221,
      85,   226,    87,   131,   226,    72,    83,     3,    85,   777,
      87,   779,     3,   529,   173,   174,   784,   546,   762,   201,
     202,   203,   150,   151,   152,   153,   135,   136,   137,   138,
     158,   140,    72,   142,     3,   144,   179,   146,   181,   148,
     183,     3,     4,     5,     6,  1211,     3,   223,   157,   578,
     226,     3,     4,     5,     6,    53,    54,    55,    56,   223,
      54,  1227,   226,  1229,   226,     3,     4,     5,     6,     6,
     179,     4,   181,   194,   183,    46,    47,    48,    49,  1245,
     217,   218,   219,    75,   212,   213,   214,   215,   216,   217,
     218,   219,   223,   223,   223,   226,   226,   226,  1264,   896,
     194,   223,  1268,   194,   226,   192,   215,   216,   217,   223,
     223,   220,   226,   222,   194,   223,   226,   227,   226,   228,
      72,    73,   187,   188,   223,    77,   160,   226,   223,    21,
      82,   226,    84,   194,    72,    73,   170,   155,   156,   173,
     174,   175,   223,   177,    82,   226,    84,   223,   100,   101,
     102,    64,   223,   223,   106,   226,   226,   191,   192,    66,
     194,   195,   100,   101,   102,    44,    45,    72,   106,    55,
      56,    57,    58,    59,    60,   223,    62,    63,   226,   223,
     709,     3,   226,   135,   136,   137,   138,     3,   140,    61,
     142,    61,   144,   222,   146,    73,   148,   135,   136,   137,
     138,     3,   140,     3,   142,   157,   144,     3,   146,     3,
     148,   223,   728,   223,   226,   223,   226,   223,   226,   157,
     226,   223,   223,    65,   226,   226,     4,   179,   223,   181,
     130,   183,     3,     4,     5,     6,     5,     6,    44,    45,
     130,   179,     3,   181,     3,   183,     3,     4,     5,     6,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,     4,   215,   216,   217,    42,    43,   220,  1037,
     222,   938,   939,   130,   130,    61,   228,   215,   216,   217,
     222,     4,   220,   165,   222,     6,     3,     6,     4,     3,
     228,     4,  1036,     4,    54,   194,     3,     3,    52,    67,
      73,    72,    73,   133,     3,    61,   212,    77,   222,     4,
      77,    82,   222,    84,   222,    72,     4,     4,     4,   222,
     222,     6,     3,   224,   222,    82,   222,    84,   222,   100,
     101,   102,     6,   222,    46,   106,    46,   222,   222,   222,
      76,     4,    77,   100,   101,   102,     6,    76,     4,   106,
    1118,     4,   223,    68,   223,   222,    54,     3,     3,   222,
     222,   222,   222,     4,   135,   136,   137,   138,    77,   140,
     222,   142,   222,   144,   222,   146,   222,   148,   135,   136,
     137,   138,     4,   140,   229,   142,   157,   144,     4,   146,
     223,   148,    76,     3,   222,   220,   222,     6,     6,     6,
     157,     5,    42,   222,     0,     3,   223,   222,   179,     6,
     181,     7,   183,     9,    10,    11,    12,    13,    14,    15,
      16,    17,   179,     4,   181,   165,   183,    23,   165,   226,
      26,    75,    28,    29,    30,   128,     3,   222,     3,   133,
     158,   223,   226,    75,   215,   216,   217,     4,   222,   220,
     155,   222,    72,   130,   226,     6,     6,   228,   215,   216,
     217,     3,     3,   220,   229,   222,   229,     6,     4,     4,
       7,   228,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    77,   226,   222,     4,    31,    23,   178,   222,    26,
     222,    28,    29,    30,     7,   222,     9,    10,    11,    12,
      13,    14,   222,    16,     4,     6,     4,     6,     3,     6,
      23,   131,     4,    26,   222,    28,    29,    30,   222,     3,
     222,   222,   222,   226,    77,     6,   223,   223,     5,    46,
     150,   151,   152,   153,   223,    49,   212,     6,   158,    23,
      77,   223,   222,   226,     6,   130,    30,   222,    32,    33,
     128,    35,   132,   165,   133,   223,    40,    41,   220,     4,
      72,   226,    74,   222,    77,   223,   223,     6,    89,    90,
      91,    92,    93,   169,     6,    96,    97,    56,    56,     3,
      51,   223,   103,   104,   105,    91,    91,   226,   226,   185,
     226,   226,   212,   213,   214,   215,   216,   217,   218,   219,
     226,   197,   226,   223,   125,   226,    91,   226,   226,   226,
     133,   207,   208,   226,    89,    90,    91,    92,    93,   131,
     226,    96,    97,   226,   226,   226,   222,   226,   103,   104,
     105,   226,   169,   226,   226,   226,   226,   226,   150,   151,
     152,   153,   154,   226,   226,    91,   158,    91,   185,    72,
     125,   226,   226,   226,   226,   226,   169,   226,    72,   133,
     197,    91,   226,   226,   226,   226,   226,   226,   226,     4,
     207,   208,   226,     3,   226,    65,   223,   161,   162,   163,
     164,   226,   166,   167,   197,   222,   170,   171,   172,     6,
      52,   175,   226,   177,   226,   208,   226,   181,   182,   226,
     212,   213,   214,   215,   216,   217,   218,   219,   131,   222,
     194,   195,   222,    53,   198,   199,   200,   131,   226,   226,
     204,     6,   226,   226,   208,   209,   210,   150,   151,   152,
     153,     6,   155,   226,     6,   158,   150,   151,   152,   153,
     226,     6,     6,   226,   158,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   226,   106,   107,   108,   109,   110,   111,   226,
     226,   114,   226,   226,   117,   118,   119,   120,   121,   122,
     123,   124,     6,   226,   226,   226,   226,     6,   226,   212,
     213,   214,   215,   216,   217,   218,   219,   226,   212,   213,
     214,   215,   216,   217,   218,   219,    89,    90,    91,    92,
      93,   226,     6,    96,    97,   226,     6,     6,   226,     6,
     103,   104,   105,    89,    90,    91,    92,    93,     6,     6,
      96,    97,     6,     6,     6,     6,     6,   103,   104,   105,
       6,     6,   125,   226,     6,     6,    89,    90,    91,    92,
      93,     6,     6,    96,    97,     6,     6,     6,     6,   125,
     103,   104,   105,    89,    90,    91,    92,    93,     6,     6,
      96,    97,   226,     6,     6,     6,     6,   103,   104,   105,
       6,     6,   125,     6,     6,     6,     6,   226,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   226,   125,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     226,     6,     4,     4,     4,     4,     4,   223,    61,     6,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
       6,   223,   223,   223,   223,   223,   223,   223,     6,   223,
       6,   223,   223,   223,   223,   223,   223,   223,     6,   223,
     223,     6,   223,   223,   223,   223,   223,     6,   165,     4,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
       4,   223,   223,   223,   223,   223,   223,   223,   223,   223,
       6,    48,     6,   226,   226,   226,   223,   226,   223,     6,
     223,   223,   223,   223,     4,   223,   223,   165,   222,    48,
     226,   223,   223,     4,     6,   223,   222,     6,    47,   223,
     223,    39,    39,     4,   222,     3,   223,    39,    39,    39,
      39,   222,     3,     3,   222,    65,   664,   223,   223,   223,
     242,   401,    22,   139,   364,   622,   203,   459,   231,   936,
     803,   425,   563,   781,   800,   781,   484,   781,   902,   751,
     941,   486,   277,   753,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     769
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
     206,   173,   174,   173,   174,     3,    76,     6,     3,   201,
     202,   203,   202,   205,     6,    72,     6,    72,   280,   280,
       3,    61,    54,   226,     6,   194,   194,   185,   186,   190,
     160,   170,   173,   174,   175,   177,   191,   192,   194,   195,
     194,     4,   192,    75,   194,   194,   280,   223,   223,   233,
      21,   222,   260,   261,    64,   268,    66,   262,    72,     3,
     280,   280,   280,     3,    61,    61,   222,   249,    73,     3,
     280,   280,   280,     3,     3,     3,   252,   253,    65,   273,
       4,   340,   340,     3,     4,     5,     6,    72,    73,    82,
      84,   100,   101,   102,   106,   135,   136,   137,   138,   140,
     142,   144,   146,   148,   157,   179,   181,   183,   215,   216,
     217,   220,   222,   228,   295,   297,   298,   299,   300,   301,
     302,   303,   304,   305,   308,   309,   310,   311,   312,   314,
     315,   316,   317,   318,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   332,   333,   334,   335,   336,   337,
       3,     5,     6,    61,   168,     3,     5,     6,    61,   168,
       3,     5,     6,    61,   168,   223,   130,   130,    39,    42,
      43,    47,    48,     3,     3,   340,     4,   130,   130,    10,
      24,    25,    61,   280,   222,   284,   340,     4,   165,     6,
       3,     6,     4,     3,     4,    54,     4,   194,     3,     3,
     260,   261,   295,    52,    67,   266,    73,   133,    54,   222,
     249,   280,    32,    33,    50,     3,   246,    37,   258,    61,
     212,   226,   273,   298,    77,    77,   222,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,    72,
      73,   299,   222,   222,    87,   298,   313,     4,     4,     4,
       4,     6,   337,   222,   222,   222,   222,   222,   222,   222,
     120,   122,   123,   126,   222,   222,   299,   299,     5,     6,
     221,   318,   330,   331,   258,   298,   227,   317,   223,   226,
      54,   155,   156,    72,    74,   131,   150,   151,   152,   153,
     154,   158,   212,   213,   214,   215,   216,   217,   218,   219,
     224,   221,   226,   221,   226,   221,   226,   226,   227,   221,
     226,   221,   226,     3,     6,    46,    46,    76,   280,    77,
     345,   259,     4,    39,    48,     6,    76,   187,   188,     4,
     223,   223,    81,   269,   263,   264,   298,   298,    68,   267,
       4,   256,     3,   127,   129,   236,   238,   239,   245,    54,
     222,   349,     3,     3,   223,   226,   222,   296,   280,   298,
     253,   222,   222,    64,   223,   295,   222,    72,   258,   298,
     298,   313,    83,    85,    87,   298,   298,   298,   298,   298,
     298,     4,   222,   222,   222,   222,     4,     4,   229,   223,
     223,    76,   297,     3,   298,   298,    74,   158,   222,    72,
     130,   299,   299,   299,   299,   299,   299,   299,   299,   299,
     299,   299,   299,   299,   299,     3,   217,   318,     6,   330,
       6,   331,   317,     6,     5,    42,    44,    45,   222,   222,
       3,   222,   223,     6,    32,    41,     4,   165,   165,   295,
      75,   270,   226,    69,    70,   265,   298,   133,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   106,   107,   108,   109,   110,
     111,   114,   117,   118,   119,   120,   121,   122,   123,   124,
     241,   128,   222,   223,   226,   256,     3,   133,     3,   295,
     226,    67,    68,    78,    79,    80,   184,   338,   339,   338,
     295,   223,   258,   223,    54,    86,    83,    85,   298,   298,
     223,   223,   223,   223,   223,   223,    75,   298,     3,   316,
     298,     4,   226,   272,   223,   226,     5,     6,   340,   222,
     299,   258,   295,   130,   155,   229,   229,     6,     6,   246,
     237,   239,     3,   347,   348,     6,     4,     4,   222,   277,
     278,   279,   280,   285,   178,   271,   264,     4,   222,   222,
     222,   222,   222,   222,   222,   222,   222,    72,   127,   129,
     130,   242,   243,   345,   222,   246,    31,   346,   238,   223,
       4,   223,   222,     6,     6,     4,     3,     6,   223,   226,
     223,   223,   223,   241,   298,   298,    83,    86,   299,   226,
     226,   226,   226,     4,    65,   223,     4,    77,   258,   295,
     223,   223,   299,    49,    46,   223,   223,   226,   212,   223,
     226,     6,   256,   226,    55,    57,    58,    59,    60,    62,
      63,   286,     3,    54,   281,   300,   301,   302,   303,   304,
     305,   306,   307,   273,     6,   241,   240,   241,    89,    90,
      91,    92,    93,    96,    97,   103,   104,   105,   125,    89,
      90,    91,    92,    93,    96,    97,   103,   104,   105,   125,
      89,    90,    91,    92,    93,    96,    97,   103,   104,   105,
     125,    89,    90,    91,    92,    93,    96,    97,   103,   104,
     105,   125,    89,    90,    91,    92,    93,    96,    97,   103,
     104,   105,   125,    89,    90,    91,    92,    93,    96,    97,
     103,   104,   105,   125,   130,   128,   132,   243,   244,   244,
     246,   223,   222,   133,   165,   295,   339,   223,    83,   298,
     223,   298,   318,   220,   319,   322,   327,   332,     4,   226,
     272,   298,   223,   222,   223,   223,     6,     6,   239,     3,
       4,     5,     6,   348,    33,    35,   223,   278,    56,    56,
       3,   226,    51,   275,   223,   226,   223,   223,   226,   226,
     226,   226,   226,   226,   226,   226,   226,   226,    91,   226,
     226,   226,   226,   226,   226,   226,   226,   226,   226,    91,
     226,   226,   226,   226,   226,   226,   226,   226,   226,   226,
      91,   226,   226,   226,   226,   226,   226,   226,   226,   226,
     226,    91,   226,   226,   226,   226,   226,   226,   226,   226,
     226,   226,    91,   226,   226,   226,   226,   226,   226,   226,
     226,   226,   226,    91,   226,   317,   133,   133,   223,   347,
       4,     3,   223,   226,   226,   226,   226,   272,   223,   338,
       6,   281,   279,   279,   222,   306,    52,    53,   274,     6,
     241,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     226,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   226,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   226,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   226,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   226,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   226,     6,     4,     4,   223,
     345,     4,     4,     4,     4,   223,   223,     6,    61,   246,
     295,   298,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,     6,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,     6,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,     6,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,     6,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,     6,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,     6,   223,   226,
     226,   226,   272,   272,   165,    39,    42,    43,    47,    48,
     298,   223,   223,   223,   223,   223,   223,   223,     4,     4,
       6,   223,   223,    48,     6,     6,   226,   272,   226,   272,
     345,   165,   222,     6,    44,    45,     6,   223,     4,   223,
      48,     4,    42,    43,     6,   272,   345,   272,   134,   165,
     345,   222,   223,     6,    47,   223,   223,    39,    39,     4,
     134,   165,   345,   134,   165,   222,   223,    39,    39,    39,
      39,     3,   222,   222,   223,     3,     3,   345,   223,   223,
     345
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
     287,   287,   287,   288,   288,   288,   288,   289,   289,   290,
     290,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     290,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     290,   290,   290,   290,   291,   292,   292,   292,   292,   292,
     292,   292,   292,   292,   292,   292,   292,   292,   292,   292,
     292,   292,   292,   292,   292,   292,   292,   292,   292,   292,
     292,   292,   292,   292,   292,   292,   292,   292,   292,   292,
     292,   293,   293,   293,   294,   294,   295,   295,   296,   296,
     297,   297,   298,   298,   298,   298,   298,   299,   299,   299,
     299,   299,   299,   299,   299,   299,   299,   299,   299,   299,
     300,   300,   300,   301,   301,   301,   301,   302,   302,   302,
     302,   303,   303,   303,   303,   304,   304,   305,   305,   306,
     306,   306,   306,   306,   306,   307,   307,   308,   308,   308,
     308,   308,   308,   308,   308,   308,   308,   308,   308,   308,
     308,   308,   308,   308,   308,   308,   308,   308,   308,   308,
     308,   308,   308,   308,   308,   308,   309,   309,   310,   311,
     311,   312,   312,   312,   312,   313,   313,   314,   315,   315,
     315,   315,   316,   316,   316,   316,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   318,
     318,   318,   318,   319,   319,   319,   320,   321,   321,   322,
     322,   323,   324,   324,   325,   326,   326,   327,   328,   328,
     329,   329,   330,   331,   332,   332,   333,   334,   334,   335,
     336,   336,   337,   337,   337,   337,   337,   337,   337,   337,
     337,   337,   337,   337,   338,   338,   339,   339,   339,   339,
     339,   339,   340,   341,   341,   342,   342,   343,   343,   344,
     344,   345,   345,   346,   346,   347,   347,   348,   348,   348,
     348,   348,   349,   349
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
       4,     3,     3,     4,     2,     3,     3,     4,     2,     3,
       2,     2,     2,     4,     2,     4,     3,     2,     4,     2,
       2,     4,     3,     2,     2,     2,     2,     2,     5,     2,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     6,     6,     5,     3,
       4,     4,     2,     5,     3,     6,     7,     9,    10,    12,
      12,    13,    14,    15,    16,    12,    13,    15,    16,     3,
       4,     5,     6,     3,     3,     4,     3,     3,     4,     4,
       6,     5,     3,     4,     3,     4,     3,     3,     5,     7,
       7,     6,     8,     8,     2,     3,     1,     3,     3,     5,
       3,     1,     1,     1,     1,     1,     1,     3,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
      14,    19,    16,    20,    16,    15,    13,    18,    14,    13,
      11,     8,    10,    13,    15,     5,     7,     4,     6,     1,
       1,     1,     1,     1,     1,     1,     3,     3,     4,     5,
       4,     4,     4,     4,     4,     4,     4,     3,     2,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     6,     3,     4,     3,     3,     5,     5,
       6,     4,     6,     3,     5,     4,     5,     6,     4,     5,
       5,     6,     1,     3,     1,     3,     1,     1,     1,     1,
       1,     2,     2,     2,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     3,     1,
       1,     2,     2,     3,     2,     2,     3,     2,     2,     2,
       2,     3,     3,     3,     1,     1,     2,     2,     3,     2,
       2,     3,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     1,     3,     2,     2,     1,     2,
       2,     2,     1,     2,     0,     3,     0,     1,     0,     2,
       0,     4,     0,     4,     0,     1,     3,     1,     3,     3,
       3,     3,     6,     3
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
#line 2501 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 331 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2509 "parser.cpp"
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
#line 2523 "parser.cpp"
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
#line 2537 "parser.cpp"
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
#line 2551 "parser.cpp"
        break;

    case YYSYMBOL_column_type_array: /* column_type_array  */
#line 192 "parser.y"
            {
    fprintf(stderr, "destroy column_type_array\n");
    delete (((*yyvaluep).column_type_array_t));
}
#line 2560 "parser.cpp"
        break;

    case YYSYMBOL_column_type: /* column_type  */
#line 187 "parser.y"
            {
    fprintf(stderr, "destroy column_type\n");
    delete (((*yyvaluep).column_type_t));
}
#line 2569 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2580 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2588 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 335 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2597 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 335 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2606 "parser.cpp"
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
#line 2620 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 288 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2631 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 370 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2641 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 370 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2651 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 370 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2661 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 370 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2671 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 370 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2681 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 370 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2691 "parser.cpp"
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
#line 2705 "parser.cpp"
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
#line 2719 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 358 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2729 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2737 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2745 "parser.cpp"
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
#line 2759 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 353 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2768 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2776 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2784 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2792 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2800 "parser.cpp"
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
#line 2814 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 353 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2823 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 353 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2832 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 353 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2841 "parser.cpp"
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
#line 2854 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 348 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2863 "parser.cpp"
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
#line 2877 "parser.cpp"
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
#line 2891 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 364 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2901 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 353 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2910 "parser.cpp"
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
#line 2924 "parser.cpp"
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
#line 2938 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2946 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2954 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2962 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2970 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2978 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2986 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2994 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3002 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3010 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3018 "parser.cpp"
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
#line 3032 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3040 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3048 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3056 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3064 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3072 "parser.cpp"
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
#line 3085 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3093 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3101 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 340 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3109 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3117 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3125 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3133 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3141 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3149 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3157 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3165 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3173 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3181 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3189 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3197 "parser.cpp"
        break;

    case YYSYMBOL_curly_brackets_expr: /* curly_brackets_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3205 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_curly_brackets_expr: /* unclosed_curly_brackets_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3213 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 385 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3221 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 389 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3229 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3237 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3245 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3253 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3261 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3269 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 344 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3277 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 331 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3285 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 221 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3296 "parser.cpp"
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
#line 3310 "parser.cpp"
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
#line 3324 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 197 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3335 "parser.cpp"
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

#line 3443 "parser.cpp"

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
#line 3658 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 525 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3669 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 531 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3680 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 538 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3686 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 539 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3692 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 540 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3698 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 541 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3704 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 542 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3710 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 543 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3716 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 544 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3722 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 545 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3728 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 546 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3734 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 547 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3740 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 548 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3746 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 549 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3752 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 550 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3758 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 551 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3764 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 552 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3770 "parser.cpp"
    break;

  case 20: /* statement: check_statement  */
#line 553 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].check_stmt); }
#line 3776 "parser.cpp"
    break;

  case 21: /* explainable_statement: create_statement  */
#line 555 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3782 "parser.cpp"
    break;

  case 22: /* explainable_statement: drop_statement  */
#line 556 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3788 "parser.cpp"
    break;

  case 23: /* explainable_statement: copy_statement  */
#line 557 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3794 "parser.cpp"
    break;

  case 24: /* explainable_statement: show_statement  */
#line 558 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3800 "parser.cpp"
    break;

  case 25: /* explainable_statement: select_statement  */
#line 559 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3806 "parser.cpp"
    break;

  case 26: /* explainable_statement: delete_statement  */
#line 560 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3812 "parser.cpp"
    break;

  case 27: /* explainable_statement: update_statement  */
#line 561 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3818 "parser.cpp"
    break;

  case 28: /* explainable_statement: insert_statement  */
#line 562 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3824 "parser.cpp"
    break;

  case 29: /* explainable_statement: flush_statement  */
#line 563 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3830 "parser.cpp"
    break;

  case 30: /* explainable_statement: optimize_statement  */
#line 564 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3836 "parser.cpp"
    break;

  case 31: /* explainable_statement: command_statement  */
#line 565 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3842 "parser.cpp"
    break;

  case 32: /* explainable_statement: compact_statement  */
#line 566 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3848 "parser.cpp"
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
#line 3870 "parser.cpp"
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
#line 3890 "parser.cpp"
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
#line 3908 "parser.cpp"
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
#line 3941 "parser.cpp"
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
#line 3961 "parser.cpp"
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
#line 3997 "parser.cpp"
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
#line 4019 "parser.cpp"
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
#line 4040 "parser.cpp"
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
#line 4073 "parser.cpp"
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
#line 4108 "parser.cpp"
    break;

  case 43: /* table_element_array: table_element  */
#line 794 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 4117 "parser.cpp"
    break;

  case 44: /* table_element_array: table_element_array ',' table_element  */
#line 798 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 4126 "parser.cpp"
    break;

  case 45: /* column_def_array: table_column  */
#line 803 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 4135 "parser.cpp"
    break;

  case 46: /* column_def_array: column_def_array ',' table_column  */
#line 807 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 4144 "parser.cpp"
    break;

  case 47: /* table_element: table_column  */
#line 813 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 4152 "parser.cpp"
    break;

  case 48: /* table_element: table_constraint  */
#line 816 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 4160 "parser.cpp"
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
#line 4185 "parser.cpp"
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
#line 4212 "parser.cpp"
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
#line 4241 "parser.cpp"
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
#line 4271 "parser.cpp"
    break;

  case 53: /* column_type_array: column_type  */
#line 915 "parser.y"
                                {
    (yyval.column_type_array_t) = new std::vector<std::unique_ptr<infinity::ColumnType>>();
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4280 "parser.cpp"
    break;

  case 54: /* column_type_array: column_type_array ',' column_type  */
#line 919 "parser.y"
                                    {
    (yyval.column_type_array_t) = (yyvsp[-2].column_type_array_t);
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4289 "parser.cpp"
    break;

  case 55: /* column_type: BOOLEAN  */
#line 925 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4295 "parser.cpp"
    break;

  case 56: /* column_type: TINYINT  */
#line 926 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4301 "parser.cpp"
    break;

  case 57: /* column_type: SMALLINT  */
#line 927 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4307 "parser.cpp"
    break;

  case 58: /* column_type: INTEGER  */
#line 928 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4313 "parser.cpp"
    break;

  case 59: /* column_type: INT  */
#line 929 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4319 "parser.cpp"
    break;

  case 60: /* column_type: BIGINT  */
#line 930 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4325 "parser.cpp"
    break;

  case 61: /* column_type: HUGEINT  */
#line 931 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4331 "parser.cpp"
    break;

  case 62: /* column_type: FLOAT  */
#line 932 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4337 "parser.cpp"
    break;

  case 63: /* column_type: REAL  */
#line 933 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4343 "parser.cpp"
    break;

  case 64: /* column_type: DOUBLE  */
#line 934 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4349 "parser.cpp"
    break;

  case 65: /* column_type: FLOAT16  */
#line 935 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4355 "parser.cpp"
    break;

  case 66: /* column_type: BFLOAT16  */
#line 936 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4361 "parser.cpp"
    break;

  case 67: /* column_type: DATE  */
#line 937 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4367 "parser.cpp"
    break;

  case 68: /* column_type: TIME  */
#line 938 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4373 "parser.cpp"
    break;

  case 69: /* column_type: DATETIME  */
#line 939 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4379 "parser.cpp"
    break;

  case 70: /* column_type: TIMESTAMP  */
#line 940 "parser.y"
            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4385 "parser.cpp"
    break;

  case 71: /* column_type: UUID  */
#line 941 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4391 "parser.cpp"
    break;

  case 72: /* column_type: POINT  */
#line 942 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4397 "parser.cpp"
    break;

  case 73: /* column_type: LINE  */
#line 943 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4403 "parser.cpp"
    break;

  case 74: /* column_type: LSEG  */
#line 944 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4409 "parser.cpp"
    break;

  case 75: /* column_type: BOX  */
#line 945 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4415 "parser.cpp"
    break;

  case 76: /* column_type: CIRCLE  */
#line 948 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4421 "parser.cpp"
    break;

  case 77: /* column_type: VARCHAR  */
#line 950 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4427 "parser.cpp"
    break;

  case 78: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 951 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4433 "parser.cpp"
    break;

  case 79: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 952 "parser.y"
                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4439 "parser.cpp"
    break;

  case 80: /* column_type: DECIMAL  */
#line 953 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4445 "parser.cpp"
    break;

  case 81: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 956 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4451 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 957 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4457 "parser.cpp"
    break;

  case 83: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 958 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4463 "parser.cpp"
    break;

  case 84: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 959 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4469 "parser.cpp"
    break;

  case 85: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 960 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4475 "parser.cpp"
    break;

  case 86: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 961 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4481 "parser.cpp"
    break;

  case 87: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 962 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4487 "parser.cpp"
    break;

  case 88: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 963 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4493 "parser.cpp"
    break;

  case 89: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 964 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4499 "parser.cpp"
    break;

  case 90: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 965 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4505 "parser.cpp"
    break;

  case 91: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 966 "parser.y"
                                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4511 "parser.cpp"
    break;

  case 92: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 967 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4517 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 968 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4523 "parser.cpp"
    break;

  case 94: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 969 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4529 "parser.cpp"
    break;

  case 95: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 970 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4535 "parser.cpp"
    break;

  case 96: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 971 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4541 "parser.cpp"
    break;

  case 97: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 972 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4547 "parser.cpp"
    break;

  case 98: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 973 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4553 "parser.cpp"
    break;

  case 99: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 974 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4559 "parser.cpp"
    break;

  case 100: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 975 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4565 "parser.cpp"
    break;

  case 101: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 976 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4571 "parser.cpp"
    break;

  case 102: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 977 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4577 "parser.cpp"
    break;

  case 103: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 978 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4583 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 979 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4589 "parser.cpp"
    break;

  case 105: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 980 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4595 "parser.cpp"
    break;

  case 106: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 981 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4601 "parser.cpp"
    break;

  case 107: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 982 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4607 "parser.cpp"
    break;

  case 108: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 983 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4613 "parser.cpp"
    break;

  case 109: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 984 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4619 "parser.cpp"
    break;

  case 110: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 985 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4625 "parser.cpp"
    break;

  case 111: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 986 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4631 "parser.cpp"
    break;

  case 112: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 987 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4637 "parser.cpp"
    break;

  case 113: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 988 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4643 "parser.cpp"
    break;

  case 114: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 989 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4649 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 990 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4655 "parser.cpp"
    break;

  case 116: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 991 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4661 "parser.cpp"
    break;

  case 117: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 992 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4667 "parser.cpp"
    break;

  case 118: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 993 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4673 "parser.cpp"
    break;

  case 119: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 994 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4679 "parser.cpp"
    break;

  case 120: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 995 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4685 "parser.cpp"
    break;

  case 121: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 996 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4691 "parser.cpp"
    break;

  case 122: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 997 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4697 "parser.cpp"
    break;

  case 123: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 998 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4703 "parser.cpp"
    break;

  case 124: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 999 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4709 "parser.cpp"
    break;

  case 125: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 1000 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4715 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1001 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4721 "parser.cpp"
    break;

  case 127: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1002 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4727 "parser.cpp"
    break;

  case 128: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1003 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4733 "parser.cpp"
    break;

  case 129: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1004 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4739 "parser.cpp"
    break;

  case 130: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1005 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4745 "parser.cpp"
    break;

  case 131: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1006 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4751 "parser.cpp"
    break;

  case 132: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1007 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4757 "parser.cpp"
    break;

  case 133: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1008 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4763 "parser.cpp"
    break;

  case 134: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1009 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4769 "parser.cpp"
    break;

  case 135: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4775 "parser.cpp"
    break;

  case 136: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1011 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4781 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1012 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4787 "parser.cpp"
    break;

  case 138: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1013 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4793 "parser.cpp"
    break;

  case 139: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1014 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4799 "parser.cpp"
    break;

  case 140: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4805 "parser.cpp"
    break;

  case 141: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4811 "parser.cpp"
    break;

  case 142: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1017 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4817 "parser.cpp"
    break;

  case 143: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1018 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4823 "parser.cpp"
    break;

  case 144: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1019 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4829 "parser.cpp"
    break;

  case 145: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1020 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4835 "parser.cpp"
    break;

  case 146: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1021 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4841 "parser.cpp"
    break;

  case 147: /* column_type: ARRAY '(' column_type ')'  */
#line 1022 "parser.y"
                            {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kArray, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_.emplace_back((yyvsp[-1].column_type_t));
}
#line 4850 "parser.cpp"
    break;

  case 148: /* column_type: TUPLE '(' column_type_array ')'  */
#line 1026 "parser.y"
                                  {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTuple, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_ = std::move(*((yyvsp[-1].column_type_array_t)));
  delete (yyvsp[-1].column_type_array_t);
}
#line 4860 "parser.cpp"
    break;

  case 149: /* column_constraints: column_constraint  */
#line 1049 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4869 "parser.cpp"
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
#line 4883 "parser.cpp"
    break;

  case 151: /* column_constraint: PRIMARY KEY  */
#line 1063 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4891 "parser.cpp"
    break;

  case 152: /* column_constraint: UNIQUE  */
#line 1066 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4899 "parser.cpp"
    break;

  case 153: /* column_constraint: NULLABLE  */
#line 1069 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4907 "parser.cpp"
    break;

  case 154: /* column_constraint: NOT NULLABLE  */
#line 1072 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4915 "parser.cpp"
    break;

  case 155: /* default_expr: DEFAULT constant_expr  */
#line 1076 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4923 "parser.cpp"
    break;

  case 156: /* default_expr: %empty  */
#line 1079 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4931 "parser.cpp"
    break;

  case 157: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1084 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4941 "parser.cpp"
    break;

  case 158: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1089 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4951 "parser.cpp"
    break;

  case 159: /* identifier_array: IDENTIFIER  */
#line 1096 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4962 "parser.cpp"
    break;

  case 160: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1102 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4973 "parser.cpp"
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
#line 4990 "parser.cpp"
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
#line 5033 "parser.cpp"
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
#line 5053 "parser.cpp"
    break;

  case 164: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1182 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 5061 "parser.cpp"
    break;

  case 165: /* optional_identifier_array: %empty  */
#line 1185 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 5069 "parser.cpp"
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
#line 5092 "parser.cpp"
    break;

  case 167: /* explain_statement: EXPLAIN explainable_statement  */
#line 1209 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 5102 "parser.cpp"
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
#line 5119 "parser.cpp"
    break;

  case 169: /* update_expr_array: update_expr  */
#line 1231 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 5128 "parser.cpp"
    break;

  case 170: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1235 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 5137 "parser.cpp"
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
#line 5149 "parser.cpp"
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
#line 5165 "parser.cpp"
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
#line 5183 "parser.cpp"
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
#line 5201 "parser.cpp"
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
#line 5219 "parser.cpp"
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
#line 5242 "parser.cpp"
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
#line 5300 "parser.cpp"
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
#line 5360 "parser.cpp"
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
#line 5412 "parser.cpp"
    break;

  case 180: /* select_statement: select_without_paren  */
#line 1493 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5420 "parser.cpp"
    break;

  case 181: /* select_statement: select_with_paren  */
#line 1496 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5428 "parser.cpp"
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
#line 5442 "parser.cpp"
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
#line 5456 "parser.cpp"
    break;

  case 184: /* select_with_paren: '(' select_without_paren ')'  */
#line 1518 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5464 "parser.cpp"
    break;

  case 185: /* select_with_paren: '(' select_with_paren ')'  */
#line 1521 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5472 "parser.cpp"
    break;

  case 186: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1525 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5481 "parser.cpp"
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
#line 5512 "parser.cpp"
    break;

  case 188: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1557 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5520 "parser.cpp"
    break;

  case 189: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1560 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5528 "parser.cpp"
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
#line 5549 "parser.cpp"
    break;

  case 191: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1582 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5557 "parser.cpp"
    break;

  case 192: /* order_by_clause: %empty  */
#line 1585 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5565 "parser.cpp"
    break;

  case 193: /* order_by_expr_list: order_by_expr  */
#line 1589 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5574 "parser.cpp"
    break;

  case 194: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1593 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5583 "parser.cpp"
    break;

  case 195: /* order_by_expr: expr order_by_type  */
#line 1598 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5593 "parser.cpp"
    break;

  case 196: /* order_by_type: ASC  */
#line 1604 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5601 "parser.cpp"
    break;

  case 197: /* order_by_type: DESC  */
#line 1607 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5609 "parser.cpp"
    break;

  case 198: /* order_by_type: %empty  */
#line 1610 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5617 "parser.cpp"
    break;

  case 199: /* limit_expr: LIMIT expr  */
#line 1614 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5625 "parser.cpp"
    break;

  case 200: /* limit_expr: %empty  */
#line 1618 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5631 "parser.cpp"
    break;

  case 201: /* offset_expr: OFFSET expr  */
#line 1620 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5639 "parser.cpp"
    break;

  case 202: /* offset_expr: %empty  */
#line 1624 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5645 "parser.cpp"
    break;

  case 203: /* distinct: DISTINCT  */
#line 1626 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5653 "parser.cpp"
    break;

  case 204: /* distinct: %empty  */
#line 1629 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5661 "parser.cpp"
    break;

  case 205: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1633 "parser.y"
                                       {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5669 "parser.cpp"
    break;

  case 206: /* highlight_clause: %empty  */
#line 1636 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5677 "parser.cpp"
    break;

  case 207: /* from_clause: FROM table_reference  */
#line 1640 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5685 "parser.cpp"
    break;

  case 208: /* from_clause: %empty  */
#line 1643 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5693 "parser.cpp"
    break;

  case 209: /* search_clause: SEARCH sub_search_array  */
#line 1647 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5703 "parser.cpp"
    break;

  case 210: /* search_clause: %empty  */
#line 1652 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5711 "parser.cpp"
    break;

  case 211: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1656 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5719 "parser.cpp"
    break;

  case 212: /* optional_search_filter_expr: %empty  */
#line 1659 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5727 "parser.cpp"
    break;

  case 213: /* where_clause: WHERE expr  */
#line 1663 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5735 "parser.cpp"
    break;

  case 214: /* where_clause: %empty  */
#line 1666 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5743 "parser.cpp"
    break;

  case 215: /* having_clause: HAVING expr  */
#line 1670 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5751 "parser.cpp"
    break;

  case 216: /* having_clause: %empty  */
#line 1673 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5759 "parser.cpp"
    break;

  case 217: /* group_by_clause: GROUP BY expr_array  */
#line 1677 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5767 "parser.cpp"
    break;

  case 218: /* group_by_clause: %empty  */
#line 1680 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5775 "parser.cpp"
    break;

  case 219: /* set_operator: UNION  */
#line 1684 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5783 "parser.cpp"
    break;

  case 220: /* set_operator: UNION ALL  */
#line 1687 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5791 "parser.cpp"
    break;

  case 221: /* set_operator: INTERSECT  */
#line 1690 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5799 "parser.cpp"
    break;

  case 222: /* set_operator: EXCEPT  */
#line 1693 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5807 "parser.cpp"
    break;

  case 223: /* table_reference: table_reference_unit  */
#line 1701 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5815 "parser.cpp"
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
#line 5833 "parser.cpp"
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
#line 5851 "parser.cpp"
    break;

  case 228: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1735 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5862 "parser.cpp"
    break;

  case 229: /* table_name: IDENTIFIER  */
#line 1744 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5872 "parser.cpp"
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
#line 5884 "parser.cpp"
    break;

  case 231: /* table_alias: AS IDENTIFIER  */
#line 1758 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5894 "parser.cpp"
    break;

  case 232: /* table_alias: IDENTIFIER  */
#line 1763 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5904 "parser.cpp"
    break;

  case 233: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1768 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5915 "parser.cpp"
    break;

  case 234: /* table_alias: %empty  */
#line 1774 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5923 "parser.cpp"
    break;

  case 235: /* with_clause: WITH with_expr_list  */
#line 1781 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5931 "parser.cpp"
    break;

  case 236: /* with_clause: %empty  */
#line 1784 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5939 "parser.cpp"
    break;

  case 237: /* with_expr_list: with_expr  */
#line 1788 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5948 "parser.cpp"
    break;

  case 238: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1791 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5957 "parser.cpp"
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
#line 5969 "parser.cpp"
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
#line 5981 "parser.cpp"
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
#line 5994 "parser.cpp"
    break;

  case 242: /* join_type: INNER  */
#line 1829 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 6002 "parser.cpp"
    break;

  case 243: /* join_type: LEFT  */
#line 1832 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 6010 "parser.cpp"
    break;

  case 244: /* join_type: RIGHT  */
#line 1835 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 6018 "parser.cpp"
    break;

  case 245: /* join_type: OUTER  */
#line 1838 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6026 "parser.cpp"
    break;

  case 246: /* join_type: FULL  */
#line 1841 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6034 "parser.cpp"
    break;

  case 247: /* join_type: CROSS  */
#line 1844 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 6042 "parser.cpp"
    break;

  case 248: /* join_type: %empty  */
#line 1847 "parser.y"
                {
}
#line 6049 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW DATABASES  */
#line 1853 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 6058 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW TABLES  */
#line 1857 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 6067 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW TASKS  */
#line 1861 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTasks;
}
#line 6076 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW CONFIGS  */
#line 1865 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 6085 "parser.cpp"
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
#line 6097 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW PROFILES  */
#line 1876 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 6106 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW BUFFER  */
#line 1880 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 6115 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW MEMINDEX  */
#line 1884 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 6124 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW QUERIES  */
#line 1888 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 6133 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1892 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 6143 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW TRANSACTIONS  */
#line 1897 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 6152 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1901 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6162 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW TRANSACTION HISTORY  */
#line 1906 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactionHistory;
}
#line 6171 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW SESSION VARIABLES  */
#line 1910 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 6180 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1914 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 6189 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1918 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6200 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1924 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6211 "parser.cpp"
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
#line 6223 "parser.cpp"
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
#line 6239 "parser.cpp"
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
#line 6255 "parser.cpp"
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
#line 6271 "parser.cpp"
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
#line 6288 "parser.cpp"
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
#line 6305 "parser.cpp"
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
#line 6323 "parser.cpp"
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
#line 6342 "parser.cpp"
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
#line 6358 "parser.cpp"
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
#line 6378 "parser.cpp"
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
#line 6400 "parser.cpp"
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
#line 6423 "parser.cpp"
    break;

  case 278: /* show_statement: SHOW LOGS  */
#line 2082 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6432 "parser.cpp"
    break;

  case 279: /* show_statement: SHOW CATALOG  */
#line 2086 "parser.y"
               {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalog;
}
#line 6441 "parser.cpp"
    break;

  case 280: /* show_statement: SHOW CATALOG TO file_path  */
#line 2090 "parser.y"
                            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogToFile;
      (yyval.show_stmt)->file_path_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6452 "parser.cpp"
    break;

  case 281: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2096 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6461 "parser.cpp"
    break;

  case 282: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2100 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6470 "parser.cpp"
    break;

  case 283: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2104 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6481 "parser.cpp"
    break;

  case 284: /* show_statement: SHOW MEMORY  */
#line 2110 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6490 "parser.cpp"
    break;

  case 285: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2114 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6499 "parser.cpp"
    break;

  case 286: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2118 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6508 "parser.cpp"
    break;

  case 287: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2122 "parser.y"
                          {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
      ParserHelper::ToLower((yyvsp[-2].str_value));
      (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
      free((yyvsp[-2].str_value));
}
#line 6520 "parser.cpp"
    break;

  case 288: /* show_statement: SHOW SNAPSHOTS  */
#line 2129 "parser.y"
                 {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListSnapshots;
}
#line 6529 "parser.cpp"
    break;

  case 289: /* show_statement: SHOW SNAPSHOT IDENTIFIER  */
#line 2133 "parser.y"
                           {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowSnapshot;
     ParserHelper::ToLower((yyvsp[0].str_value));
     (yyval.show_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6541 "parser.cpp"
    break;

  case 290: /* show_statement: SHOW CACHES  */
#line 2140 "parser.y"
              {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCaches;
}
#line 6550 "parser.cpp"
    break;

  case 291: /* show_statement: SHOW CACHE  */
#line 2144 "parser.y"
             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowCache;
}
#line 6559 "parser.cpp"
    break;

  case 292: /* show_statement: SHOW COMPACT  */
#line 2148 "parser.y"
               {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCompact;
}
#line 6568 "parser.cpp"
    break;

  case 293: /* show_statement: SHOW COMPACT NOT NULLABLE  */
#line 2152 "parser.y"
                            {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCompact;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6578 "parser.cpp"
    break;

  case 294: /* show_statement: SHOW CHECKPOINT  */
#line 2157 "parser.y"
                  {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCheckpoint;
}
#line 6587 "parser.cpp"
    break;

  case 295: /* show_statement: SHOW CHECKPOINT NOT NULLABLE  */
#line 2161 "parser.y"
                               {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCheckpoint;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6597 "parser.cpp"
    break;

  case 296: /* show_statement: SHOW CHECKPOINT LONG_VALUE  */
#line 2166 "parser.y"
                             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowCheckpoint;
     (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6607 "parser.cpp"
    break;

  case 297: /* show_statement: SHOW OPTIMIZE  */
#line 2171 "parser.y"
                {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListOptimize;
}
#line 6616 "parser.cpp"
    break;

  case 298: /* show_statement: SHOW OPTIMIZE NOT NULLABLE  */
#line 2175 "parser.y"
                             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListOptimize;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6626 "parser.cpp"
    break;

  case 299: /* show_statement: SHOW IMPORT  */
#line 2180 "parser.y"
              {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListImport;
}
#line 6635 "parser.cpp"
    break;

  case 300: /* show_statement: SHOW CLEAN  */
#line 2184 "parser.y"
             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListClean;
}
#line 6644 "parser.cpp"
    break;

  case 301: /* show_statement: SHOW CLEAN NOT NULLABLE  */
#line 2188 "parser.y"
                          {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListClean;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6654 "parser.cpp"
    break;

  case 302: /* show_statement: SHOW CLEAN LONG_VALUE  */
#line 2193 "parser.y"
                        {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowClean;
     (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6664 "parser.cpp"
    break;

  case 303: /* flush_statement: FLUSH DATA  */
#line 2202 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6673 "parser.cpp"
    break;

  case 304: /* flush_statement: FLUSH LOG  */
#line 2206 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6682 "parser.cpp"
    break;

  case 305: /* flush_statement: FLUSH BUFFER  */
#line 2210 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6691 "parser.cpp"
    break;

  case 306: /* flush_statement: FLUSH CATALOG  */
#line 2214 "parser.y"
                {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kCatalog;
}
#line 6700 "parser.cpp"
    break;

  case 307: /* optimize_statement: OPTIMIZE table_name  */
#line 2222 "parser.y"
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
#line 6715 "parser.cpp"
    break;

  case 308: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 2233 "parser.y"
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
#line 6740 "parser.cpp"
    break;

  case 309: /* command_statement: USE IDENTIFIER  */
#line 2257 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6751 "parser.cpp"
    break;

  case 310: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2263 "parser.y"
                                       {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6761 "parser.cpp"
    break;

  case 311: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2268 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6772 "parser.cpp"
    break;

  case 312: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2274 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6783 "parser.cpp"
    break;

  case 313: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2280 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6796 "parser.cpp"
    break;

  case 314: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2288 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6807 "parser.cpp"
    break;

  case 315: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2294 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6818 "parser.cpp"
    break;

  case 316: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2300 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6829 "parser.cpp"
    break;

  case 317: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2306 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6840 "parser.cpp"
    break;

  case 318: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2312 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6853 "parser.cpp"
    break;

  case 319: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2320 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6864 "parser.cpp"
    break;

  case 320: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2326 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6875 "parser.cpp"
    break;

  case 321: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2332 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6886 "parser.cpp"
    break;

  case 322: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2338 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6897 "parser.cpp"
    break;

  case 323: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2344 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6910 "parser.cpp"
    break;

  case 324: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2352 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6921 "parser.cpp"
    break;

  case 325: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2358 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6932 "parser.cpp"
    break;

  case 326: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON TABLE IDENTIFIER  */
#line 2364 "parser.y"
                                                 {
    ParserHelper::ToLower((yyvsp[-3].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kTable, (yyvsp[0].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[0].str_value));
}
#line 6945 "parser.cpp"
    break;

  case 327: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON DATABASE IDENTIFIER  */
#line 2372 "parser.y"
                                                    {
    ParserHelper::ToLower((yyvsp[-3].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kDatabase, (yyvsp[0].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[0].str_value));
}
#line 6958 "parser.cpp"
    break;

  case 328: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON SYSTEM  */
#line 2380 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-2].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kSystem);
    free((yyvsp[-2].str_value));
}
#line 6969 "parser.cpp"
    break;

  case 329: /* command_statement: DROP SNAPSHOT IDENTIFIER  */
#line 2386 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kDrop, infinity::SnapshotScope::kIgnore);
    free((yyvsp[0].str_value));
}
#line 6980 "parser.cpp"
    break;

  case 330: /* command_statement: RESTORE DATABASE SNAPSHOT IDENTIFIER  */
#line 2392 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kDatabase);
    free((yyvsp[0].str_value));
}
#line 6991 "parser.cpp"
    break;

  case 331: /* command_statement: RESTORE TABLE SNAPSHOT IDENTIFIER  */
#line 2398 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kTable);
    free((yyvsp[0].str_value));
}
#line 7002 "parser.cpp"
    break;

  case 332: /* command_statement: CLEAN DATA  */
#line 2404 "parser.y"
             {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::CleanupCmd>();
}
#line 7011 "parser.cpp"
    break;

  case 333: /* command_statement: DUMP INDEX IDENTIFIER ON table_name  */
#line 2408 "parser.y"
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
#line 7027 "parser.cpp"
    break;

  case 334: /* compact_statement: COMPACT TABLE table_name  */
#line 2420 "parser.y"
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
#line 7044 "parser.cpp"
    break;

  case 335: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2433 "parser.y"
                                                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 7055 "parser.cpp"
    break;

  case 336: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2439 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 7067 "parser.cpp"
    break;

  case 337: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2446 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 7080 "parser.cpp"
    break;

  case 338: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2454 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 7094 "parser.cpp"
    break;

  case 339: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2463 "parser.y"
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
#line 7109 "parser.cpp"
    break;

  case 340: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2473 "parser.y"
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
#line 7124 "parser.cpp"
    break;

  case 341: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2483 "parser.y"
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
#line 7140 "parser.cpp"
    break;

  case 342: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2494 "parser.y"
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
#line 7156 "parser.cpp"
    break;

  case 343: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2505 "parser.y"
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
#line 7173 "parser.cpp"
    break;

  case 344: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2517 "parser.y"
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
#line 7190 "parser.cpp"
    break;

  case 345: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2529 "parser.y"
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
#line 7205 "parser.cpp"
    break;

  case 346: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2539 "parser.y"
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
#line 7221 "parser.cpp"
    break;

  case 347: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2550 "parser.y"
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
#line 7238 "parser.cpp"
    break;

  case 348: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2562 "parser.y"
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
#line 7256 "parser.cpp"
    break;

  case 349: /* admin_statement: ADMIN SHOW LOGS  */
#line 2575 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 7265 "parser.cpp"
    break;

  case 350: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2579 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 7275 "parser.cpp"
    break;

  case 351: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2584 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 7285 "parser.cpp"
    break;

  case 352: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2589 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 7296 "parser.cpp"
    break;

  case 353: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2595 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 7305 "parser.cpp"
    break;

  case 354: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2599 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 7314 "parser.cpp"
    break;

  case 355: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2603 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     ParserHelper::ToLower((yyvsp[0].str_value));
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7326 "parser.cpp"
    break;

  case 356: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2610 "parser.y"
                        {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
}
#line 7335 "parser.cpp"
    break;

  case 357: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2614 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
}
#line 7344 "parser.cpp"
    break;

  case 358: /* admin_statement: ADMIN SHOW SNAPSHOT IDENTIFIER  */
#line 2618 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSnapshot;
     ParserHelper::ToLower((yyvsp[0].str_value));
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7356 "parser.cpp"
    break;

  case 359: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2625 "parser.y"
                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7367 "parser.cpp"
    break;

  case 360: /* admin_statement: ADMIN EXPORT SNAPSHOT STRING TO STRING  */
#line 2631 "parser.y"
                                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kExportSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[-2].str_value);
     (yyval.admin_stmt)->export_path_ = (yyvsp[0].str_value);
     free((yyvsp[-2].str_value));
     free((yyvsp[0].str_value));
}
#line 7380 "parser.cpp"
    break;

  case 361: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2639 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7391 "parser.cpp"
    break;

  case 362: /* admin_statement: ADMIN SHOW NODES  */
#line 2645 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 7400 "parser.cpp"
    break;

  case 363: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2649 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7411 "parser.cpp"
    break;

  case 364: /* admin_statement: ADMIN SHOW NODE  */
#line 2655 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 7420 "parser.cpp"
    break;

  case 365: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2659 "parser.y"
                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7431 "parser.cpp"
    break;

  case 366: /* admin_statement: ADMIN SET ADMIN  */
#line 2665 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
}
#line 7441 "parser.cpp"
    break;

  case 367: /* admin_statement: ADMIN SET STANDALONE  */
#line 2670 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
}
#line 7451 "parser.cpp"
    break;

  case 368: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2675 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7463 "parser.cpp"
    break;

  case 369: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2682 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7477 "parser.cpp"
    break;

  case 370: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2691 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7491 "parser.cpp"
    break;

  case 371: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2701 "parser.y"
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
#line 7506 "parser.cpp"
    break;

  case 372: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2711 "parser.y"
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
#line 7523 "parser.cpp"
    break;

  case 373: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2723 "parser.y"
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
#line 7539 "parser.cpp"
    break;

  case 374: /* check_statement: CHECK SYSTEM  */
#line 2735 "parser.y"
                               {
    (yyval.check_stmt) = new infinity::CheckStatement();
    (yyval.check_stmt)->check_type_ = infinity::CheckStmtType::kSystem;
}
#line 7548 "parser.cpp"
    break;

  case 375: /* check_statement: CHECK TABLE table_name  */
#line 2739 "parser.y"
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
#line 7564 "parser.cpp"
    break;

  case 376: /* expr_array: expr_alias  */
#line 2755 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7573 "parser.cpp"
    break;

  case 377: /* expr_array: expr_array ',' expr_alias  */
#line 2759 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7582 "parser.cpp"
    break;

  case 378: /* insert_row_list: '(' expr_array ')'  */
#line 2764 "parser.y"
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
#line 7597 "parser.cpp"
    break;

  case 379: /* insert_row_list: insert_row_list ',' '(' expr_array ')'  */
#line 2774 "parser.y"
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
#line 7612 "parser.cpp"
    break;

  case 380: /* expr_alias: expr AS IDENTIFIER  */
#line 2796 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7623 "parser.cpp"
    break;

  case 381: /* expr_alias: expr  */
#line 2802 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7631 "parser.cpp"
    break;

  case 387: /* operand: '(' expr ')'  */
#line 2812 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 7639 "parser.cpp"
    break;

  case 388: /* operand: '(' select_without_paren ')'  */
#line 2815 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7650 "parser.cpp"
    break;

  case 389: /* operand: constant_expr  */
#line 2821 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7658 "parser.cpp"
    break;

  case 400: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2837 "parser.y"
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
#line 7678 "parser.cpp"
    break;

  case 401: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')'  */
#line 2853 "parser.y"
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
#line 7700 "parser.cpp"
    break;

  case 402: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' IGNORE INDEX  */
#line 2871 "parser.y"
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
#line 7721 "parser.cpp"
    break;

  case 403: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2889 "parser.y"
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
#line 7783 "parser.cpp"
    break;

  case 404: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2947 "parser.y"
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
#line 7837 "parser.cpp"
    break;

  case 405: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2997 "parser.y"
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
#line 7895 "parser.cpp"
    break;

  case 406: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3051 "parser.y"
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
#line 7954 "parser.cpp"
    break;

  case 407: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 3109 "parser.y"
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
#line 7983 "parser.cpp"
    break;

  case 408: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 3134 "parser.y"
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
#line 8010 "parser.cpp"
    break;

  case 409: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 3157 "parser.y"
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
#line 8035 "parser.cpp"
    break;

  case 410: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3178 "parser.y"
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
#line 8060 "parser.cpp"
    break;

  case 411: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3199 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8074 "parser.cpp"
    break;

  case 412: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3208 "parser.y"
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
#line 8090 "parser.cpp"
    break;

  case 413: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3219 "parser.y"
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
#line 8106 "parser.cpp"
    break;

  case 414: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3230 "parser.y"
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
#line 8124 "parser.cpp"
    break;

  case 415: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 3244 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8136 "parser.cpp"
    break;

  case 416: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3251 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8150 "parser.cpp"
    break;

  case 417: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3261 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 8161 "parser.cpp"
    break;

  case 418: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 3267 "parser.y"
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
#line 8177 "parser.cpp"
    break;

  case 419: /* sub_search: match_vector_expr  */
#line 3279 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8185 "parser.cpp"
    break;

  case 420: /* sub_search: match_text_expr  */
#line 3282 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8193 "parser.cpp"
    break;

  case 421: /* sub_search: match_tensor_expr  */
#line 3285 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8201 "parser.cpp"
    break;

  case 422: /* sub_search: match_sparse_expr  */
#line 3288 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8209 "parser.cpp"
    break;

  case 423: /* sub_search: query_expr  */
#line 3291 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8217 "parser.cpp"
    break;

  case 424: /* sub_search: fusion_expr  */
#line 3294 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8225 "parser.cpp"
    break;

  case 425: /* sub_search_array: sub_search  */
#line 3298 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 8234 "parser.cpp"
    break;

  case 426: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3302 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 8243 "parser.cpp"
    break;

  case 427: /* function_expr: IDENTIFIER '(' ')'  */
#line 3307 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 8256 "parser.cpp"
    break;

  case 428: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3315 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 8269 "parser.cpp"
    break;

  case 429: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 3323 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 8283 "parser.cpp"
    break;

  case 430: /* function_expr: YEAR '(' expr ')'  */
#line 3332 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "year";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8295 "parser.cpp"
    break;

  case 431: /* function_expr: MONTH '(' expr ')'  */
#line 3339 "parser.y"
                     {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "month";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8307 "parser.cpp"
    break;

  case 432: /* function_expr: DAY '(' expr ')'  */
#line 3346 "parser.y"
                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "day";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8319 "parser.cpp"
    break;

  case 433: /* function_expr: HOUR '(' expr ')'  */
#line 3353 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "hour";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8331 "parser.cpp"
    break;

  case 434: /* function_expr: MINUTE '(' expr ')'  */
#line 3360 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "minute";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8343 "parser.cpp"
    break;

  case 435: /* function_expr: SECOND '(' expr ')'  */
#line 3367 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "second";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8355 "parser.cpp"
    break;

  case 436: /* function_expr: operand IS NOT NULLABLE  */
#line 3374 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8367 "parser.cpp"
    break;

  case 437: /* function_expr: operand IS NULLABLE  */
#line 3381 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8379 "parser.cpp"
    break;

  case 438: /* function_expr: NOT operand  */
#line 3388 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8391 "parser.cpp"
    break;

  case 439: /* function_expr: '-' operand  */
#line 3395 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8403 "parser.cpp"
    break;

  case 440: /* function_expr: '+' operand  */
#line 3402 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8415 "parser.cpp"
    break;

  case 441: /* function_expr: operand '-' operand  */
#line 3409 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8428 "parser.cpp"
    break;

  case 442: /* function_expr: operand '+' operand  */
#line 3417 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8441 "parser.cpp"
    break;

  case 443: /* function_expr: operand '*' operand  */
#line 3425 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8454 "parser.cpp"
    break;

  case 444: /* function_expr: operand '/' operand  */
#line 3433 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8467 "parser.cpp"
    break;

  case 445: /* function_expr: operand '%' operand  */
#line 3441 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8480 "parser.cpp"
    break;

  case 446: /* function_expr: operand '=' operand  */
#line 3449 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8493 "parser.cpp"
    break;

  case 447: /* function_expr: operand EQUAL operand  */
#line 3457 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8506 "parser.cpp"
    break;

  case 448: /* function_expr: operand NOT_EQ operand  */
#line 3465 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8519 "parser.cpp"
    break;

  case 449: /* function_expr: operand '<' operand  */
#line 3473 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8532 "parser.cpp"
    break;

  case 450: /* function_expr: operand '>' operand  */
#line 3481 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8545 "parser.cpp"
    break;

  case 451: /* function_expr: operand LESS_EQ operand  */
#line 3489 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8558 "parser.cpp"
    break;

  case 452: /* function_expr: operand GREATER_EQ operand  */
#line 3497 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8571 "parser.cpp"
    break;

  case 453: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3505 "parser.y"
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
#line 8606 "parser.cpp"
    break;

  case 454: /* function_expr: operand LIKE operand  */
#line 3535 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8619 "parser.cpp"
    break;

  case 455: /* function_expr: operand NOT LIKE operand  */
#line 3543 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8632 "parser.cpp"
    break;

  case 456: /* conjunction_expr: expr AND expr  */
#line 3552 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8645 "parser.cpp"
    break;

  case 457: /* conjunction_expr: expr OR expr  */
#line 3560 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8658 "parser.cpp"
    break;

  case 458: /* between_expr: operand BETWEEN operand AND operand  */
#line 3569 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 8670 "parser.cpp"
    break;

  case 459: /* in_expr: operand IN '(' expr_array ')'  */
#line 3577 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8681 "parser.cpp"
    break;

  case 460: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3583 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8692 "parser.cpp"
    break;

  case 461: /* case_expr: CASE expr case_check_array END  */
#line 3590 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8703 "parser.cpp"
    break;

  case 462: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3596 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8715 "parser.cpp"
    break;

  case 463: /* case_expr: CASE case_check_array END  */
#line 3603 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8725 "parser.cpp"
    break;

  case 464: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3608 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8736 "parser.cpp"
    break;

  case 465: /* case_check_array: WHEN expr THEN expr  */
#line 3615 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 8748 "parser.cpp"
    break;

  case 466: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3622 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 8760 "parser.cpp"
    break;

  case 467: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3630 "parser.y"
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
#line 8777 "parser.cpp"
    break;

  case 468: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3643 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8788 "parser.cpp"
    break;

  case 469: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3649 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8799 "parser.cpp"
    break;

  case 470: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3655 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8811 "parser.cpp"
    break;

  case 471: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3662 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8823 "parser.cpp"
    break;

  case 472: /* column_expr: IDENTIFIER  */
#line 3670 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8835 "parser.cpp"
    break;

  case 473: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3677 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8847 "parser.cpp"
    break;

  case 474: /* column_expr: '*'  */
#line 3684 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8857 "parser.cpp"
    break;

  case 475: /* column_expr: column_expr '.' '*'  */
#line 3689 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8871 "parser.cpp"
    break;

  case 476: /* constant_expr: STRING  */
#line 3699 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8881 "parser.cpp"
    break;

  case 477: /* constant_expr: TRUE  */
#line 3704 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8891 "parser.cpp"
    break;

  case 478: /* constant_expr: FALSE  */
#line 3709 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8901 "parser.cpp"
    break;

  case 479: /* constant_expr: DOUBLE_VALUE  */
#line 3714 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8911 "parser.cpp"
    break;

  case 480: /* constant_expr: LONG_VALUE  */
#line 3719 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8921 "parser.cpp"
    break;

  case 481: /* constant_expr: DATE STRING  */
#line 3724 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8931 "parser.cpp"
    break;

  case 482: /* constant_expr: TIME STRING  */
#line 3729 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8941 "parser.cpp"
    break;

  case 483: /* constant_expr: DATETIME STRING  */
#line 3734 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8951 "parser.cpp"
    break;

  case 484: /* constant_expr: TIMESTAMP STRING  */
#line 3739 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8961 "parser.cpp"
    break;

  case 485: /* constant_expr: INTERVAL interval_expr  */
#line 3744 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8969 "parser.cpp"
    break;

  case 486: /* constant_expr: interval_expr  */
#line 3747 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8977 "parser.cpp"
    break;

  case 487: /* constant_expr: common_array_expr  */
#line 3750 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8985 "parser.cpp"
    break;

  case 488: /* constant_expr: curly_brackets_expr  */
#line 3753 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8993 "parser.cpp"
    break;

  case 489: /* common_array_expr: array_expr  */
#line 3757 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9001 "parser.cpp"
    break;

  case 490: /* common_array_expr: subarray_array_expr  */
#line 3760 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9009 "parser.cpp"
    break;

  case 491: /* common_array_expr: sparse_array_expr  */
#line 3763 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9017 "parser.cpp"
    break;

  case 492: /* common_array_expr: empty_array_expr  */
#line 3766 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9025 "parser.cpp"
    break;

  case 493: /* common_sparse_array_expr: sparse_array_expr  */
#line 3770 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9033 "parser.cpp"
    break;

  case 494: /* common_sparse_array_expr: array_expr  */
#line 3773 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9041 "parser.cpp"
    break;

  case 495: /* common_sparse_array_expr: empty_array_expr  */
#line 3776 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9049 "parser.cpp"
    break;

  case 496: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3780 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9057 "parser.cpp"
    break;

  case 497: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3784 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 9067 "parser.cpp"
    break;

  case 498: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3789 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9076 "parser.cpp"
    break;

  case 499: /* sparse_array_expr: long_sparse_array_expr  */
#line 3794 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9084 "parser.cpp"
    break;

  case 500: /* sparse_array_expr: double_sparse_array_expr  */
#line 3797 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9092 "parser.cpp"
    break;

  case 501: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3801 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9100 "parser.cpp"
    break;

  case 502: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3805 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 9112 "parser.cpp"
    break;

  case 503: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3812 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9123 "parser.cpp"
    break;

  case 504: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3819 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9131 "parser.cpp"
    break;

  case 505: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3823 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 9143 "parser.cpp"
    break;

  case 506: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3830 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9154 "parser.cpp"
    break;

  case 507: /* empty_array_expr: '[' ']'  */
#line 3837 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 9162 "parser.cpp"
    break;

  case 508: /* curly_brackets_expr: unclosed_curly_brackets_expr '}'  */
#line 3841 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9170 "parser.cpp"
    break;

  case 509: /* curly_brackets_expr: '{' '}'  */
#line 3844 "parser.y"
          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
}
#line 9178 "parser.cpp"
    break;

  case 510: /* unclosed_curly_brackets_expr: '{' constant_expr  */
#line 3848 "parser.y"
                                                {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
    (yyval.const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
}
#line 9187 "parser.cpp"
    break;

  case 511: /* unclosed_curly_brackets_expr: unclosed_curly_brackets_expr ',' constant_expr  */
#line 3852 "parser.y"
                                                 {
    (yyvsp[-2].const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9196 "parser.cpp"
    break;

  case 512: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3857 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 9204 "parser.cpp"
    break;

  case 513: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3861 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 9212 "parser.cpp"
    break;

  case 514: /* array_expr: long_array_expr  */
#line 3865 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9220 "parser.cpp"
    break;

  case 515: /* array_expr: double_array_expr  */
#line 3868 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9228 "parser.cpp"
    break;

  case 516: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3872 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9236 "parser.cpp"
    break;

  case 517: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3876 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9246 "parser.cpp"
    break;

  case 518: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3881 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9255 "parser.cpp"
    break;

  case 519: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3886 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9263 "parser.cpp"
    break;

  case 520: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3890 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9273 "parser.cpp"
    break;

  case 521: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3895 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9282 "parser.cpp"
    break;

  case 522: /* interval_expr: LONG_VALUE SECONDS  */
#line 3900 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9293 "parser.cpp"
    break;

  case 523: /* interval_expr: LONG_VALUE SECOND  */
#line 3906 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9304 "parser.cpp"
    break;

  case 524: /* interval_expr: LONG_VALUE MINUTES  */
#line 3912 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9315 "parser.cpp"
    break;

  case 525: /* interval_expr: LONG_VALUE MINUTE  */
#line 3918 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9326 "parser.cpp"
    break;

  case 526: /* interval_expr: LONG_VALUE HOURS  */
#line 3924 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9337 "parser.cpp"
    break;

  case 527: /* interval_expr: LONG_VALUE HOUR  */
#line 3930 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9348 "parser.cpp"
    break;

  case 528: /* interval_expr: LONG_VALUE DAYS  */
#line 3936 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9359 "parser.cpp"
    break;

  case 529: /* interval_expr: LONG_VALUE DAY  */
#line 3942 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9370 "parser.cpp"
    break;

  case 530: /* interval_expr: LONG_VALUE MONTHS  */
#line 3948 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9381 "parser.cpp"
    break;

  case 531: /* interval_expr: LONG_VALUE MONTH  */
#line 3954 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9392 "parser.cpp"
    break;

  case 532: /* interval_expr: LONG_VALUE YEARS  */
#line 3960 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9403 "parser.cpp"
    break;

  case 533: /* interval_expr: LONG_VALUE YEAR  */
#line 3966 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9414 "parser.cpp"
    break;

  case 534: /* copy_option_list: copy_option  */
#line 3977 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 9423 "parser.cpp"
    break;

  case 535: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3981 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 9432 "parser.cpp"
    break;

  case 536: /* copy_option: FORMAT IDENTIFIER  */
#line 3986 "parser.y"
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
#line 9469 "parser.cpp"
    break;

  case 537: /* copy_option: DELIMITER STRING  */
#line 4018 "parser.y"
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
#line 9484 "parser.cpp"
    break;

  case 538: /* copy_option: HEADER  */
#line 4028 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 9494 "parser.cpp"
    break;

  case 539: /* copy_option: OFFSET LONG_VALUE  */
#line 4033 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 9504 "parser.cpp"
    break;

  case 540: /* copy_option: LIMIT LONG_VALUE  */
#line 4038 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 9514 "parser.cpp"
    break;

  case 541: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 4043 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 9524 "parser.cpp"
    break;

  case 542: /* file_path: STRING  */
#line 4049 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 9532 "parser.cpp"
    break;

  case 543: /* if_exists: IF EXISTS  */
#line 4053 "parser.y"
                     { (yyval.bool_value) = true; }
#line 9538 "parser.cpp"
    break;

  case 544: /* if_exists: %empty  */
#line 4054 "parser.y"
  { (yyval.bool_value) = false; }
#line 9544 "parser.cpp"
    break;

  case 545: /* if_not_exists: IF NOT EXISTS  */
#line 4056 "parser.y"
                              { (yyval.bool_value) = true; }
#line 9550 "parser.cpp"
    break;

  case 546: /* if_not_exists: %empty  */
#line 4057 "parser.y"
  { (yyval.bool_value) = false; }
#line 9556 "parser.cpp"
    break;

  case 549: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 4072 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9569 "parser.cpp"
    break;

  case 550: /* if_not_exists_info: %empty  */
#line 4080 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 9577 "parser.cpp"
    break;

  case 551: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 4084 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9585 "parser.cpp"
    break;

  case 552: /* with_index_param_list: %empty  */
#line 4087 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 9593 "parser.cpp"
    break;

  case 553: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 4091 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9601 "parser.cpp"
    break;

  case 554: /* optional_table_properties_list: %empty  */
#line 4094 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 9609 "parser.cpp"
    break;

  case 555: /* index_param_list: index_param  */
#line 4098 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 9618 "parser.cpp"
    break;

  case 556: /* index_param_list: index_param_list ',' index_param  */
#line 4102 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 9627 "parser.cpp"
    break;

  case 557: /* index_param: IDENTIFIER  */
#line 4107 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9638 "parser.cpp"
    break;

  case 558: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 4113 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9653 "parser.cpp"
    break;

  case 559: /* index_param: IDENTIFIER '=' STRING  */
#line 4123 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9668 "parser.cpp"
    break;

  case 560: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 4133 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 9681 "parser.cpp"
    break;

  case 561: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 4141 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 9694 "parser.cpp"
    break;

  case 562: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 4152 "parser.y"
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
#line 9731 "parser.cpp"
    break;

  case 563: /* index_info: '(' IDENTIFIER ')'  */
#line 4184 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 9743 "parser.cpp"
    break;


#line 9747 "parser.cpp"

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

#line 4192 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

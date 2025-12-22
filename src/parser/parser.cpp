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
  YYSYMBOL_CHUNKS = 50,                    /* CHUNKS  */
  YYSYMBOL_SYSTEM = 51,                    /* SYSTEM  */
  YYSYMBOL_GROUP = 52,                     /* GROUP  */
  YYSYMBOL_BY = 53,                        /* BY  */
  YYSYMBOL_HAVING = 54,                    /* HAVING  */
  YYSYMBOL_AS = 55,                        /* AS  */
  YYSYMBOL_NATURAL = 56,                   /* NATURAL  */
  YYSYMBOL_JOIN = 57,                      /* JOIN  */
  YYSYMBOL_LEFT = 58,                      /* LEFT  */
  YYSYMBOL_RIGHT = 59,                     /* RIGHT  */
  YYSYMBOL_OUTER = 60,                     /* OUTER  */
  YYSYMBOL_FULL = 61,                      /* FULL  */
  YYSYMBOL_ON = 62,                        /* ON  */
  YYSYMBOL_INNER = 63,                     /* INNER  */
  YYSYMBOL_CROSS = 64,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 65,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 66,                     /* WHERE  */
  YYSYMBOL_ORDER = 67,                     /* ORDER  */
  YYSYMBOL_LIMIT = 68,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 69,                    /* OFFSET  */
  YYSYMBOL_ASC = 70,                       /* ASC  */
  YYSYMBOL_DESC = 71,                      /* DESC  */
  YYSYMBOL_IF = 72,                        /* IF  */
  YYSYMBOL_NOT = 73,                       /* NOT  */
  YYSYMBOL_EXISTS = 74,                    /* EXISTS  */
  YYSYMBOL_IN = 75,                        /* IN  */
  YYSYMBOL_FROM = 76,                      /* FROM  */
  YYSYMBOL_TO = 77,                        /* TO  */
  YYSYMBOL_WITH = 78,                      /* WITH  */
  YYSYMBOL_DELIMITER = 79,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 80,                    /* FORMAT  */
  YYSYMBOL_HEADER = 81,                    /* HEADER  */
  YYSYMBOL_HIGHLIGHT = 82,                 /* HIGHLIGHT  */
  YYSYMBOL_CAST = 83,                      /* CAST  */
  YYSYMBOL_END = 84,                       /* END  */
  YYSYMBOL_CASE = 85,                      /* CASE  */
  YYSYMBOL_ELSE = 86,                      /* ELSE  */
  YYSYMBOL_THEN = 87,                      /* THEN  */
  YYSYMBOL_WHEN = 88,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 89,                   /* BOOLEAN  */
  YYSYMBOL_JSON = 90,                      /* JSON  */
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
  YYSYMBOL_ARRAY = 119,                    /* ARRAY  */
  YYSYMBOL_TUPLE = 120,                    /* TUPLE  */
  YYSYMBOL_EMBEDDING = 121,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 122,                   /* VECTOR  */
  YYSYMBOL_MULTIVECTOR = 123,              /* MULTIVECTOR  */
  YYSYMBOL_TENSOR = 124,                   /* TENSOR  */
  YYSYMBOL_SPARSE = 125,                   /* SPARSE  */
  YYSYMBOL_TENSORARRAY = 126,              /* TENSORARRAY  */
  YYSYMBOL_BIT = 127,                      /* BIT  */
  YYSYMBOL_TEXT = 128,                     /* TEXT  */
  YYSYMBOL_PRIMARY = 129,                  /* PRIMARY  */
  YYSYMBOL_KEY = 130,                      /* KEY  */
  YYSYMBOL_UNIQUE = 131,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 132,                 /* NULLABLE  */
  YYSYMBOL_IS = 133,                       /* IS  */
  YYSYMBOL_DEFAULT = 134,                  /* DEFAULT  */
  YYSYMBOL_COMMENT = 135,                  /* COMMENT  */
  YYSYMBOL_IGNORE = 136,                   /* IGNORE  */
  YYSYMBOL_TRUE = 137,                     /* TRUE  */
  YYSYMBOL_FALSE = 138,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 139,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 140,                   /* SECOND  */
  YYSYMBOL_SECONDS = 141,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 142,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 143,                  /* MINUTES  */
  YYSYMBOL_HOUR = 144,                     /* HOUR  */
  YYSYMBOL_HOURS = 145,                    /* HOURS  */
  YYSYMBOL_DAY = 146,                      /* DAY  */
  YYSYMBOL_DAYS = 147,                     /* DAYS  */
  YYSYMBOL_MONTH = 148,                    /* MONTH  */
  YYSYMBOL_MONTHS = 149,                   /* MONTHS  */
  YYSYMBOL_YEAR = 150,                     /* YEAR  */
  YYSYMBOL_YEARS = 151,                    /* YEARS  */
  YYSYMBOL_EQUAL = 152,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 153,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 154,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 155,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 156,                  /* BETWEEN  */
  YYSYMBOL_AND = 157,                      /* AND  */
  YYSYMBOL_OR = 158,                       /* OR  */
  YYSYMBOL_EXTRACT = 159,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 160,                     /* LIKE  */
  YYSYMBOL_DATA = 161,                     /* DATA  */
  YYSYMBOL_LOG = 162,                      /* LOG  */
  YYSYMBOL_BUFFER = 163,                   /* BUFFER  */
  YYSYMBOL_TRANSACTIONS = 164,             /* TRANSACTIONS  */
  YYSYMBOL_TRANSACTION = 165,              /* TRANSACTION  */
  YYSYMBOL_MEMINDEX = 166,                 /* MEMINDEX  */
  YYSYMBOL_USING = 167,                    /* USING  */
  YYSYMBOL_SESSION = 168,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 169,                   /* GLOBAL  */
  YYSYMBOL_OFF = 170,                      /* OFF  */
  YYSYMBOL_EXPORT = 171,                   /* EXPORT  */
  YYSYMBOL_CONFIGS = 172,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 173,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 174,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 175,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 176,                 /* VARIABLE  */
  YYSYMBOL_LOGS = 177,                     /* LOGS  */
  YYSYMBOL_CATALOGS = 178,                 /* CATALOGS  */
  YYSYMBOL_CATALOG = 179,                  /* CATALOG  */
  YYSYMBOL_SEARCH = 180,                   /* SEARCH  */
  YYSYMBOL_MATCH = 181,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 182,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 183,                    /* QUERY  */
  YYSYMBOL_QUERIES = 184,                  /* QUERIES  */
  YYSYMBOL_FUSION = 185,                   /* FUSION  */
  YYSYMBOL_ROWLIMIT = 186,                 /* ROWLIMIT  */
  YYSYMBOL_ADMIN = 187,                    /* ADMIN  */
  YYSYMBOL_LEADER = 188,                   /* LEADER  */
  YYSYMBOL_FOLLOWER = 189,                 /* FOLLOWER  */
  YYSYMBOL_LEARNER = 190,                  /* LEARNER  */
  YYSYMBOL_CONNECT = 191,                  /* CONNECT  */
  YYSYMBOL_STANDALONE = 192,               /* STANDALONE  */
  YYSYMBOL_NODES = 193,                    /* NODES  */
  YYSYMBOL_NODE = 194,                     /* NODE  */
  YYSYMBOL_REMOVE = 195,                   /* REMOVE  */
  YYSYMBOL_SNAPSHOT = 196,                 /* SNAPSHOT  */
  YYSYMBOL_SNAPSHOTS = 197,                /* SNAPSHOTS  */
  YYSYMBOL_RECOVER = 198,                  /* RECOVER  */
  YYSYMBOL_RESTORE = 199,                  /* RESTORE  */
  YYSYMBOL_CACHES = 200,                   /* CACHES  */
  YYSYMBOL_CACHE = 201,                    /* CACHE  */
  YYSYMBOL_PERSISTENCE = 202,              /* PERSISTENCE  */
  YYSYMBOL_OBJECT = 203,                   /* OBJECT  */
  YYSYMBOL_OBJECTS = 204,                  /* OBJECTS  */
  YYSYMBOL_FILES = 205,                    /* FILES  */
  YYSYMBOL_MEMORY = 206,                   /* MEMORY  */
  YYSYMBOL_ALLOCATION = 207,               /* ALLOCATION  */
  YYSYMBOL_HISTORY = 208,                  /* HISTORY  */
  YYSYMBOL_CHECK = 209,                    /* CHECK  */
  YYSYMBOL_CLEAN = 210,                    /* CLEAN  */
  YYSYMBOL_CHECKPOINT = 211,               /* CHECKPOINT  */
  YYSYMBOL_IMPORT = 212,                   /* IMPORT  */
  YYSYMBOL_PARSE_JSON = 213,               /* PARSE_JSON  */
  YYSYMBOL_NUMBER = 214,                   /* NUMBER  */
  YYSYMBOL_215_ = 215,                     /* '='  */
  YYSYMBOL_216_ = 216,                     /* '<'  */
  YYSYMBOL_217_ = 217,                     /* '>'  */
  YYSYMBOL_218_ = 218,                     /* '+'  */
  YYSYMBOL_219_ = 219,                     /* '-'  */
  YYSYMBOL_220_ = 220,                     /* '*'  */
  YYSYMBOL_221_ = 221,                     /* '/'  */
  YYSYMBOL_222_ = 222,                     /* '%'  */
  YYSYMBOL_223_ = 223,                     /* ';'  */
  YYSYMBOL_224_ = 224,                     /* '('  */
  YYSYMBOL_225_ = 225,                     /* ')'  */
  YYSYMBOL_226_ = 226,                     /* ','  */
  YYSYMBOL_227_ = 227,                     /* '.'  */
  YYSYMBOL_228_ = 228,                     /* ']'  */
  YYSYMBOL_229_ = 229,                     /* '['  */
  YYSYMBOL_230_ = 230,                     /* '}'  */
  YYSYMBOL_231_ = 231,                     /* '{'  */
  YYSYMBOL_232_ = 232,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 233,                 /* $accept  */
  YYSYMBOL_input_pattern = 234,            /* input_pattern  */
  YYSYMBOL_statement_list = 235,           /* statement_list  */
  YYSYMBOL_statement = 236,                /* statement  */
  YYSYMBOL_explainable_statement = 237,    /* explainable_statement  */
  YYSYMBOL_create_statement = 238,         /* create_statement  */
  YYSYMBOL_table_element_array = 239,      /* table_element_array  */
  YYSYMBOL_column_def_array = 240,         /* column_def_array  */
  YYSYMBOL_table_element = 241,            /* table_element  */
  YYSYMBOL_table_column = 242,             /* table_column  */
  YYSYMBOL_column_type_array = 243,        /* column_type_array  */
  YYSYMBOL_column_type = 244,              /* column_type  */
  YYSYMBOL_column_constraints = 245,       /* column_constraints  */
  YYSYMBOL_column_constraint = 246,        /* column_constraint  */
  YYSYMBOL_default_expr = 247,             /* default_expr  */
  YYSYMBOL_table_constraint = 248,         /* table_constraint  */
  YYSYMBOL_identifier_array = 249,         /* identifier_array  */
  YYSYMBOL_delete_statement = 250,         /* delete_statement  */
  YYSYMBOL_insert_statement = 251,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 252, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 253,        /* explain_statement  */
  YYSYMBOL_update_statement = 254,         /* update_statement  */
  YYSYMBOL_update_expr_array = 255,        /* update_expr_array  */
  YYSYMBOL_update_expr = 256,              /* update_expr  */
  YYSYMBOL_drop_statement = 257,           /* drop_statement  */
  YYSYMBOL_copy_statement = 258,           /* copy_statement  */
  YYSYMBOL_select_statement = 259,         /* select_statement  */
  YYSYMBOL_select_with_paren = 260,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 261,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 262, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 263, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 264, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 265,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 266,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 267,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 268,            /* order_by_type  */
  YYSYMBOL_limit_expr = 269,               /* limit_expr  */
  YYSYMBOL_offset_expr = 270,              /* offset_expr  */
  YYSYMBOL_distinct = 271,                 /* distinct  */
  YYSYMBOL_highlight_clause = 272,         /* highlight_clause  */
  YYSYMBOL_from_clause = 273,              /* from_clause  */
  YYSYMBOL_search_clause = 274,            /* search_clause  */
  YYSYMBOL_optional_search_filter_expr = 275, /* optional_search_filter_expr  */
  YYSYMBOL_where_clause = 276,             /* where_clause  */
  YYSYMBOL_having_clause = 277,            /* having_clause  */
  YYSYMBOL_group_by_clause = 278,          /* group_by_clause  */
  YYSYMBOL_set_operator = 279,             /* set_operator  */
  YYSYMBOL_table_reference = 280,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 281,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 282,     /* table_reference_name  */
  YYSYMBOL_table_name = 283,               /* table_name  */
  YYSYMBOL_table_alias = 284,              /* table_alias  */
  YYSYMBOL_with_clause = 285,              /* with_clause  */
  YYSYMBOL_with_expr_list = 286,           /* with_expr_list  */
  YYSYMBOL_with_expr = 287,                /* with_expr  */
  YYSYMBOL_join_clause = 288,              /* join_clause  */
  YYSYMBOL_join_type = 289,                /* join_type  */
  YYSYMBOL_show_statement = 290,           /* show_statement  */
  YYSYMBOL_flush_statement = 291,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 292,       /* optimize_statement  */
  YYSYMBOL_command_statement = 293,        /* command_statement  */
  YYSYMBOL_compact_statement = 294,        /* compact_statement  */
  YYSYMBOL_admin_statement = 295,          /* admin_statement  */
  YYSYMBOL_alter_statement = 296,          /* alter_statement  */
  YYSYMBOL_check_statement = 297,          /* check_statement  */
  YYSYMBOL_expr_array = 298,               /* expr_array  */
  YYSYMBOL_insert_row_list = 299,          /* insert_row_list  */
  YYSYMBOL_expr_alias = 300,               /* expr_alias  */
  YYSYMBOL_expr = 301,                     /* expr  */
  YYSYMBOL_operand = 302,                  /* operand  */
  YYSYMBOL_match_tensor_expr = 303,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 304,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 305,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 306,          /* match_text_expr  */
  YYSYMBOL_query_expr = 307,               /* query_expr  */
  YYSYMBOL_fusion_expr = 308,              /* fusion_expr  */
  YYSYMBOL_sub_search = 309,               /* sub_search  */
  YYSYMBOL_sub_search_array = 310,         /* sub_search_array  */
  YYSYMBOL_function_expr = 311,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 312,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 313,             /* between_expr  */
  YYSYMBOL_in_expr = 314,                  /* in_expr  */
  YYSYMBOL_case_expr = 315,                /* case_expr  */
  YYSYMBOL_case_check_array = 316,         /* case_check_array  */
  YYSYMBOL_cast_expr = 317,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 318,            /* subquery_expr  */
  YYSYMBOL_column_expr = 319,              /* column_expr  */
  YYSYMBOL_constant_expr = 320,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 321,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 322, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 323,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 324, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 325,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 326,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 327, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 328, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 329, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 330,         /* empty_array_expr  */
  YYSYMBOL_curly_brackets_expr = 331,      /* curly_brackets_expr  */
  YYSYMBOL_unclosed_curly_brackets_expr = 332, /* unclosed_curly_brackets_expr  */
  YYSYMBOL_int_sparse_ele = 333,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 334,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 335,               /* array_expr  */
  YYSYMBOL_long_array_expr = 336,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 337, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 338,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 339, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 340,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 341,         /* copy_option_list  */
  YYSYMBOL_copy_option = 342,              /* copy_option  */
  YYSYMBOL_file_path = 343,                /* file_path  */
  YYSYMBOL_if_exists = 344,                /* if_exists  */
  YYSYMBOL_if_not_exists = 345,            /* if_not_exists  */
  YYSYMBOL_semicolon = 346,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 347,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 348,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 349, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 350,         /* index_param_list  */
  YYSYMBOL_index_param = 351,              /* index_param  */
  YYSYMBOL_index_info = 352                /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 98 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 479 "parser.cpp"

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
#define YYFINAL  139
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1595

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  233
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  120
/* YYNRULES -- Number of rules.  */
#define YYNRULES  568
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1274

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   469


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
       2,     2,     2,     2,     2,     2,     2,   222,     2,     2,
     224,   225,   220,   218,   226,   219,   227,   221,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   232,   223,
     216,   215,   217,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   229,     2,   228,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   231,     2,   230,     2,     2,     2,     2,
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
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   516,   516,   520,   526,   533,   534,   535,   536,   537,
     538,   539,   540,   541,   542,   543,   544,   545,   546,   547,
     548,   550,   551,   552,   553,   554,   555,   556,   557,   558,
     559,   560,   561,   568,   585,   602,   618,   647,   662,   694,
     712,   730,   758,   789,   793,   798,   802,   808,   811,   818,
     838,   860,   884,   910,   914,   920,   921,   922,   923,   924,
     925,   926,   927,   928,   929,   930,   931,   932,   933,   934,
     935,   936,   937,   938,   939,   940,   941,   944,   946,   947,
     948,   949,   952,   953,   954,   955,   956,   957,   958,   959,
     960,   961,   962,   963,   964,   965,   966,   967,   968,   969,
     970,   971,   972,   973,   974,   975,   976,   977,   978,   979,
     980,   981,   982,   983,   984,   985,   986,   987,   988,   989,
     990,   991,   992,   993,   994,   995,   996,   997,   998,   999,
    1000,  1001,  1002,  1003,  1004,  1005,  1006,  1007,  1008,  1009,
    1010,  1011,  1012,  1013,  1014,  1015,  1016,  1017,  1018,  1022,
    1045,  1049,  1059,  1062,  1065,  1068,  1072,  1075,  1080,  1085,
    1092,  1098,  1108,  1124,  1162,  1178,  1181,  1188,  1205,  1214,
    1227,  1231,  1236,  1249,  1262,  1277,  1292,  1307,  1330,  1383,
    1438,  1489,  1492,  1495,  1504,  1514,  1517,  1521,  1526,  1553,
    1556,  1561,  1578,  1581,  1585,  1589,  1594,  1600,  1603,  1606,
    1610,  1613,  1616,  1619,  1622,  1625,  1629,  1632,  1636,  1639,
    1643,  1648,  1652,  1655,  1659,  1662,  1666,  1669,  1673,  1676,
    1680,  1683,  1686,  1689,  1697,  1700,  1715,  1715,  1717,  1731,
    1740,  1745,  1754,  1759,  1764,  1770,  1777,  1780,  1784,  1787,
    1792,  1804,  1811,  1825,  1828,  1831,  1834,  1837,  1840,  1843,
    1850,  1854,  1858,  1862,  1866,  1873,  1877,  1881,  1885,  1889,
    1894,  1898,  1903,  1907,  1911,  1915,  1921,  1927,  1934,  1945,
    1956,  1967,  1979,  1991,  2004,  2018,  2029,  2044,  2061,  2078,
    2096,  2100,  2104,  2111,  2117,  2121,  2125,  2131,  2135,  2139,
    2143,  2150,  2154,  2161,  2165,  2169,  2173,  2178,  2182,  2187,
    2192,  2196,  2201,  2205,  2209,  2214,  2223,  2227,  2231,  2235,
    2243,  2257,  2263,  2268,  2274,  2280,  2288,  2294,  2300,  2306,
    2312,  2320,  2326,  2332,  2338,  2344,  2352,  2358,  2364,  2372,
    2380,  2386,  2392,  2398,  2404,  2410,  2414,  2426,  2439,  2445,
    2452,  2460,  2469,  2479,  2489,  2500,  2511,  2523,  2535,  2545,
    2556,  2568,  2581,  2585,  2590,  2595,  2601,  2605,  2609,  2616,
    2620,  2624,  2631,  2637,  2645,  2651,  2655,  2661,  2665,  2671,
    2676,  2681,  2688,  2697,  2707,  2717,  2729,  2740,  2759,  2763,
    2779,  2783,  2788,  2798,  2820,  2826,  2830,  2831,  2832,  2833,
    2834,  2836,  2839,  2845,  2848,  2849,  2850,  2851,  2852,  2853,
    2854,  2855,  2856,  2857,  2861,  2877,  2895,  2913,  2971,  3021,
    3075,  3133,  3158,  3181,  3202,  3223,  3232,  3244,  3251,  3261,
    3267,  3279,  3282,  3285,  3288,  3291,  3294,  3298,  3302,  3307,
    3315,  3323,  3332,  3339,  3346,  3353,  3360,  3367,  3374,  3381,
    3388,  3395,  3402,  3409,  3417,  3425,  3433,  3441,  3449,  3457,
    3465,  3473,  3481,  3489,  3497,  3505,  3535,  3543,  3552,  3560,
    3569,  3577,  3583,  3590,  3596,  3603,  3608,  3615,  3622,  3630,
    3642,  3656,  3662,  3668,  3675,  3683,  3690,  3697,  3702,  3712,
    3717,  3722,  3727,  3732,  3737,  3742,  3747,  3752,  3757,  3762,
    3765,  3768,  3771,  3775,  3778,  3781,  3784,  3788,  3791,  3794,
    3798,  3802,  3807,  3812,  3815,  3819,  3823,  3830,  3837,  3841,
    3848,  3855,  3859,  3862,  3866,  3870,  3875,  3879,  3883,  3886,
    3890,  3894,  3899,  3904,  3908,  3913,  3918,  3924,  3930,  3936,
    3942,  3948,  3954,  3960,  3966,  3972,  3978,  3984,  3995,  3999,
    4004,  4009,  4038,  4048,  4053,  4058,  4063,  4069,  4073,  4074,
    4076,  4077,  4079,  4080,  4092,  4100,  4104,  4107,  4111,  4114,
    4118,  4122,  4127,  4133,  4143,  4153,  4161,  4172,  4225
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
  "COLUMNS", "INDEXES", "CHUNK", "CHUNKS", "SYSTEM", "GROUP", "BY",
  "HAVING", "AS", "NATURAL", "JOIN", "LEFT", "RIGHT", "OUTER", "FULL",
  "ON", "INNER", "CROSS", "DISTINCT", "WHERE", "ORDER", "LIMIT", "OFFSET",
  "ASC", "DESC", "IF", "NOT", "EXISTS", "IN", "FROM", "TO", "WITH",
  "DELIMITER", "FORMAT", "HEADER", "HIGHLIGHT", "CAST", "END", "CASE",
  "ELSE", "THEN", "WHEN", "BOOLEAN", "JSON", "INTEGER", "INT", "TINYINT",
  "SMALLINT", "BIGINT", "HUGEINT", "VARCHAR", "FLOAT", "DOUBLE", "REAL",
  "DECIMAL", "DATE", "TIME", "DATETIME", "FLOAT16", "BFLOAT16", "UNSIGNED",
  "TIMESTAMP", "UUID", "POINT", "LINE", "LSEG", "BOX", "PATH", "POLYGON",
  "CIRCLE", "BLOB", "BITMAP", "ARRAY", "TUPLE", "EMBEDDING", "VECTOR",
  "MULTIVECTOR", "TENSOR", "SPARSE", "TENSORARRAY", "BIT", "TEXT",
  "PRIMARY", "KEY", "UNIQUE", "NULLABLE", "IS", "DEFAULT", "COMMENT",
  "IGNORE", "TRUE", "FALSE", "INTERVAL", "SECOND", "SECONDS", "MINUTE",
  "MINUTES", "HOUR", "HOURS", "DAY", "DAYS", "MONTH", "MONTHS", "YEAR",
  "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ", "GREATER_EQ", "BETWEEN", "AND",
  "OR", "EXTRACT", "LIKE", "DATA", "LOG", "BUFFER", "TRANSACTIONS",
  "TRANSACTION", "MEMINDEX", "USING", "SESSION", "GLOBAL", "OFF", "EXPORT",
  "CONFIGS", "CONFIG", "PROFILES", "VARIABLES", "VARIABLE", "LOGS",
  "CATALOGS", "CATALOG", "SEARCH", "MATCH", "MAXSIM", "QUERY", "QUERIES",
  "FUSION", "ROWLIMIT", "ADMIN", "LEADER", "FOLLOWER", "LEARNER",
  "CONNECT", "STANDALONE", "NODES", "NODE", "REMOVE", "SNAPSHOT",
  "SNAPSHOTS", "RECOVER", "RESTORE", "CACHES", "CACHE", "PERSISTENCE",
  "OBJECT", "OBJECTS", "FILES", "MEMORY", "ALLOCATION", "HISTORY", "CHECK",
  "CLEAN", "CHECKPOINT", "IMPORT", "PARSE_JSON", "NUMBER", "'='", "'<'",
  "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "';'", "'('", "')'", "','",
  "'.'", "']'", "'['", "'}'", "'{'", "':'", "$accept", "input_pattern",
  "statement_list", "statement", "explainable_statement",
  "create_statement", "table_element_array", "column_def_array",
  "table_element", "table_column", "column_type_array", "column_type",
  "column_constraints", "column_constraint", "default_expr",
  "table_constraint", "identifier_array", "delete_statement",
  "insert_statement", "optional_identifier_array", "explain_statement",
  "update_statement", "update_expr_array", "update_expr", "drop_statement",
  "copy_statement", "select_statement", "select_with_paren",
  "select_without_paren", "select_clause_with_modifier",
  "select_clause_without_modifier_paren", "select_clause_without_modifier",
  "order_by_clause", "order_by_expr_list", "order_by_expr",
  "order_by_type", "limit_expr", "offset_expr", "distinct",
  "highlight_clause", "from_clause", "search_clause",
  "optional_search_filter_expr", "where_clause", "having_clause",
  "group_by_clause", "set_operator", "table_reference",
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

#define YYPACT_NINF (-786)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-556)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     367,    72,    36,    75,    86,   102,    86,   376,  1021,  1112,
     270,   153,   154,   350,   279,    86,   285,   188,    69,   293,
      49,   151,   -33,   375,   218,  -786,  -786,  -786,  -786,  -786,
    -786,  -786,  -786,   542,  -786,  -786,   439,  -786,  -786,  -786,
    -786,  -786,  -786,  -786,  -786,   361,   361,   361,   361,   208,
     488,    86,   416,   416,   416,   416,   416,   505,   239,   500,
      86,   -20,   522,   531,   536,  1080,  -786,  -786,  -786,  -786,
    -786,  -786,  -786,   542,  -786,  -786,  -786,  -786,  -786,   296,
     473,   475,   553,    86,  -786,  -786,  -786,  -786,  -786,    38,
    -786,   -81,   144,  -786,   555,  -786,  -786,    56,   568,  -786,
     582,  -786,  -786,  -786,   425,   242,   159,   229,  -786,   503,
      86,    86,   587,  -786,  -786,  -786,  -786,  -786,  -786,   516,
     366,  -786,   594,   406,   410,   394,   330,   423,   617,   441,
     567,   467,   482,   484,    86,  -786,  -786,   457,   459,  -786,
     323,  -786,   691,  -786,  -786,    12,   630,  -786,   652,   667,
     762,    86,    86,    86,   764,   709,   722,   573,   725,   789,
      86,    86,    86,   798,  -786,   800,   802,   750,   813,   813,
     769,    61,    87,    93,  -786,   595,   686,   689,  -786,   442,
    -786,  -786,  -786,   816,  -786,   819,  -786,  -786,   813,  -786,
    -786,   820,  -786,  -786,  -786,  -786,  -786,   693,  -786,   698,
      86,   494,  -786,   761,   608,   285,   813,  -786,   829,  -786,
     668,  -786,   830,  -786,  -786,   835,  -786,   834,  -786,   840,
     842,  -786,   843,   791,   844,   653,   847,   848,   850,  -786,
    -786,  -786,  -786,  -786,    12,  -786,  -786,  -786,   769,   803,
     787,   783,   723,   -21,  -786,   573,  -786,    86,   407,   861,
     253,  -786,  -786,  -786,  -786,  -786,   804,  -786,   650,   -24,
    -786,   769,  -786,  -786,   790,   801,   643,  -786,  -786,  1123,
     857,   654,   657,   537,   878,   880,   882,   884,   893,  -786,
    -786,   877,   675,   676,   677,   678,   679,   680,   681,   340,
     688,   690,   692,   919,   919,  -786,   656,    10,    13,   275,
    -786,     0,   561,  -786,  -786,  -786,  -786,  -786,  -786,  -786,
    -786,  -786,  -786,  -786,  -786,  -786,   671,  -786,  -786,  -786,
     -54,  -786,  -786,    80,  -786,   174,  -786,  -786,  -151,  -786,
    -786,   195,  -786,   227,  -786,  -786,  -786,  -786,  -786,  -786,
    -786,  -786,  -786,  -786,  -786,  -786,  -786,  -786,  -786,  -786,
    -786,  -786,  -786,   907,   912,  -786,  -786,  -786,  -786,  -786,
    -786,  -786,  -786,  -786,   849,   874,   875,   852,    86,   439,
    -786,  -786,  -786,   922,    39,  -786,   926,  -786,  -786,   856,
     389,  -786,   930,  -786,  -786,  -786,   710,   711,   -32,   769,
     769,   868,  -786,   934,   -33,    33,   886,   715,   940,   941,
    -786,  -786,   342,   721,  -786,    86,   769,   802,  -786,   455,
     724,   727,   413,  -786,  -786,  -786,  -786,  -786,  -786,  -786,
    -786,  -786,  -786,  -786,  -786,   919,   729,   968,   871,   769,
     769,   -26,   509,  -786,  -786,  -786,  -786,  -786,  1123,  -786,
     769,   769,   769,   769,   769,   769,   942,   731,   732,   733,
     734,   958,   959,   769,   451,   451,   739,    53,  -786,   735,
    -786,  -786,  -786,  -786,  -786,  -786,   889,   769,   965,   769,
     769,    -7,   745,     4,   919,   919,   919,   919,   919,   919,
     919,   919,   919,   919,   919,   919,   919,   919,    21,   746,
    -786,   970,  -786,   973,  -786,    22,  -786,   974,  -786,   976,
    -786,   948,   579,   759,  -786,   767,   784,  1007,  -786,   786,
    -786,  1006,  -786,   257,  1009,   851,   853,  -786,  -786,  -786,
     769,   938,   793,  -786,    48,   455,   769,  -786,  -786,    31,
    1113,   885,   805,   430,  -786,  -786,  -786,   -33,  1014,   890,
    -786,  -786,  -786,  1023,   769,   810,  -786,   455,  -786,   206,
     206,   769,  -786,   463,   871,   883,   814,    19,   -18,   515,
    -786,   769,   769,   104,   138,   147,   160,   164,   190,   969,
     769,    34,   769,  1042,   822,   465,   255,  -786,  -786,   687,
     813,  -786,  -786,  -786,   895,   831,   919,   656,   921,  -786,
     139,   139,   210,   210,   674,   139,   139,   210,   210,   451,
     451,  -786,  -786,  -786,  -786,  -786,  -786,   828,  -786,   832,
    -786,  -786,  -786,  -786,  1048,  1056,  -786,  1063,   861,  1065,
    -786,  -786,  -786,  1066,  -786,  -786,  1067,  1069,   854,    26,
     894,   769,  -786,  -786,  -786,   455,  1075,  -786,  -786,  -786,
    -786,  -786,  -786,  -786,  -786,  -786,  -786,  -786,  -786,   858,
    -786,  -786,  -786,  -786,  -786,  -786,  -786,  -786,  -786,  -786,
    -786,  -786,   859,   860,   873,   881,   887,   888,   892,   900,
     237,   901,   861,  1054,    33,   542,   870,  1094,  -786,   481,
     902,  1101,  1107,  1110,    77,  -786,  1111,   483,  -786,   485,
     498,  -786,   904,  -786,  1113,   769,  -786,   769,   -23,   -14,
    -786,  -786,  -786,  -786,  -786,  -786,   919,   -56,  -786,   499,
      71,   905,   203,   908,  -786,  1114,  -786,  -786,  -786,  1041,
     656,   139,   909,   502,  -786,   919,  1121,  1125,   682,  1090,
     925,   510,  -786,   512,   517,  -786,  1135,  -786,  -786,   -33,
     920,   641,  -786,    83,  -786,   424,   750,  -786,  -786,  1143,
    1113,  1113,   559,   879,  1068,  1155,  1234,  1251,  1019,  1024,
    -786,  -786,   325,  -786,  1022,   861,   519,   931,  1029,  -786,
     990,  -786,  -786,   769,  -786,  -786,  -786,  -786,  -786,  -786,
    -786,   206,  -786,  -786,  -786,   943,   455,   101,  -786,   769,
     254,   769,   746,   936,  1165,   944,   769,  -786,   946,   952,
     947,   523,  -786,  -786,   968,  1171,  -786,  1172,   612,  -786,
    1063,  -786,  -786,  1065,   462,    27,    26,  1122,  -786,  -786,
    -786,  -786,  -786,  -786,  1124,  -786,  1177,  -786,  -786,  -786,
    -786,  -786,  -786,  -786,  -786,   956,  1139,   526,   971,   528,
    -786,   967,   972,   975,  1001,  1002,  1004,  1015,  1016,  1017,
    1104,  1018,  1026,  1030,  1031,  1032,  1033,  1057,  1061,  1062,
    1071,  1106,  1072,  1073,  1074,  1076,  1077,  1079,  1081,  1084,
    1085,  1089,  1147,  1091,  1093,  1095,  1105,  1108,  1109,  1126,
    1127,  1128,  1129,  1162,  1133,  1134,  1136,  1137,  1138,  1140,
    1141,  1142,  1144,  1145,  1199,  1146,  1148,  1149,  1150,  1151,
    1153,  1154,  1156,  1157,  1158,  1200,  1159,  -786,  -786,    22,
    -786,  1059,  1166,   544,  -786,  1063,  1196,  1291,   560,  -786,
    -786,  -786,   455,  -786,   119,  1160,    17,  1161,  -786,  -786,
    -786,  1163,  1240,  1167,   455,  -786,   206,  -786,  -786,  -786,
    -786,  -786,  -786,  -786,  -786,  -786,  -786,  1310,  -786,    83,
     641,    26,    26,  1096,   424,  1277,  1282,  -786,  1331,  -786,
    -786,  1113,  1332,  1341,  1342,  1345,  1359,  1363,  1367,  1375,
    1382,  1164,  1385,  1387,  1388,  1389,  1390,  1391,  1392,  1393,
    1394,  1395,  1176,  1397,  1398,  1399,  1400,  1401,  1402,  1403,
    1404,  1405,  1406,  1187,  1408,  1409,  1410,  1411,  1412,  1413,
    1414,  1415,  1416,  1417,  1198,  1419,  1420,  1421,  1422,  1423,
    1424,  1425,  1426,  1427,  1428,  1209,  1430,  1431,  1432,  1433,
    1434,  1435,  1436,  1437,  1438,  1439,  1220,  1441,  -786,  1444,
    1445,  -786,   562,  -786,   849,  -786,  1446,  1447,  1448,   262,
    1228,  -786,   565,  1449,  -786,  -786,  1396,   861,  -786,   769,
     769,  -786,  1229,  -786,  1231,  1232,  1235,  1236,  1237,  1238,
    1239,  1241,  1242,  1453,  1243,  1244,  1245,  1246,  1247,  1248,
    1249,  1250,  1252,  1253,  1459,  1254,  1255,  1256,  1257,  1258,
    1259,  1260,  1261,  1262,  1263,  1470,  1264,  1265,  1266,  1267,
    1268,  1269,  1270,  1271,  1272,  1273,  1493,  1275,  1276,  1278,
    1279,  1280,  1281,  1283,  1284,  1285,  1286,  1496,  1287,  1288,
    1289,  1290,  1292,  1293,  1294,  1295,  1296,  1297,  1501,  1298,
    -786,  -786,  -786,  -786,  1299,  1300,  1301,   944,  -786,  -786,
     541,   769,   583,   854,   455,  -786,  -786,  -786,  -786,  -786,
    -786,  -786,  -786,  -786,  -786,  1303,  -786,  -786,  -786,  -786,
    -786,  -786,  -786,  -786,  -786,  -786,  1304,  -786,  -786,  -786,
    -786,  -786,  -786,  -786,  -786,  -786,  -786,  1305,  -786,  -786,
    -786,  -786,  -786,  -786,  -786,  -786,  -786,  -786,  1306,  -786,
    -786,  -786,  -786,  -786,  -786,  -786,  -786,  -786,  -786,  1307,
    -786,  -786,  -786,  -786,  -786,  -786,  -786,  -786,  -786,  -786,
    1308,  -786,  1512,  1520,   121,  1309,  1311,  1529,  1531,  -786,
    -786,  -786,   455,  -786,  -786,  -786,  -786,  -786,  -786,  -786,
    1312,  1313,   944,   849,  -786,  1534,   718,   135,  1316,  1538,
    1318,  -786,   768,  1539,  -786,   944,   849,   944,     6,  1540,
    -786,  1497,  1322,  -786,  1323,  1510,  1511,  -786,  -786,  -786,
      59,   -45,  -786,  1327,  1513,  1514,  -786,  1515,  1516,  1553,
    -786,  1333,  -786,  1334,  1335,  1556,  1558,   849,  1337,  1338,
    -786,   849,  -786,  -786
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     237,     0,     0,     0,     0,     0,     0,     0,   237,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   237,     0,   553,     3,     5,    10,    12,    13,
      11,     6,     7,     9,   182,   181,     0,     8,    14,    15,
      16,    17,    18,    19,    20,   551,   551,   551,   551,   551,
       0,     0,   549,   549,   549,   549,   549,     0,   230,     0,
       0,     0,     0,     0,     0,   237,   168,    21,    26,    28,
      27,    22,    23,    25,    24,    29,    30,    31,    32,     0,
     295,   300,     0,     0,   251,   252,   250,   256,   260,     0,
     257,     0,     0,   253,     0,   255,   280,   281,     0,   258,
       0,   291,   293,   294,     0,   287,   303,   297,   302,     0,
       0,     0,     0,   306,   307,   308,   309,   311,   310,     0,
     236,   238,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   378,   335,     0,     0,     1,
     237,     2,   220,   222,   223,     0,   205,   187,   193,     0,
       0,     0,     0,     0,     0,     0,     0,   166,     0,     0,
       0,     0,     0,     0,   331,     0,     0,   215,     0,     0,
       0,     0,     0,     0,   167,     0,     0,     0,   267,   268,
     261,   262,   263,     0,   264,     0,   254,   282,     0,   259,
     292,     0,   285,   284,   288,   289,   305,     0,   299,     0,
       0,     0,   337,     0,     0,     0,     0,   359,     0,   369,
       0,   370,     0,   356,   357,     0,   352,     0,   365,   367,
       0,   360,     0,     0,     0,     0,     0,     0,     0,   379,
     186,   185,     4,   221,     0,   183,   184,   204,     0,     0,
     201,     0,    34,     0,    35,   166,   554,     0,     0,     0,
     237,   548,   173,   175,   174,   176,     0,   231,     0,   215,
     170,     0,   162,   547,     0,     0,   475,   479,   482,   483,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   480,
     481,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   477,   237,     0,     0,     0,
     380,   385,   386,   400,   398,   401,   399,   402,   403,   395,
     390,   389,   388,   396,   397,   387,   394,   393,   491,   494,
       0,   495,   503,     0,   504,     0,   496,   492,     0,   493,
     518,     0,   519,     0,   490,   315,   317,   316,   313,   314,
     320,   322,   321,   318,   319,   325,   327,   326,   323,   324,
     290,   296,   301,     0,     0,   270,   269,   275,   265,   266,
     283,   286,   304,   298,   557,     0,     0,     0,     0,     0,
     239,   312,   362,     0,   353,   358,     0,   366,   361,     0,
       0,   368,     0,   333,   334,   332,     0,     0,   207,     0,
       0,   203,   550,     0,   237,     0,     0,     0,     0,     0,
     330,   160,     0,     0,   164,     0,     0,     0,   169,   214,
       0,     0,     0,   527,   526,   529,   528,   531,   530,   533,
     532,   535,   534,   537,   536,     0,     0,   440,   237,     0,
       0,     0,     0,   484,   485,   486,   487,   488,     0,   489,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   442,   441,     0,     0,   524,   521,
     511,   501,   506,   509,   513,   514,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     500,     0,   505,     0,   508,     0,   512,     0,   520,     0,
     523,   276,   271,     0,   377,     0,     0,     0,   336,     0,
     371,     0,   354,     0,     0,     0,     0,   364,   190,   189,
       0,   209,   192,   194,   199,   200,     0,   188,    33,    37,
       0,     0,     0,     0,    43,    47,    48,   237,     0,    41,
     329,   328,   165,     0,     0,   163,   177,   172,   171,     0,
       0,     0,   429,     0,   237,     0,     0,     0,     0,     0,
     465,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   213,     0,     0,   392,   391,     0,
       0,   381,   384,   458,   459,     0,     0,   237,     0,   439,
     449,   450,   453,   454,     0,   456,   448,   451,   452,   444,
     443,   445,   446,   447,   476,   478,   502,     0,   507,     0,
     510,   515,   522,   525,     0,     0,   272,     0,     0,     0,
     374,   240,   355,     0,   338,   363,     0,     0,   206,     0,
     211,     0,   197,   198,   196,   202,     0,    55,    56,    59,
      60,    57,    58,    61,    62,    78,    63,    65,    64,    81,
      68,    69,    70,    66,    67,    71,    72,    73,    74,    75,
      76,    77,     0,     0,     0,     0,     0,     0,     0,     0,
     557,     0,     0,   559,     0,    40,     0,     0,   161,     0,
       0,     0,     0,     0,     0,   543,     0,     0,   538,     0,
       0,   430,     0,   471,     0,     0,   463,     0,     0,     0,
     437,   436,   435,   434,   433,   432,     0,     0,   475,     0,
       0,     0,     0,     0,   419,     0,   470,   517,   516,     0,
     237,   457,     0,     0,   438,     0,     0,     0,   277,   273,
     562,     0,   560,     0,     0,    45,   339,   372,   373,   237,
     208,   224,   226,   235,   227,     0,   215,   195,    39,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     153,   154,   157,   150,   157,     0,     0,     0,    36,    44,
     568,    42,   382,     0,   545,   544,   542,   541,   540,   546,
     180,     0,   178,   431,   472,     0,   467,     0,   466,     0,
       0,     0,     0,     0,     0,   213,     0,   417,     0,     0,
       0,     0,   473,   461,   460,     0,   278,     0,     0,   556,
       0,   376,   375,     0,     0,     0,     0,     0,   244,   245,
     246,   247,   243,   248,     0,   233,     0,   228,   423,   421,
     424,   422,   425,   426,   427,   210,   219,     0,     0,     0,
      53,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   155,   152,     0,
     151,    50,    49,     0,   159,     0,     0,     0,     0,   539,
     469,   464,   468,   455,     0,     0,     0,     0,   497,   499,
     498,   213,     0,     0,   212,   420,     0,   474,   462,   279,
     274,   563,   564,   566,   565,   561,    46,     0,   340,   235,
     225,     0,     0,   232,     0,     0,   217,    80,     0,   148,
     149,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   156,     0,
       0,   158,     0,    38,   557,   383,     0,     0,     0,     0,
       0,   418,     0,   341,   229,   241,     0,     0,   428,     0,
       0,   191,     0,    54,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      52,    51,   558,   567,     0,     0,   213,   213,   415,   179,
       0,     0,     0,   218,   216,    79,    85,    86,    83,    84,
      87,    88,    89,    90,    91,     0,    82,   129,   130,   127,
     128,   131,   132,   133,   134,   135,     0,   126,    96,    97,
      94,    95,    98,    99,   100,   101,   102,     0,    93,   107,
     108,   105,   106,   109,   110,   111,   112,   113,     0,   104,
     140,   141,   138,   139,   142,   143,   144,   145,   146,     0,
     137,   118,   119,   116,   117,   120,   121,   122,   123,   124,
       0,   115,     0,     0,     0,     0,     0,     0,     0,   343,
     342,   348,   242,   234,    92,   136,   103,   114,   147,   125,
     213,     0,   213,   557,   416,   349,   344,     0,     0,     0,
       0,   414,     0,     0,   345,   213,   557,   213,   557,     0,
     350,   346,     0,   410,     0,     0,     0,   413,   351,   347,
     557,   404,   412,     0,     0,     0,   409,     0,     0,     0,
     408,     0,   406,     0,     0,     0,     0,   557,     0,     0,
     411,   557,   405,   407
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -786,  -786,  -786,  1429,  1499,   248,  -786,  -786,   891,  -586,
    -786,  -680,  -786,   806,   807,  -786,  -609,   308,   359,  1321,
    -786,   378,  -786,  1168,   383,   386,    -6,  1545,   -17,  1201,
    1339,   -80,  -786,  -786,   945,  -786,  -786,  -786,  -786,  -786,
    -786,  -786,  -785,  -253,  -786,  -786,  -786,  -786,   756,  -138,
      37,   625,  -786,  -786,  1372,  -786,  -786,   395,   396,   402,
     412,   417,  -786,  -786,  -786,  -237,  -786,  1115,  -261,  -262,
    -724,  -720,  -707,  -706,  -705,  -699,   624,  -786,  -786,  -786,
    -786,  -786,  -786,  1152,  -786,  -786,  1008,  -295,  -293,  -786,
    -786,  -786,   788,  -786,  -786,  -786,  -786,   792,  -786,  -786,
    1097,  1087,   794,  -786,  -786,  -786,  -786,  1314,  -537,   808,
    -158,   593,   621,  -786,  -786,  -663,  -786,   669,   776,  -786
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    23,    24,    25,    66,    26,   533,   734,   534,   535,
     839,   670,   762,   763,   911,   536,   402,    27,    28,   250,
      29,    30,   259,   260,    31,    32,    33,    34,    35,   147,
     235,   148,   240,   522,   523,   634,   391,   527,   238,   521,
     630,   746,   713,   262,  1051,   956,   145,   740,   741,   742,
     743,   827,    36,   120,   121,   744,   824,    37,    38,    39,
      40,    41,    42,    43,    44,   299,   545,   300,   301,   302,
     303,   304,   305,   306,   307,   308,   834,   835,   309,   310,
     311,   312,   313,   432,   314,   315,   316,   317,   318,   927,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     462,   463,   329,   330,   331,   332,   333,   334,   687,   688,
     264,   159,   150,   141,   155,   504,   768,   731,   732,   539
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     409,   388,    73,   465,   461,   138,   408,   764,   427,   733,
     933,   265,   431,   689,   785,   458,   459,   267,   268,   269,
     146,   828,   458,   459,   604,   829,   267,   268,   269,    58,
     360,   454,   455,   735,   394,   457,   530,   708,   830,   831,
     832,    59,   261,    61,   180,    16,   833,   142,   371,   143,
     520,   142,   118,   143,   144,   468,   168,   169,   144,    73,
     187,   788,   430,   766,   335,   236,   336,   337,   585,   695,
     838,   840,    51,   789,   694,   495,   123,   588,   511,   496,
     777,   124,   134,   125,   503,   126,   825,   512,   157,    58,
     340,  1257,   341,   342,   182,   183,   345,   167,   346,   347,
     135,   469,   470,   274,    45,    46,    47,    52,    53,    54,
      48,    49,   274,    55,    56,   275,   276,   277,   632,   633,
     179,   278,  1258,   338,   275,   276,   277,  1222,   524,   525,
     278,   469,   470,   188,   469,   470,   589,   503,   826,   469,
     470,  1235,  1245,   469,   470,   547,  1040,   201,   202,   343,
     279,   280,   281,   586,   387,   348,   913,   469,   470,   279,
     280,   281,   531,   427,   532,   196,   636,   778,   557,   558,
     791,   229,   489,  1246,   490,   553,   469,   470,    60,   563,
     564,   565,   566,   567,   568,   921,   111,   796,   243,   244,
     245,    22,   576,   112,   467,  1254,   606,   253,   254,   255,
     611,   796,   407,   395,   170,   469,   470,   795,   583,   584,
     469,   470,   590,   591,   592,   593,   594,   595,   596,   597,
     598,   599,   600,   601,   602,   603,  1255,   946,   469,   470,
     828,   339,   197,   404,   829,   198,   234,   364,   460,   297,
     127,   605,   297,   464,   298,   460,   181,   830,   831,   832,
     739,   297,   949,   298,   295,   833,    67,   344,   469,   470,
     128,   469,   470,   349,   129,   635,  1127,   130,    50,   796,
    -555,    57,   473,   109,   681,   682,   469,   470,   578,   456,
     149,  1053,   117,   628,   397,   683,   684,   685,   119,   623,
     403,  -556,  -556,   476,   477,   469,   470,   793,   624,  -556,
     698,   699,   199,   110,   469,   470,   491,   679,   492,   707,
     758,   710,   136,    67,   690,   503,    68,   469,   470,   184,
     185,   469,   470,  -552,   721,   131,   132,   555,   796,   700,
       1,    16,     2,     3,     4,     5,     6,     7,     8,     9,
      10,  1205,  1206,   473,   133,  1036,    11,   469,   470,    12,
     723,    13,    14,    15,  -556,   481,   482,   483,   484,   485,
     486,   487,   122,   701,  -556,  -556,   759,    69,   760,   761,
     524,  1123,   702,    68,     1,   139,     2,     3,     4,     5,
       6,     7,     8,     9,    10,   703,    70,   473,   529,   704,
      11,    71,   686,    12,    72,    13,    14,    15,   758,  1042,
     493,    16,   494,    74,    75,   508,   474,   475,   476,   477,
      76,   556,   469,   470,   479,   705,   266,   267,   268,   269,
      77,   497,   719,   498,    69,    78,  -556,  -556,   483,   484,
     485,   486,   487,   149,   786,  1228,   787,  1230,  1132,   398,
     399,   140,   546,    70,   790,    16,   194,   146,    71,   195,
    1242,    72,  1244,   499,   759,   500,   760,   761,   400,   909,
      74,    75,   447,   804,   448,   449,   165,    76,   450,   480,
     481,   482,   483,   484,   485,   486,   487,    77,   551,   923,
     716,   353,    78,   801,   354,   355,   270,   271,   158,   356,
     357,   156,   212,   836,    17,   947,   272,   948,   273,   925,
     466,   467,   213,   274,   365,   214,   215,   216,   164,   217,
      18,   113,   114,   115,   166,   275,   276,   277,   366,   367,
     175,   278,    19,   218,   219,   171,   220,   221,   922,   116,
     924,   675,    20,    21,   172,   934,   918,   692,    17,   173,
     266,   267,   268,   269,    62,    63,   176,    22,   177,    64,
     279,   280,   281,   282,    18,   283,   178,   284,   186,   285,
    1231,   286,   142,   287,   143,   200,    19,   542,   543,   144,
     722,   204,   288,  1243,   189,  1247,    20,    21,   515,   516,
    1207,   209,   210,  1208,  1209,   190,   211,  1256,  1210,  1211,
     203,    22,   205,   560,   289,   561,   290,   562,   291,   696,
     206,   697,   207,   562,  1270,   289,   208,   290,  1273,   291,
     270,   271,   469,   470,  1028,   941,   942,   943,   944,   222,
     272,   223,   273,   615,   616,   430,   292,   274,   191,   192,
     193,   293,   294,   295,   471,   224,   472,   296,   552,   275,
     276,   277,   297,   225,   298,   278,   160,   161,   162,   163,
     841,   842,   843,   844,   845,   673,   674,   846,   847,   266,
     267,   268,   269,   226,   848,   849,   850,   151,   152,   153,
     154,   485,   486,   487,   279,   280,   281,   282,   227,   283,
     228,   284,   230,   285,   231,   286,   851,   287,   691,   467,
     714,   715,   717,   718,   473,   237,   288,   817,  -249,   818,
     819,   820,   821,   800,   822,   823,   772,   467,   780,   781,
     782,   781,   233,   474,   475,   476,   477,   478,   289,   239,
     290,   479,   291,   783,   467,   792,   488,   803,   467,   270,
     271,   805,   806,   815,    16,   809,   810,   811,   543,   272,
     241,   273,   812,   813,   914,   543,   274,   555,   938,   467,
     292,   957,   958,   960,   961,   293,   294,   295,   275,   276,
     277,   296,  1233,  1234,   278,   242,   297,   246,   298,  1031,
     543,   247,   266,   267,   268,   269,   480,   481,   482,   483,
     484,   485,   486,   487,   248,  1035,   467,  1122,   810,  1134,
    1129,   781,   252,   279,   280,   281,   282,   249,   283,   251,
     284,   256,   285,   257,   286,   258,   287,   473,  1213,   543,
    1239,  1240,  1133,  1045,  1046,   288,   261,   263,   351,   358,
     350,   352,   359,   368,   361,   362,   474,   475,   476,   477,
     363,   725,   369,   372,   479,   373,   374,   289,   375,   290,
     376,   291,   270,   271,   377,   378,   380,   379,   381,   382,
     383,   384,   272,   385,   273,   390,   389,   392,   393,   274,
     266,   267,   268,   269,   401,   406,   405,   412,   410,   292,
    1212,   275,   276,   277,   293,   294,   295,   278,   428,   411,
     296,   429,   433,   438,   434,   297,   435,   298,   436,   480,
     481,   482,   483,   484,   485,   486,   487,   437,   488,   440,
     441,   442,   443,   444,   445,   446,   279,   280,   281,   282,
     501,   283,   451,   284,   452,   285,   453,   286,   502,   287,
     505,   506,   266,   267,   268,   269,   510,   503,   288,   507,
     425,   426,   513,   514,   517,   518,   519,   526,   528,   538,
     272,   537,   273,   540,   541,   544,   569,   274,   549,    16,
     289,   550,   290,   554,   291,   570,   571,   572,   573,   275,
     276,   277,   574,   575,   577,   278,   580,   579,   582,   587,
     852,   853,   854,   855,   856,   297,   607,   857,   858,   609,
     612,   613,   292,   617,   859,   860,   861,   293,   294,   295,
     614,   618,   425,   296,   279,   280,   281,   282,   297,   283,
     298,   284,   272,   285,   273,   286,   862,   287,   619,   274,
     620,   621,   622,   625,   629,   671,   288,   676,   626,   631,
     627,   275,   276,   277,    65,   677,   678,   278,     1,   672,
       2,     3,     4,     5,     6,     7,   680,     9,   289,   693,
     290,   555,   291,   586,    11,   706,   711,    12,   712,    13,
      14,    15,   469,   724,   728,   720,   279,   280,   281,   282,
     726,   283,   729,   284,   727,   285,   730,   286,   530,   287,
     292,   737,   736,   738,   745,   293,   294,   295,   288,   748,
     467,   296,   749,   750,   751,   767,   297,     1,   298,     2,
       3,     4,     5,     6,     7,   770,     9,   752,   771,    16,
     289,   473,   290,    11,   291,   753,    12,   774,    13,    14,
      15,   754,   755,   775,   776,    79,   756,   779,   798,   799,
     474,   475,   476,   477,   757,   765,   773,   718,   479,   784,
     717,   794,   292,   797,   802,    80,   807,   293,   294,   295,
     808,   814,    81,   296,    82,    83,   816,    84,   297,   837,
     298,   907,    85,    86,   908,   915,   909,   917,    16,   863,
     864,   865,   866,   867,   916,   926,   868,   869,   920,   931,
     932,   935,   937,   870,   871,   872,   936,   939,   940,   951,
     953,   952,   954,   480,   481,   482,   483,   484,   485,   486,
     487,   955,    17,   962,  1029,   873,   959,   971,   963,   982,
    1033,   964,   637,   638,   639,   640,   641,   642,   643,   644,
     645,   646,   647,   648,   649,   650,   651,   652,   653,   654,
      19,   655,   656,   657,   658,   659,   660,   965,   966,   661,
     967,    21,   662,   663,   664,   665,   666,   667,   668,   669,
     993,   968,   969,   970,   972,    22,   874,   875,   876,   877,
     878,    17,   973,   879,   880,  1004,   974,   975,   976,   977,
     881,   882,   883,   413,   414,   415,   416,   417,   418,   419,
     420,   421,   422,   423,   424,    87,    88,    89,    90,    19,
      91,    92,   884,   978,    93,    94,    95,   979,   980,    96,
      21,    97,  1015,  1026,  1034,    98,    99,   981,   983,   984,
     985,  1030,   986,   987,    22,   988,   796,   989,   100,   101,
     990,   991,   102,   103,   104,   992,  1043,   994,   105,   995,
    1047,   996,   106,   107,   108,   885,   886,   887,   888,   889,
    1049,   997,   890,   891,   998,   999,  1050,  1052,  1054,   892,
     893,   894,   896,   897,   898,   899,   900,  1055,  1056,   901,
     902,  1057,  1000,  1001,  1002,  1003,   903,   904,   905,  1005,
    1006,   895,  1007,  1008,  1009,  1058,  1010,  1011,  1012,  1059,
    1013,  1014,  1016,  1060,  1017,  1018,  1019,  1020,   906,  1021,
    1022,  1061,  1023,  1024,  1025,  1027,  1037,  1038,  1062,  1039,
    1063,  1064,  1041,  1065,  1066,  1067,  1068,  1069,  1070,  1071,
    1072,  1073,  1074,  1075,  1076,  1077,  1078,  1079,  1080,  1081,
    1082,  1083,  1084,  1085,  1086,  1087,  1088,  1089,  1090,  1091,
    1092,  1093,  1094,  1095,  1096,  1097,  1098,  1099,  1100,  1101,
    1102,  1103,  1104,  1105,  1106,  1107,  1108,  1109,  1110,  1111,
    1112,  1113,  1114,  1115,  1116,  1117,  1118,  1119,  1120,  1121,
    1124,  1125,  1126,  1128,  1135,  1130,  1136,  1137,  1131,  1145,
    1138,  1139,  1140,  1141,  1142,  1156,  1143,  1144,  1146,  1147,
    1148,  1149,  1150,  1151,  1152,  1153,  1167,  1154,  1155,  1157,
    1158,  1159,  1160,  1161,  1162,  1163,  1164,  1165,  1166,  1168,
    1169,  1170,  1171,  1172,  1173,  1174,  1175,  1176,  1177,  1178,
    1179,  1180,  1189,  1181,  1182,  1183,  1184,  1200,  1185,  1186,
    1187,  1188,  1190,  1191,  1192,  1193,  1220,  1194,  1195,  1196,
    1197,  1198,  1199,  1201,  1221,  1202,  1203,  1204,  1214,  1215,
    1216,  1217,  1218,  1219,  1223,  1225,  1224,  1226,  1227,  1229,
    1232,  1236,  1237,  1238,  1249,  1241,  1248,  1250,  1251,  1252,
    1253,  1259,  1260,  1261,  1262,  1263,  1264,  1265,  1266,  1268,
    1267,  1269,  1271,  1272,   174,   769,   396,   137,   910,   232,
     509,   912,   950,   386,  1044,   548,   747,   370,  1048,   709,
     610,   928,   581,   559,  1032,   929,   945,   930,   608,   919,
       0,     0,     0,     0,     0,   439
};

static const yytype_int16 yycheck[] =
{
     261,   238,     8,   298,   297,    22,   259,   670,   270,   618,
     795,   169,   273,   550,   694,     5,     6,     4,     5,     6,
       8,   745,     5,     6,     3,   745,     4,     5,     6,     3,
     188,   293,   294,   619,    55,   296,     3,     3,   745,   745,
     745,     4,    66,     6,     6,    78,   745,    20,   206,    22,
      82,    20,    15,    22,    27,    55,    76,    77,    27,    65,
       4,    84,    88,   672,     3,   145,     5,     6,    75,    87,
     750,   751,    36,    87,    55,   226,     7,    73,    39,   230,
       3,    12,    33,    14,    78,    16,     3,    48,    51,     3,
       3,   136,     5,     6,   175,   176,     3,    60,     5,     6,
      51,   157,   158,    90,    32,    33,    34,    32,    33,    34,
      38,    39,    90,    38,    39,   102,   103,   104,    70,    71,
      83,   108,   167,    62,   102,   103,   104,     6,   389,   390,
     108,   157,   158,    77,   157,   158,   132,    78,    55,   157,
     158,     6,   136,   157,   158,   406,   931,   110,   111,    62,
     137,   138,   139,   160,   234,    62,   765,   157,   158,   137,
     138,   139,   129,   425,   131,     6,   135,    90,   429,   430,
     226,   134,   226,   167,   228,   412,   157,   158,    76,   440,
     441,   442,   443,   444,   445,    84,    33,    66,   151,   152,
     153,   224,   453,    39,   226,   136,   489,   160,   161,   162,
     495,    66,   226,   224,   224,   157,   158,     4,   469,   470,
     157,   158,   474,   475,   476,   477,   478,   479,   480,   481,
     482,   483,   484,   485,   486,   487,   167,   813,   157,   158,
     954,   170,    73,   250,   954,     6,   224,   200,   228,   229,
     171,   220,   229,   230,   231,   228,   208,   954,   954,   954,
     224,   229,   225,   231,   220,   954,     8,   170,   157,   158,
     191,   157,   158,   170,   195,   526,     4,   198,   196,    66,
      62,   196,   133,     3,    68,    69,   157,   158,   225,   296,
      72,   961,     3,   520,   247,    79,    80,    81,     3,    32,
      37,   152,   153,   154,   155,   157,   158,   226,    41,   160,
     561,   562,    73,    33,   157,   158,   226,   544,   228,   570,
      73,   572,   161,    65,   551,    78,     8,   157,   158,   175,
     176,   157,   158,     0,   586,    32,    33,    73,    66,   225,
       7,    78,     9,    10,    11,    12,    13,    14,    15,    16,
      17,  1126,  1127,   133,    51,   226,    23,   157,   158,    26,
     587,    28,    29,    30,   215,   216,   217,   218,   219,   220,
     221,   222,   174,   225,   154,   155,   129,     8,   131,   132,
     631,  1034,   225,    65,     7,     0,     9,    10,    11,    12,
      13,    14,    15,    16,    17,   225,     8,   133,   394,   225,
      23,     8,   186,    26,     8,    28,    29,    30,    73,   936,
     226,    78,   228,     8,     8,   368,   152,   153,   154,   155,
       8,   428,   157,   158,   160,   225,     3,     4,     5,     6,
       8,   226,   580,   228,    65,     8,   216,   217,   218,   219,
     220,   221,   222,    72,   695,  1220,   697,  1222,  1047,    32,
      33,   223,   405,    65,   706,    78,   204,     8,    65,   207,
    1235,    65,  1237,   226,   129,   228,   131,   132,    51,   134,
      65,    65,   122,   725,   124,   125,   227,    65,   128,   215,
     216,   217,   218,   219,   220,   221,   222,    65,    65,   225,
     225,    39,    65,   720,    42,    43,    73,    74,    72,    47,
      48,     3,   162,   746,   171,    33,    83,    35,    85,   792,
     225,   226,   172,    90,    10,   175,   176,   177,     3,   179,
     187,   161,   162,   163,    14,   102,   103,   104,    24,    25,
     224,   108,   199,   193,   194,     3,   196,   197,   789,   179,
     791,   537,   209,   210,     3,   796,   773,   554,   171,     3,
       3,     4,     5,     6,   168,   169,    73,   224,    73,   173,
     137,   138,   139,   140,   187,   142,     3,   144,     3,   146,
    1223,   148,    20,   150,    22,    62,   199,   225,   226,    27,
     587,    55,   159,  1236,     6,  1238,   209,   210,   189,   190,
      39,   187,   188,    42,    43,     3,   192,  1250,    47,    48,
       3,   224,   226,    84,   181,    86,   183,    88,   185,    84,
       6,    86,   196,    88,  1267,   181,   196,   183,  1271,   185,
      73,    74,   157,   158,   909,     3,     4,     5,     6,   196,
      83,     4,    85,    44,    45,    88,   213,    90,   203,   204,
     205,   218,   219,   220,    73,   194,    75,   224,   225,   102,
     103,   104,   229,    76,   231,   108,    53,    54,    55,    56,
      91,    92,    93,    94,    95,   225,   226,    98,    99,     3,
       4,     5,     6,   196,   105,   106,   107,    46,    47,    48,
      49,   220,   221,   222,   137,   138,   139,   140,   196,   142,
     196,   144,   225,   146,   225,   148,   127,   150,   225,   226,
     225,   226,     5,     6,   133,    65,   159,    56,    57,    58,
      59,    60,    61,   720,    63,    64,   225,   226,   225,   226,
     225,   226,    21,   152,   153,   154,   155,   156,   181,    67,
     183,   160,   185,   225,   226,   226,   227,   225,   226,    73,
      74,    49,    50,   739,    78,   225,   226,   225,   226,    83,
      73,    85,   225,   226,   225,   226,    90,    73,   225,   226,
     213,   225,   226,   225,   226,   218,   219,   220,   102,   103,
     104,   224,    44,    45,   108,     3,   229,     3,   231,   225,
     226,    62,     3,     4,     5,     6,   215,   216,   217,   218,
     219,   220,   221,   222,    62,   225,   226,   225,   226,  1050,
     225,   226,     3,   137,   138,   139,   140,   224,   142,    74,
     144,     3,   146,     3,   148,     3,   150,   133,   225,   226,
      42,    43,  1049,   951,   952,   159,    66,     4,   132,     3,
     225,   132,     3,    62,     4,   132,   152,   153,   154,   155,
     132,   157,   224,     4,   160,   167,     6,   181,     3,   183,
       6,   185,    73,    74,     4,     3,    55,     4,     4,   196,
       3,     3,    83,     3,    85,    68,    53,    74,   135,    90,
       3,     4,     5,     6,     3,   215,    62,   224,    78,   213,
    1131,   102,   103,   104,   218,   219,   220,   108,   224,    78,
     224,   224,     4,     6,     4,   229,     4,   231,     4,   215,
     216,   217,   218,   219,   220,   221,   222,     4,   227,   224,
     224,   224,   224,   224,   224,   224,   137,   138,   139,   140,
       3,   142,   224,   144,   224,   146,   224,   148,     6,   150,
      46,    46,     3,     4,     5,     6,     4,    78,   159,    77,
      73,    74,     6,    77,     4,   225,   225,    69,     4,   224,
      83,    55,    85,     3,     3,   224,     4,    90,   224,    78,
     181,   224,   183,   224,   185,   224,   224,   224,   224,   102,
     103,   104,     4,     4,   225,   108,    77,   232,     3,   224,
      91,    92,    93,    94,    95,   229,     6,    98,    99,     6,
       6,     5,   213,   224,   105,   106,   107,   218,   219,   220,
      42,   224,    73,   224,   137,   138,   139,   140,   229,   142,
     231,   144,    83,   146,    85,   148,   127,   150,   224,    90,
       3,   225,     6,     4,    76,   130,   159,     3,   167,   226,
     167,   102,   103,   104,     3,   135,     3,   108,     7,   224,
       9,    10,    11,    12,    13,    14,   226,    16,   181,   225,
     183,    73,   185,   160,    23,    76,     4,    26,   226,    28,
      29,    30,   157,   132,     6,   224,   137,   138,   139,   140,
     232,   142,     6,   144,   232,   146,     3,   148,     3,   150,
     213,     4,     6,     4,   180,   218,   219,   220,   159,     4,
     226,   224,   224,   224,   224,    31,   229,     7,   231,     9,
      10,    11,    12,    13,    14,   225,    16,   224,     4,    78,
     181,   133,   183,    23,   185,   224,    26,     6,    28,    29,
      30,   224,   224,     6,     4,     3,   224,     6,     4,    78,
     152,   153,   154,   155,   224,   224,   224,     6,   160,   225,
       5,   226,   213,   225,   225,    23,    46,   218,   219,   220,
     215,     6,    30,   224,    32,    33,   226,    35,   229,     6,
     231,   132,    40,    41,   130,   224,   134,   167,    78,    91,
      92,    93,    94,    95,   135,   229,    98,    99,   225,     4,
     226,   225,   225,   105,   106,   107,   224,     6,     6,    57,
       3,    57,   226,   215,   216,   217,   218,   219,   220,   221,
     222,    52,   171,   226,   135,   127,   225,    93,   226,    93,
       4,   226,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     199,   108,   109,   110,   111,   112,   113,   226,   226,   116,
     226,   210,   119,   120,   121,   122,   123,   124,   125,   126,
      93,   226,   226,   226,   226,   224,    91,    92,    93,    94,
      95,   171,   226,    98,    99,    93,   226,   226,   226,   226,
     105,   106,   107,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   163,   164,   165,   166,   199,
     168,   169,   127,   226,   172,   173,   174,   226,   226,   177,
     210,   179,    93,    93,     3,   183,   184,   226,   226,   226,
     226,   135,   226,   226,   224,   226,    66,   226,   196,   197,
     226,   226,   200,   201,   202,   226,     6,   226,   206,   226,
     224,   226,   210,   211,   212,    91,    92,    93,    94,    95,
      53,   226,    98,    99,   226,   226,    54,     6,     6,   105,
     106,   107,    91,    92,    93,    94,    95,     6,     6,    98,
      99,     6,   226,   226,   226,   226,   105,   106,   107,   226,
     226,   127,   226,   226,   226,     6,   226,   226,   226,     6,
     226,   226,   226,     6,   226,   226,   226,   226,   127,   226,
     226,     6,   226,   226,   226,   226,   226,   226,     6,   226,
     226,     6,   225,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   226,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   226,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   226,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   226,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   226,     6,     4,     4,
       4,     4,     4,   225,   225,     6,   225,   225,    62,     6,
     225,   225,   225,   225,   225,     6,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,     6,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,     6,
     225,   225,     6,   225,   225,   225,   225,     6,   225,   225,
     225,   225,   225,   225,   225,   225,     4,   225,   225,   225,
     225,   225,   225,   225,     4,   226,   226,   226,   225,   225,
     225,   225,   225,   225,   225,     6,   225,     6,   226,   226,
       6,   225,     4,   225,    47,     6,     6,   225,   225,    39,
      39,   224,    39,    39,    39,    39,     3,   224,   224,     3,
     225,     3,   225,   225,    65,   674,   245,    22,   762,   140,
     369,   764,   816,   234,   949,   407,   631,   205,   954,   571,
     493,   793,   467,   431,   915,   793,   810,   793,   491,   781,
      -1,    -1,    -1,    -1,    -1,   281
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    23,    26,    28,    29,    30,    78,   171,   187,   199,
     209,   210,   224,   234,   235,   236,   238,   250,   251,   253,
     254,   257,   258,   259,   260,   261,   285,   290,   291,   292,
     293,   294,   295,   296,   297,    32,    33,    34,    38,    39,
     196,    36,    32,    33,    34,    38,    39,   196,     3,   283,
      76,   283,   168,   169,   173,     3,   237,   238,   250,   251,
     254,   257,   258,   259,   290,   291,   292,   293,   294,     3,
      23,    30,    32,    33,    35,    40,    41,   163,   164,   165,
     166,   168,   169,   172,   173,   174,   177,   179,   183,   184,
     196,   197,   200,   201,   202,   206,   210,   211,   212,     3,
      33,    33,    39,   161,   162,   163,   179,     3,   283,     3,
     286,   287,   174,     7,    12,    14,    16,   171,   191,   195,
     198,    32,    33,    51,    33,    51,   161,   260,   261,     0,
     223,   346,    20,    22,    27,   279,     8,   262,   264,    72,
     345,   345,   345,   345,   345,   347,     3,   283,    72,   344,
     344,   344,   344,   344,     3,   227,    14,   283,    76,    77,
     224,     3,     3,     3,   237,   224,    73,    73,     3,   283,
       6,   208,   175,   176,   175,   176,     3,     4,    77,     6,
       3,   203,   204,   205,   204,   207,     6,    73,     6,    73,
      62,   283,   283,     3,    55,   226,     6,   196,   196,   187,
     188,   192,   162,   172,   175,   176,   177,   179,   193,   194,
     196,   197,   196,     4,   194,    76,   196,   196,   196,   283,
     225,   225,   236,    21,   224,   263,   264,    65,   271,    67,
     265,    73,     3,   283,   283,   283,     3,    62,    62,   224,
     252,    74,     3,   283,   283,   283,     3,     3,     3,   255,
     256,    66,   276,     4,   343,   343,     3,     4,     5,     6,
      73,    74,    83,    85,    90,   102,   103,   104,   108,   137,
     138,   139,   140,   142,   144,   146,   148,   150,   159,   181,
     183,   185,   213,   218,   219,   220,   224,   229,   231,   298,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   311,
     312,   313,   314,   315,   317,   318,   319,   320,   321,   323,
     324,   325,   326,   327,   328,   329,   330,   331,   332,   335,
     336,   337,   338,   339,   340,     3,     5,     6,    62,   170,
       3,     5,     6,    62,   170,     3,     5,     6,    62,   170,
     225,   132,   132,    39,    42,    43,    47,    48,     3,     3,
     343,     4,   132,   132,   283,    10,    24,    25,    62,   224,
     287,   343,     4,   167,     6,     3,     6,     4,     3,     4,
      55,     4,   196,     3,     3,     3,   263,   264,   298,    53,
      68,   269,    74,   135,    55,   224,   252,   283,    32,    33,
      51,     3,   249,    37,   261,    62,   215,   226,   276,   301,
      78,    78,   224,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,    73,    74,   302,   224,   224,
      88,   301,   316,     4,     4,     4,     4,     4,     6,   340,
     224,   224,   224,   224,   224,   224,   224,   122,   124,   125,
     128,   224,   224,   224,   302,   302,   261,   301,     5,     6,
     228,   321,   333,   334,   230,   320,   225,   226,    55,   157,
     158,    73,    75,   133,   152,   153,   154,   155,   156,   160,
     215,   216,   217,   218,   219,   220,   221,   222,   227,   226,
     228,   226,   228,   226,   228,   226,   230,   226,   228,   226,
     228,     3,     6,    78,   348,    46,    46,    77,   283,   262,
       4,    39,    48,     6,    77,   189,   190,     4,   225,   225,
      82,   272,   266,   267,   301,   301,    69,   270,     4,   259,
       3,   129,   131,   239,   241,   242,   248,    55,   224,   352,
       3,     3,   225,   226,   224,   299,   283,   301,   256,   224,
     224,    65,   225,   298,   224,    73,   261,   301,   301,   316,
      84,    86,    88,   301,   301,   301,   301,   301,   301,     4,
     224,   224,   224,   224,     4,     4,   301,   225,   225,   232,
      77,   300,     3,   301,   301,    75,   160,   224,    73,   132,
     302,   302,   302,   302,   302,   302,   302,   302,   302,   302,
     302,   302,   302,   302,     3,   220,   321,     6,   333,     6,
     334,   320,     6,     5,    42,    44,    45,   224,   224,   224,
       3,   225,     6,    32,    41,     4,   167,   167,   298,    76,
     273,   226,    70,    71,   268,   301,   135,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   108,   109,   110,   111,   112,
     113,   116,   119,   120,   121,   122,   123,   124,   125,   126,
     244,   130,   224,   225,   226,   259,     3,   135,     3,   298,
     226,    68,    69,    79,    80,    81,   186,   341,   342,   341,
     298,   225,   261,   225,    55,    87,    84,    86,   301,   301,
     225,   225,   225,   225,   225,   225,    76,   301,     3,   319,
     301,     4,   226,   275,   225,   226,   225,     5,     6,   343,
     224,   302,   261,   298,   132,   157,   232,   232,     6,     6,
       3,   350,   351,   249,   240,   242,     6,     4,     4,   224,
     280,   281,   282,   283,   288,   180,   274,   267,     4,   224,
     224,   224,   224,   224,   224,   224,   224,   224,    73,   129,
     131,   132,   245,   246,   348,   224,   249,    31,   349,   241,
     225,     4,   225,   224,     6,     6,     4,     3,    90,     6,
     225,   226,   225,   225,   225,   244,   301,   301,    84,    87,
     302,   226,   226,   226,   226,     4,    66,   225,     4,    78,
     261,   298,   225,   225,   302,    49,    50,    46,   215,   225,
     226,   225,   225,   226,     6,   259,   226,    56,    58,    59,
      60,    61,    63,    64,   289,     3,    55,   284,   303,   304,
     305,   306,   307,   308,   309,   310,   276,     6,   244,   243,
     244,    91,    92,    93,    94,    95,    98,    99,   105,   106,
     107,   127,    91,    92,    93,    94,    95,    98,    99,   105,
     106,   107,   127,    91,    92,    93,    94,    95,    98,    99,
     105,   106,   107,   127,    91,    92,    93,    94,    95,    98,
      99,   105,   106,   107,   127,    91,    92,    93,    94,    95,
      98,    99,   105,   106,   107,   127,    91,    92,    93,    94,
      95,    98,    99,   105,   106,   107,   127,   132,   130,   134,
     246,   247,   247,   249,   225,   224,   135,   167,   298,   342,
     225,    84,   301,   225,   301,   321,   229,   322,   325,   330,
     335,     4,   226,   275,   301,   225,   224,   225,   225,     6,
       6,     3,     4,     5,     6,   351,   242,    33,    35,   225,
     281,    57,    57,     3,   226,    52,   278,   225,   226,   225,
     225,   226,   226,   226,   226,   226,   226,   226,   226,   226,
     226,    93,   226,   226,   226,   226,   226,   226,   226,   226,
     226,   226,    93,   226,   226,   226,   226,   226,   226,   226,
     226,   226,   226,    93,   226,   226,   226,   226,   226,   226,
     226,   226,   226,   226,    93,   226,   226,   226,   226,   226,
     226,   226,   226,   226,   226,    93,   226,   226,   226,   226,
     226,   226,   226,   226,   226,   226,    93,   226,   320,   135,
     135,   225,   350,     4,     3,   225,   226,   226,   226,   226,
     275,   225,   341,     6,   284,   282,   282,   224,   309,    53,
      54,   277,     6,   244,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   226,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   226,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   226,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   226,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   226,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   226,     6,
       4,     4,   225,   348,     4,     4,     4,     4,   225,   225,
       6,    62,   249,   298,   301,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,     6,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,     6,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,     6,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,     6,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,     6,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
       6,   225,   226,   226,   226,   275,   275,    39,    42,    43,
      47,    48,   301,   225,   225,   225,   225,   225,   225,   225,
       4,     4,     6,   225,   225,     6,     6,   226,   275,   226,
     275,   348,     6,    44,    45,     6,   225,     4,   225,    42,
      43,     6,   275,   348,   275,   136,   167,   348,     6,    47,
     225,   225,    39,    39,   136,   167,   348,   136,   167,   224,
      39,    39,    39,    39,     3,   224,   224,   225,     3,     3,
     348,   225,   225,   348
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   233,   234,   235,   235,   236,   236,   236,   236,   236,
     236,   236,   236,   236,   236,   236,   236,   236,   236,   236,
     236,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   239,   239,   240,   240,   241,   241,   242,
     242,   242,   242,   243,   243,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     245,   245,   246,   246,   246,   246,   247,   247,   248,   248,
     249,   249,   250,   251,   251,   252,   252,   253,   253,   254,
     255,   255,   256,   257,   257,   257,   257,   257,   258,   258,
     258,   259,   259,   259,   259,   260,   260,   261,   262,   263,
     263,   264,   265,   265,   266,   266,   267,   268,   268,   268,
     269,   269,   270,   270,   271,   271,   272,   272,   273,   273,
     274,   274,   275,   275,   276,   276,   277,   277,   278,   278,
     279,   279,   279,   279,   280,   280,   281,   281,   282,   282,
     283,   283,   284,   284,   284,   284,   285,   285,   286,   286,
     287,   288,   288,   289,   289,   289,   289,   289,   289,   289,
     290,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     290,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     290,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     290,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     290,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     290,   290,   290,   290,   290,   290,   291,   291,   291,   291,
     292,   293,   293,   293,   293,   293,   293,   293,   293,   293,
     293,   293,   293,   293,   293,   293,   293,   293,   293,   293,
     293,   293,   293,   293,   293,   293,   293,   294,   295,   295,
     295,   295,   295,   295,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   296,   296,   296,   296,   297,   297,
     298,   298,   299,   299,   300,   300,   301,   301,   301,   301,
     301,   302,   302,   302,   302,   302,   302,   302,   302,   302,
     302,   302,   302,   302,   303,   303,   303,   304,   304,   304,
     304,   305,   305,   305,   305,   306,   306,   307,   307,   308,
     308,   309,   309,   309,   309,   309,   309,   310,   310,   311,
     311,   311,   311,   311,   311,   311,   311,   311,   311,   311,
     311,   311,   311,   311,   311,   311,   311,   311,   311,   311,
     311,   311,   311,   311,   311,   311,   311,   311,   312,   312,
     313,   314,   314,   315,   315,   315,   315,   316,   316,   317,
     317,   318,   318,   318,   318,   319,   319,   319,   319,   320,
     320,   320,   320,   320,   320,   320,   320,   320,   320,   320,
     320,   320,   320,   321,   321,   321,   321,   322,   322,   322,
     323,   324,   324,   325,   325,   326,   327,   327,   328,   329,
     329,   330,   331,   331,   332,   332,   333,   334,   335,   335,
     336,   337,   337,   338,   339,   339,   340,   340,   340,   340,
     340,   340,   340,   340,   340,   340,   340,   340,   341,   341,
     342,   342,   342,   342,   342,   342,   342,   343,   344,   344,
     345,   345,   346,   346,   347,   347,   348,   348,   349,   349,
     350,   350,   351,   351,   351,   351,   351,   352,   352
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
       1,     1,     1,     1,     1,     1,     1,     1,     1,     6,
       4,     1,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     7,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     7,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     7,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     7,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     7,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     7,     4,     4,
       1,     2,     2,     1,     1,     2,     2,     0,     5,     4,
       1,     3,     4,     6,     5,     3,     0,     3,     2,     5,
       1,     3,     3,     4,     4,     4,     4,     6,     8,    11,
       8,     1,     1,     3,     3,     3,     3,     2,     4,     3,
       3,     9,     3,     0,     1,     3,     2,     1,     1,     0,
       2,     0,     2,     0,     1,     0,     2,     0,     2,     0,
       2,     0,     3,     0,     2,     0,     2,     0,     3,     0,
       1,     2,     1,     1,     1,     3,     1,     1,     2,     4,
       1,     3,     2,     1,     5,     0,     2,     0,     1,     3,
       5,     4,     6,     1,     1,     1,     1,     1,     1,     0,
       2,     2,     2,     2,     3,     2,     2,     2,     2,     3,
       2,     3,     3,     3,     3,     4,     4,     3,     3,     4,
       4,     5,     6,     7,     9,     4,     5,     7,     8,     9,
       2,     2,     3,     4,     3,     3,     4,     2,     3,     3,
       4,     2,     3,     2,     2,     2,     4,     2,     4,     3,
       2,     4,     2,     2,     4,     3,     2,     2,     2,     2,
       2,     2,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     6,     6,
       5,     3,     4,     4,     4,     2,     5,     3,     6,     7,
       9,    10,    12,    12,    13,    14,    15,    16,    12,    13,
      15,    16,     3,     4,     5,     6,     3,     3,     4,     3,
       3,     4,     4,     6,     5,     3,     4,     3,     4,     3,
       3,     5,     7,     7,     6,     8,     8,     5,     2,     3,
       1,     3,     3,     5,     3,     1,     1,     1,     1,     1,
       1,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,    14,    19,    16,    20,    16,    15,
      13,    18,    14,    13,    11,     8,    10,     5,     7,     4,
       6,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       4,     5,     4,     4,     4,     4,     4,     4,     4,     3,
       2,     2,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     6,     3,     4,     3,     3,
       5,     5,     6,     4,     6,     3,     5,     4,     5,     6,
       4,     4,     5,     5,     6,     1,     3,     1,     3,     1,
       1,     1,     1,     1,     2,     2,     2,     2,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     3,     1,     1,     2,     2,     3,     2,     2,
       3,     2,     2,     2,     2,     3,     3,     3,     1,     1,
       2,     2,     3,     2,     2,     3,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     1,     3,
       2,     2,     2,     1,     2,     2,     2,     1,     2,     0,
       3,     0,     1,     0,     2,     0,     4,     0,     4,     0,
       1,     3,     1,     3,     3,     3,     3,     6,     3
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
#line 330 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2503 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 330 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2511 "parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 247 "parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 2525 "parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 227 "parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 2539 "parser.cpp"
        break;

    case YYSYMBOL_column_def_array: /* column_def_array  */
#line 237 "parser.y"
            {
    fprintf(stderr, "destroy column def array\n");
    if ((((*yyvaluep).column_def_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).column_def_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).column_def_array_t));
    }
}
#line 2553 "parser.cpp"
        break;

    case YYSYMBOL_column_type_array: /* column_type_array  */
#line 191 "parser.y"
            {
    fprintf(stderr, "destroy column_type_array\n");
    delete (((*yyvaluep).column_type_array_t));
}
#line 2562 "parser.cpp"
        break;

    case YYSYMBOL_column_type: /* column_type  */
#line 186 "parser.y"
            {
    fprintf(stderr, "destroy column_type\n");
    delete (((*yyvaluep).column_type_t));
}
#line 2571 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 323 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2582 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2590 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2599 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2608 "parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 294 "parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 2622 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 287 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2633 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2643 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2653 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2663 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2673 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2683 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2693 "parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 277 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2707 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 277 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2721 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 357 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2731 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2739 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2747 "parser.cpp"
        break;

    case YYSYMBOL_highlight_clause: /* highlight_clause  */
#line 257 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2761 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2770 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2778 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2786 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2794 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2802 "parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 257 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2816 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2825 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2834 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2843 "parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 314 "parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2856 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 347 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2865 "parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 304 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2879 "parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 304 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2893 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 363 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2903 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2912 "parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 257 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2926 "parser.cpp"
        break;

    case YYSYMBOL_insert_row_list: /* insert_row_list  */
#line 267 "parser.y"
            {
    fprintf(stderr, "destroy insert row list\n");
    if ((((*yyvaluep).insert_row_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).insert_row_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).insert_row_list_t));
    }
}
#line 2940 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2948 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2956 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2964 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2972 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2980 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2988 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2996 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3004 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3012 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3020 "parser.cpp"
        break;

    case YYSYMBOL_sub_search_array: /* sub_search_array  */
#line 257 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 3034 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3042 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3050 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3058 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3066 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3074 "parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 375 "parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 3087 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3095 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3103 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3111 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3119 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3127 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3135 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3143 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3151 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3159 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3167 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3175 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3183 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3191 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3199 "parser.cpp"
        break;

    case YYSYMBOL_curly_brackets_expr: /* curly_brackets_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3207 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_curly_brackets_expr: /* unclosed_curly_brackets_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3215 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 384 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3223 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 388 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3231 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3239 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3247 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3255 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3263 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3271 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3279 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 330 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3287 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 220 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3298 "parser.cpp"
        break;

    case YYSYMBOL_with_index_param_list: /* with_index_param_list  */
#line 203 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 3312 "parser.cpp"
        break;

    case YYSYMBOL_optional_table_properties_list: /* optional_table_properties_list  */
#line 203 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 3326 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 196 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3337 "parser.cpp"
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
#line 88 "parser.y"
{
  // Initialize
  yylloc.first_column = 0;
  yylloc.last_column = 0;
  yylloc.first_line = 0;
  yylloc.last_line = 0;
  yylloc.total_column = 0;
  yylloc.string_length = 0;
}

#line 3445 "parser.cpp"

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
#line 516 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3660 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 520 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3671 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 526 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3682 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 533 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3688 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 534 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3694 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 535 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3700 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 536 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3706 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 537 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3712 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 538 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3718 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 539 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3724 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 540 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3730 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 541 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3736 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 542 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3742 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 543 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3748 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 544 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3754 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 545 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3760 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 546 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3766 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 547 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3772 "parser.cpp"
    break;

  case 20: /* statement: check_statement  */
#line 548 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].check_stmt); }
#line 3778 "parser.cpp"
    break;

  case 21: /* explainable_statement: create_statement  */
#line 550 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3784 "parser.cpp"
    break;

  case 22: /* explainable_statement: drop_statement  */
#line 551 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3790 "parser.cpp"
    break;

  case 23: /* explainable_statement: copy_statement  */
#line 552 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3796 "parser.cpp"
    break;

  case 24: /* explainable_statement: show_statement  */
#line 553 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3802 "parser.cpp"
    break;

  case 25: /* explainable_statement: select_statement  */
#line 554 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3808 "parser.cpp"
    break;

  case 26: /* explainable_statement: delete_statement  */
#line 555 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3814 "parser.cpp"
    break;

  case 27: /* explainable_statement: update_statement  */
#line 556 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3820 "parser.cpp"
    break;

  case 28: /* explainable_statement: insert_statement  */
#line 557 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3826 "parser.cpp"
    break;

  case 29: /* explainable_statement: flush_statement  */
#line 558 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3832 "parser.cpp"
    break;

  case 30: /* explainable_statement: optimize_statement  */
#line 559 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3838 "parser.cpp"
    break;

  case 31: /* explainable_statement: command_statement  */
#line 560 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3844 "parser.cpp"
    break;

  case 32: /* explainable_statement: compact_statement  */
#line 561 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3850 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER COMMENT STRING  */
#line 568 "parser.y"
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
#line 3872 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 585 "parser.y"
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
#line 3892 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 602 "parser.y"
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
#line 3910 "parser.cpp"
    break;

  case 36: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 618 "parser.y"
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
#line 3943 "parser.cpp"
    break;

  case 37: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 647 "parser.y"
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
#line 3963 "parser.cpp"
    break;

  case 38: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list COMMENT STRING  */
#line 662 "parser.y"
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
#line 3999 "parser.cpp"
    break;

  case 39: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement COMMENT STRING  */
#line 694 "parser.y"
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
#line 4021 "parser.cpp"
    break;

  case 40: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 712 "parser.y"
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
#line 4042 "parser.cpp"
    break;

  case 41: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info  */
#line 730 "parser.y"
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
#line 4075 "parser.cpp"
    break;

  case 42: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info COMMENT STRING  */
#line 758 "parser.y"
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
#line 4110 "parser.cpp"
    break;

  case 43: /* table_element_array: table_element  */
#line 789 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 4119 "parser.cpp"
    break;

  case 44: /* table_element_array: table_element_array ',' table_element  */
#line 793 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 4128 "parser.cpp"
    break;

  case 45: /* column_def_array: table_column  */
#line 798 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 4137 "parser.cpp"
    break;

  case 46: /* column_def_array: column_def_array ',' table_column  */
#line 802 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 4146 "parser.cpp"
    break;

  case 47: /* table_element: table_column  */
#line 808 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 4154 "parser.cpp"
    break;

  case 48: /* table_element: table_constraint  */
#line 811 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 4162 "parser.cpp"
    break;

  case 49: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 818 "parser.y"
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
#line 4187 "parser.cpp"
    break;

  case 50: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 838 "parser.y"
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
#line 4214 "parser.cpp"
    break;

  case 51: /* table_column: IDENTIFIER column_type with_index_param_list default_expr COMMENT STRING  */
#line 860 "parser.y"
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
#line 4243 "parser.cpp"
    break;

  case 52: /* table_column: IDENTIFIER column_type column_constraints default_expr COMMENT STRING  */
#line 884 "parser.y"
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
#line 4273 "parser.cpp"
    break;

  case 53: /* column_type_array: column_type  */
#line 910 "parser.y"
                                {
    (yyval.column_type_array_t) = new std::vector<std::unique_ptr<infinity::ColumnType>>();
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4282 "parser.cpp"
    break;

  case 54: /* column_type_array: column_type_array ',' column_type  */
#line 914 "parser.y"
                                    {
    (yyval.column_type_array_t) = (yyvsp[-2].column_type_array_t);
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4291 "parser.cpp"
    break;

  case 55: /* column_type: BOOLEAN  */
#line 920 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4297 "parser.cpp"
    break;

  case 56: /* column_type: JSON  */
#line 921 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kJson, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4303 "parser.cpp"
    break;

  case 57: /* column_type: TINYINT  */
#line 922 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4309 "parser.cpp"
    break;

  case 58: /* column_type: SMALLINT  */
#line 923 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4315 "parser.cpp"
    break;

  case 59: /* column_type: INTEGER  */
#line 924 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4321 "parser.cpp"
    break;

  case 60: /* column_type: INT  */
#line 925 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4327 "parser.cpp"
    break;

  case 61: /* column_type: BIGINT  */
#line 926 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4333 "parser.cpp"
    break;

  case 62: /* column_type: HUGEINT  */
#line 927 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4339 "parser.cpp"
    break;

  case 63: /* column_type: FLOAT  */
#line 928 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4345 "parser.cpp"
    break;

  case 64: /* column_type: REAL  */
#line 929 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4351 "parser.cpp"
    break;

  case 65: /* column_type: DOUBLE  */
#line 930 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4357 "parser.cpp"
    break;

  case 66: /* column_type: FLOAT16  */
#line 931 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4363 "parser.cpp"
    break;

  case 67: /* column_type: BFLOAT16  */
#line 932 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4369 "parser.cpp"
    break;

  case 68: /* column_type: DATE  */
#line 933 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4375 "parser.cpp"
    break;

  case 69: /* column_type: TIME  */
#line 934 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4381 "parser.cpp"
    break;

  case 70: /* column_type: DATETIME  */
#line 935 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4387 "parser.cpp"
    break;

  case 71: /* column_type: TIMESTAMP  */
#line 936 "parser.y"
            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4393 "parser.cpp"
    break;

  case 72: /* column_type: UUID  */
#line 937 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4399 "parser.cpp"
    break;

  case 73: /* column_type: POINT  */
#line 938 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4405 "parser.cpp"
    break;

  case 74: /* column_type: LINE  */
#line 939 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4411 "parser.cpp"
    break;

  case 75: /* column_type: LSEG  */
#line 940 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4417 "parser.cpp"
    break;

  case 76: /* column_type: BOX  */
#line 941 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4423 "parser.cpp"
    break;

  case 77: /* column_type: CIRCLE  */
#line 944 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4429 "parser.cpp"
    break;

  case 78: /* column_type: VARCHAR  */
#line 946 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4435 "parser.cpp"
    break;

  case 79: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 947 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4441 "parser.cpp"
    break;

  case 80: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 948 "parser.y"
                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4447 "parser.cpp"
    break;

  case 81: /* column_type: DECIMAL  */
#line 949 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4453 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 952 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4459 "parser.cpp"
    break;

  case 83: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 953 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4465 "parser.cpp"
    break;

  case 84: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 954 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4471 "parser.cpp"
    break;

  case 85: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 955 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4477 "parser.cpp"
    break;

  case 86: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 956 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4483 "parser.cpp"
    break;

  case 87: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 957 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4489 "parser.cpp"
    break;

  case 88: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 958 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4495 "parser.cpp"
    break;

  case 89: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 959 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4501 "parser.cpp"
    break;

  case 90: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 960 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4507 "parser.cpp"
    break;

  case 91: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 961 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4513 "parser.cpp"
    break;

  case 92: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 962 "parser.y"
                                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4519 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 963 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4525 "parser.cpp"
    break;

  case 94: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 964 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4531 "parser.cpp"
    break;

  case 95: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 965 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4537 "parser.cpp"
    break;

  case 96: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 966 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4543 "parser.cpp"
    break;

  case 97: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 967 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4549 "parser.cpp"
    break;

  case 98: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 968 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4555 "parser.cpp"
    break;

  case 99: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 969 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4561 "parser.cpp"
    break;

  case 100: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 970 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4567 "parser.cpp"
    break;

  case 101: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 971 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4573 "parser.cpp"
    break;

  case 102: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 972 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4579 "parser.cpp"
    break;

  case 103: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 973 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4585 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 974 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4591 "parser.cpp"
    break;

  case 105: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 975 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4597 "parser.cpp"
    break;

  case 106: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 976 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4603 "parser.cpp"
    break;

  case 107: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 977 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4609 "parser.cpp"
    break;

  case 108: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 978 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4615 "parser.cpp"
    break;

  case 109: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 979 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4621 "parser.cpp"
    break;

  case 110: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 980 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4627 "parser.cpp"
    break;

  case 111: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 981 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4633 "parser.cpp"
    break;

  case 112: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 982 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4639 "parser.cpp"
    break;

  case 113: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 983 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4645 "parser.cpp"
    break;

  case 114: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 984 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4651 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 985 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4657 "parser.cpp"
    break;

  case 116: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 986 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4663 "parser.cpp"
    break;

  case 117: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 987 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4669 "parser.cpp"
    break;

  case 118: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 988 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4675 "parser.cpp"
    break;

  case 119: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 989 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4681 "parser.cpp"
    break;

  case 120: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 990 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4687 "parser.cpp"
    break;

  case 121: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 991 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4693 "parser.cpp"
    break;

  case 122: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 992 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4699 "parser.cpp"
    break;

  case 123: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 993 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4705 "parser.cpp"
    break;

  case 124: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 994 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4711 "parser.cpp"
    break;

  case 125: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 995 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4717 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 996 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4723 "parser.cpp"
    break;

  case 127: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 997 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4729 "parser.cpp"
    break;

  case 128: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 998 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4735 "parser.cpp"
    break;

  case 129: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 999 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4741 "parser.cpp"
    break;

  case 130: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1000 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4747 "parser.cpp"
    break;

  case 131: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1001 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4753 "parser.cpp"
    break;

  case 132: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1002 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4759 "parser.cpp"
    break;

  case 133: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1003 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4765 "parser.cpp"
    break;

  case 134: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1004 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4771 "parser.cpp"
    break;

  case 135: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1005 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4777 "parser.cpp"
    break;

  case 136: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1006 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4783 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1007 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4789 "parser.cpp"
    break;

  case 138: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1008 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4795 "parser.cpp"
    break;

  case 139: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1009 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4801 "parser.cpp"
    break;

  case 140: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4807 "parser.cpp"
    break;

  case 141: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1011 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4813 "parser.cpp"
    break;

  case 142: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1012 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4819 "parser.cpp"
    break;

  case 143: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1013 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4825 "parser.cpp"
    break;

  case 144: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1014 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4831 "parser.cpp"
    break;

  case 145: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4837 "parser.cpp"
    break;

  case 146: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4843 "parser.cpp"
    break;

  case 147: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1017 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4849 "parser.cpp"
    break;

  case 148: /* column_type: ARRAY '(' column_type ')'  */
#line 1018 "parser.y"
                            {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kArray, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_.emplace_back((yyvsp[-1].column_type_t));
}
#line 4858 "parser.cpp"
    break;

  case 149: /* column_type: TUPLE '(' column_type_array ')'  */
#line 1022 "parser.y"
                                  {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTuple, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_ = std::move(*((yyvsp[-1].column_type_array_t)));
  delete (yyvsp[-1].column_type_array_t);
}
#line 4868 "parser.cpp"
    break;

  case 150: /* column_constraints: column_constraint  */
#line 1045 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4877 "parser.cpp"
    break;

  case 151: /* column_constraints: column_constraints column_constraint  */
#line 1049 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4891 "parser.cpp"
    break;

  case 152: /* column_constraint: PRIMARY KEY  */
#line 1059 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4899 "parser.cpp"
    break;

  case 153: /* column_constraint: UNIQUE  */
#line 1062 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4907 "parser.cpp"
    break;

  case 154: /* column_constraint: NULLABLE  */
#line 1065 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4915 "parser.cpp"
    break;

  case 155: /* column_constraint: NOT NULLABLE  */
#line 1068 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4923 "parser.cpp"
    break;

  case 156: /* default_expr: DEFAULT constant_expr  */
#line 1072 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4931 "parser.cpp"
    break;

  case 157: /* default_expr: %empty  */
#line 1075 "parser.y"
                                   {
    (yyval.const_expr_t) = nullptr;
}
#line 4939 "parser.cpp"
    break;

  case 158: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1080 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4949 "parser.cpp"
    break;

  case 159: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1085 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4959 "parser.cpp"
    break;

  case 160: /* identifier_array: IDENTIFIER  */
#line 1092 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4970 "parser.cpp"
    break;

  case 161: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1098 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4981 "parser.cpp"
    break;

  case 162: /* delete_statement: DELETE FROM table_name where_clause  */
#line 1108 "parser.y"
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
#line 4998 "parser.cpp"
    break;

  case 163: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES insert_row_list  */
#line 1124 "parser.y"
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
#line 5041 "parser.cpp"
    break;

  case 164: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1162 "parser.y"
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
#line 5061 "parser.cpp"
    break;

  case 165: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1178 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 5069 "parser.cpp"
    break;

  case 166: /* optional_identifier_array: %empty  */
#line 1181 "parser.y"
         {
    (yyval.identifier_array_t) = nullptr;
}
#line 5077 "parser.cpp"
    break;

  case 167: /* explain_statement: EXPLAIN IDENTIFIER explainable_statement  */
#line 1188 "parser.y"
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
#line 5100 "parser.cpp"
    break;

  case 168: /* explain_statement: EXPLAIN explainable_statement  */
#line 1205 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 5110 "parser.cpp"
    break;

  case 169: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1214 "parser.y"
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
#line 5127 "parser.cpp"
    break;

  case 170: /* update_expr_array: update_expr  */
#line 1227 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 5136 "parser.cpp"
    break;

  case 171: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1231 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 5145 "parser.cpp"
    break;

  case 172: /* update_expr: IDENTIFIER '=' expr  */
#line 1236 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 5157 "parser.cpp"
    break;

  case 173: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1249 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 5173 "parser.cpp"
    break;

  case 174: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1262 "parser.y"
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
#line 5191 "parser.cpp"
    break;

  case 175: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1277 "parser.y"
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
#line 5209 "parser.cpp"
    break;

  case 176: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1292 "parser.y"
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
#line 5227 "parser.cpp"
    break;

  case 177: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1307 "parser.y"
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
#line 5250 "parser.cpp"
    break;

  case 178: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1330 "parser.y"
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
#line 5308 "parser.cpp"
    break;

  case 179: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1383 "parser.y"
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
#line 5368 "parser.cpp"
    break;

  case 180: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1438 "parser.y"
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
#line 5420 "parser.cpp"
    break;

  case 181: /* select_statement: select_without_paren  */
#line 1489 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5428 "parser.cpp"
    break;

  case 182: /* select_statement: select_with_paren  */
#line 1492 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5436 "parser.cpp"
    break;

  case 183: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1495 "parser.y"
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

  case 184: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1504 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5464 "parser.cpp"
    break;

  case 185: /* select_with_paren: '(' select_without_paren ')'  */
#line 1514 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5472 "parser.cpp"
    break;

  case 186: /* select_with_paren: '(' select_with_paren ')'  */
#line 1517 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5480 "parser.cpp"
    break;

  case 187: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1521 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5489 "parser.cpp"
    break;

  case 188: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1526 "parser.y"
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
#line 5520 "parser.cpp"
    break;

  case 189: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1553 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5528 "parser.cpp"
    break;

  case 190: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1556 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5536 "parser.cpp"
    break;

  case 191: /* select_clause_without_modifier: SELECT distinct expr_array highlight_clause from_clause search_clause where_clause group_by_clause having_clause  */
#line 1561 "parser.y"
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
#line 5557 "parser.cpp"
    break;

  case 192: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1578 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5565 "parser.cpp"
    break;

  case 193: /* order_by_clause: %empty  */
#line 1581 "parser.y"
                              {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5573 "parser.cpp"
    break;

  case 194: /* order_by_expr_list: order_by_expr  */
#line 1585 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5582 "parser.cpp"
    break;

  case 195: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1589 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5591 "parser.cpp"
    break;

  case 196: /* order_by_expr: expr order_by_type  */
#line 1594 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5601 "parser.cpp"
    break;

  case 197: /* order_by_type: ASC  */
#line 1600 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5609 "parser.cpp"
    break;

  case 198: /* order_by_type: DESC  */
#line 1603 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5617 "parser.cpp"
    break;

  case 199: /* order_by_type: %empty  */
#line 1606 "parser.y"
         {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5625 "parser.cpp"
    break;

  case 200: /* limit_expr: LIMIT expr  */
#line 1610 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5633 "parser.cpp"
    break;

  case 201: /* limit_expr: %empty  */
#line 1614 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5639 "parser.cpp"
    break;

  case 202: /* offset_expr: OFFSET expr  */
#line 1616 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5647 "parser.cpp"
    break;

  case 203: /* offset_expr: %empty  */
#line 1620 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5653 "parser.cpp"
    break;

  case 204: /* distinct: DISTINCT  */
#line 1622 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5661 "parser.cpp"
    break;

  case 205: /* distinct: %empty  */
#line 1625 "parser.y"
         {
    (yyval.bool_value) = false;
}
#line 5669 "parser.cpp"
    break;

  case 206: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1629 "parser.y"
                                       {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5677 "parser.cpp"
    break;

  case 207: /* highlight_clause: %empty  */
#line 1632 "parser.y"
         {
    (yyval.expr_array_t) = nullptr;
}
#line 5685 "parser.cpp"
    break;

  case 208: /* from_clause: FROM table_reference  */
#line 1636 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5693 "parser.cpp"
    break;

  case 209: /* from_clause: %empty  */
#line 1639 "parser.y"
                              {
    (yyval.table_reference_t) = nullptr;
}
#line 5701 "parser.cpp"
    break;

  case 210: /* search_clause: SEARCH sub_search_array  */
#line 1643 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5711 "parser.cpp"
    break;

  case 211: /* search_clause: %empty  */
#line 1648 "parser.y"
                                {
    (yyval.expr_t) = nullptr;
}
#line 5719 "parser.cpp"
    break;

  case 212: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1652 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5727 "parser.cpp"
    break;

  case 213: /* optional_search_filter_expr: %empty  */
#line 1655 "parser.y"
                               {
    (yyval.expr_t) = nullptr;
}
#line 5735 "parser.cpp"
    break;

  case 214: /* where_clause: WHERE expr  */
#line 1659 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5743 "parser.cpp"
    break;

  case 215: /* where_clause: %empty  */
#line 1662 "parser.y"
                               {
    (yyval.expr_t) = nullptr;
}
#line 5751 "parser.cpp"
    break;

  case 216: /* having_clause: HAVING expr  */
#line 1666 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5759 "parser.cpp"
    break;

  case 217: /* having_clause: %empty  */
#line 1669 "parser.y"
                               {
    (yyval.expr_t) = nullptr;
}
#line 5767 "parser.cpp"
    break;

  case 218: /* group_by_clause: GROUP BY expr_array  */
#line 1673 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5775 "parser.cpp"
    break;

  case 219: /* group_by_clause: %empty  */
#line 1676 "parser.y"
         {
    (yyval.expr_array_t) = nullptr;
}
#line 5783 "parser.cpp"
    break;

  case 220: /* set_operator: UNION  */
#line 1680 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5791 "parser.cpp"
    break;

  case 221: /* set_operator: UNION ALL  */
#line 1683 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5799 "parser.cpp"
    break;

  case 222: /* set_operator: INTERSECT  */
#line 1686 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5807 "parser.cpp"
    break;

  case 223: /* set_operator: EXCEPT  */
#line 1689 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5815 "parser.cpp"
    break;

  case 224: /* table_reference: table_reference_unit  */
#line 1697 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5823 "parser.cpp"
    break;

  case 225: /* table_reference: table_reference ',' table_reference_unit  */
#line 1700 "parser.y"
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
#line 5841 "parser.cpp"
    break;

  case 228: /* table_reference_name: table_name table_alias  */
#line 1717 "parser.y"
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
#line 5859 "parser.cpp"
    break;

  case 229: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1731 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5870 "parser.cpp"
    break;

  case 230: /* table_name: IDENTIFIER  */
#line 1740 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5880 "parser.cpp"
    break;

  case 231: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1745 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5892 "parser.cpp"
    break;

  case 232: /* table_alias: AS IDENTIFIER  */
#line 1754 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5902 "parser.cpp"
    break;

  case 233: /* table_alias: IDENTIFIER  */
#line 1759 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5912 "parser.cpp"
    break;

  case 234: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1764 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5923 "parser.cpp"
    break;

  case 235: /* table_alias: %empty  */
#line 1770 "parser.y"
         {
    (yyval.table_alias_t) = nullptr;
}
#line 5931 "parser.cpp"
    break;

  case 236: /* with_clause: WITH with_expr_list  */
#line 1777 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5939 "parser.cpp"
    break;

  case 237: /* with_clause: %empty  */
#line 1780 "parser.y"
                                 {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5947 "parser.cpp"
    break;

  case 238: /* with_expr_list: with_expr  */
#line 1784 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5956 "parser.cpp"
    break;

  case 239: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1787 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5965 "parser.cpp"
    break;

  case 240: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1792 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5977 "parser.cpp"
    break;

  case 241: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1804 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5989 "parser.cpp"
    break;

  case 242: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1811 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 6002 "parser.cpp"
    break;

  case 243: /* join_type: INNER  */
#line 1825 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 6010 "parser.cpp"
    break;

  case 244: /* join_type: LEFT  */
#line 1828 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 6018 "parser.cpp"
    break;

  case 245: /* join_type: RIGHT  */
#line 1831 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 6026 "parser.cpp"
    break;

  case 246: /* join_type: OUTER  */
#line 1834 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6034 "parser.cpp"
    break;

  case 247: /* join_type: FULL  */
#line 1837 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6042 "parser.cpp"
    break;

  case 248: /* join_type: CROSS  */
#line 1840 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 6050 "parser.cpp"
    break;

  case 249: /* join_type: %empty  */
#line 1843 "parser.y"
                       {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 6058 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW DATABASES  */
#line 1850 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 6067 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW TABLES  */
#line 1854 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 6076 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW TASKS  */
#line 1858 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTasks;
}
#line 6085 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW CONFIGS  */
#line 1862 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 6094 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1866 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6106 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW PROFILES  */
#line 1873 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 6115 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW BUFFER  */
#line 1877 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 6124 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW MEMINDEX  */
#line 1881 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 6133 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW QUERIES  */
#line 1885 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 6142 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1889 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 6152 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW TRANSACTIONS  */
#line 1894 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 6161 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1898 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6171 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW TRANSACTION HISTORY  */
#line 1903 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactionHistory;
}
#line 6180 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW SESSION VARIABLES  */
#line 1907 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 6189 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1911 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 6198 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1915 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6209 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1921 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6220 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1927 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6232 "parser.cpp"
    break;

  case 268: /* show_statement: SHOW TABLE table_name  */
#line 1934 "parser.y"
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
#line 6248 "parser.cpp"
    break;

  case 269: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1945 "parser.y"
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
#line 6264 "parser.cpp"
    break;

  case 270: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1956 "parser.y"
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
#line 6280 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1967 "parser.y"
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
#line 6297 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1979 "parser.y"
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
#line 6314 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1991 "parser.y"
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
#line 6332 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 2004 "parser.y"
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
#line 6351 "parser.cpp"
    break;

  case 275: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 2018 "parser.y"
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
#line 6367 "parser.cpp"
    break;

  case 276: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 2029 "parser.y"
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
#line 6387 "parser.cpp"
    break;

  case 277: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 2044 "parser.y"
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
#line 6409 "parser.cpp"
    break;

  case 278: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNKS  */
#line 2061 "parser.y"
                                                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kIndexChunks;
    if((yyvsp[-5].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.show_stmt)->schema_name_ = (yyvsp[-5].table_name_t)->schema_name_ptr_;
        free((yyvsp[-5].table_name_t)->schema_name_ptr_);
    }
    (yyval.show_stmt)->table_name_ = (yyvsp[-5].table_name_t)->table_name_ptr_;
    free((yyvsp[-5].table_name_t)->table_name_ptr_);
    delete (yyvsp[-5].table_name_t);

    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.show_stmt)->index_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));

    (yyval.show_stmt)->segment_id_ = (yyvsp[-1].long_value);
}
#line 6431 "parser.cpp"
    break;

  case 279: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 2078 "parser.y"
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
#line 6454 "parser.cpp"
    break;

  case 280: /* show_statement: SHOW LOGS  */
#line 2096 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6463 "parser.cpp"
    break;

  case 281: /* show_statement: SHOW CATALOG  */
#line 2100 "parser.y"
               {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalog;
}
#line 6472 "parser.cpp"
    break;

  case 282: /* show_statement: SHOW CATALOG STRING  */
#line 2104 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCatalogKey;
      ParserHelper::ToLower((yyvsp[0].str_value));
      (yyval.show_stmt)->var_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6484 "parser.cpp"
    break;

  case 283: /* show_statement: SHOW CATALOG TO file_path  */
#line 2111 "parser.y"
                            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogToFile;
      (yyval.show_stmt)->file_path_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6495 "parser.cpp"
    break;

  case 284: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2117 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6504 "parser.cpp"
    break;

  case 285: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2121 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6513 "parser.cpp"
    break;

  case 286: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2125 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6524 "parser.cpp"
    break;

  case 287: /* show_statement: SHOW MEMORY  */
#line 2131 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6533 "parser.cpp"
    break;

  case 288: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2135 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6542 "parser.cpp"
    break;

  case 289: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2139 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6551 "parser.cpp"
    break;

  case 290: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2143 "parser.y"
                          {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
      ParserHelper::ToLower((yyvsp[-2].str_value));
      (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
      free((yyvsp[-2].str_value));
}
#line 6563 "parser.cpp"
    break;

  case 291: /* show_statement: SHOW SNAPSHOTS  */
#line 2150 "parser.y"
                 {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListSnapshots;
}
#line 6572 "parser.cpp"
    break;

  case 292: /* show_statement: SHOW SNAPSHOT IDENTIFIER  */
#line 2154 "parser.y"
                           {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowSnapshot;
     ParserHelper::ToLower((yyvsp[0].str_value));
     (yyval.show_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6584 "parser.cpp"
    break;

  case 293: /* show_statement: SHOW CACHES  */
#line 2161 "parser.y"
              {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCaches;
}
#line 6593 "parser.cpp"
    break;

  case 294: /* show_statement: SHOW CACHE  */
#line 2165 "parser.y"
             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowCache;
}
#line 6602 "parser.cpp"
    break;

  case 295: /* show_statement: SHOW COMPACT  */
#line 2169 "parser.y"
               {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCompact;
}
#line 6611 "parser.cpp"
    break;

  case 296: /* show_statement: SHOW COMPACT NOT NULLABLE  */
#line 2173 "parser.y"
                            {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCompact;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6621 "parser.cpp"
    break;

  case 297: /* show_statement: SHOW CHECKPOINT  */
#line 2178 "parser.y"
                  {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCheckpoint;
}
#line 6630 "parser.cpp"
    break;

  case 298: /* show_statement: SHOW CHECKPOINT NOT NULLABLE  */
#line 2182 "parser.y"
                               {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCheckpoint;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6640 "parser.cpp"
    break;

  case 299: /* show_statement: SHOW CHECKPOINT LONG_VALUE  */
#line 2187 "parser.y"
                             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowCheckpoint;
     (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6650 "parser.cpp"
    break;

  case 300: /* show_statement: SHOW OPTIMIZE  */
#line 2192 "parser.y"
                {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListOptimize;
}
#line 6659 "parser.cpp"
    break;

  case 301: /* show_statement: SHOW OPTIMIZE NOT NULLABLE  */
#line 2196 "parser.y"
                             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListOptimize;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6669 "parser.cpp"
    break;

  case 302: /* show_statement: SHOW IMPORT  */
#line 2201 "parser.y"
              {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListImport;
}
#line 6678 "parser.cpp"
    break;

  case 303: /* show_statement: SHOW CLEAN  */
#line 2205 "parser.y"
             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListClean;
}
#line 6687 "parser.cpp"
    break;

  case 304: /* show_statement: SHOW CLEAN NOT NULLABLE  */
#line 2209 "parser.y"
                          {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListClean;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6697 "parser.cpp"
    break;

  case 305: /* show_statement: SHOW CLEAN LONG_VALUE  */
#line 2214 "parser.y"
                        {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowClean;
     (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6707 "parser.cpp"
    break;

  case 306: /* flush_statement: FLUSH DATA  */
#line 2223 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6716 "parser.cpp"
    break;

  case 307: /* flush_statement: FLUSH LOG  */
#line 2227 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6725 "parser.cpp"
    break;

  case 308: /* flush_statement: FLUSH BUFFER  */
#line 2231 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6734 "parser.cpp"
    break;

  case 309: /* flush_statement: FLUSH CATALOG  */
#line 2235 "parser.y"
                {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kCatalog;
}
#line 6743 "parser.cpp"
    break;

  case 310: /* optimize_statement: OPTIMIZE table_name  */
#line 2243 "parser.y"
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
#line 6758 "parser.cpp"
    break;

  case 311: /* command_statement: USE IDENTIFIER  */
#line 2257 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6769 "parser.cpp"
    break;

  case 312: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2263 "parser.y"
                                       {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6779 "parser.cpp"
    break;

  case 313: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2268 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6790 "parser.cpp"
    break;

  case 314: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2274 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6801 "parser.cpp"
    break;

  case 315: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2280 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6814 "parser.cpp"
    break;

  case 316: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2288 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6825 "parser.cpp"
    break;

  case 317: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2294 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6836 "parser.cpp"
    break;

  case 318: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2300 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6847 "parser.cpp"
    break;

  case 319: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2306 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6858 "parser.cpp"
    break;

  case 320: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2312 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6871 "parser.cpp"
    break;

  case 321: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2320 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6882 "parser.cpp"
    break;

  case 322: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2326 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6893 "parser.cpp"
    break;

  case 323: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2332 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6904 "parser.cpp"
    break;

  case 324: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2338 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6915 "parser.cpp"
    break;

  case 325: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2344 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6928 "parser.cpp"
    break;

  case 326: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2352 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6939 "parser.cpp"
    break;

  case 327: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2358 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6950 "parser.cpp"
    break;

  case 328: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON TABLE IDENTIFIER  */
#line 2364 "parser.y"
                                                 {
    ParserHelper::ToLower((yyvsp[-3].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kTable, (yyvsp[0].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[0].str_value));
}
#line 6963 "parser.cpp"
    break;

  case 329: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON DATABASE IDENTIFIER  */
#line 2372 "parser.y"
                                                    {
    ParserHelper::ToLower((yyvsp[-3].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kDatabase, (yyvsp[0].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[0].str_value));
}
#line 6976 "parser.cpp"
    break;

  case 330: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON SYSTEM  */
#line 2380 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-2].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kSystem);
    free((yyvsp[-2].str_value));
}
#line 6987 "parser.cpp"
    break;

  case 331: /* command_statement: DROP SNAPSHOT IDENTIFIER  */
#line 2386 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kDrop, infinity::SnapshotScope::kIgnore);
    free((yyvsp[0].str_value));
}
#line 6998 "parser.cpp"
    break;

  case 332: /* command_statement: RESTORE SYSTEM SNAPSHOT IDENTIFIER  */
#line 2392 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kSystem);
    free((yyvsp[0].str_value));
}
#line 7009 "parser.cpp"
    break;

  case 333: /* command_statement: RESTORE DATABASE SNAPSHOT IDENTIFIER  */
#line 2398 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kDatabase);
    free((yyvsp[0].str_value));
}
#line 7020 "parser.cpp"
    break;

  case 334: /* command_statement: RESTORE TABLE SNAPSHOT IDENTIFIER  */
#line 2404 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kTable);
    free((yyvsp[0].str_value));
}
#line 7031 "parser.cpp"
    break;

  case 335: /* command_statement: CLEAN DATA  */
#line 2410 "parser.y"
             {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::CleanupCmd>();
}
#line 7040 "parser.cpp"
    break;

  case 336: /* command_statement: DUMP INDEX IDENTIFIER ON table_name  */
#line 2414 "parser.y"
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
#line 7056 "parser.cpp"
    break;

  case 337: /* compact_statement: COMPACT TABLE table_name  */
#line 2426 "parser.y"
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
#line 7073 "parser.cpp"
    break;

  case 338: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2439 "parser.y"
                                                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 7084 "parser.cpp"
    break;

  case 339: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2445 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 7096 "parser.cpp"
    break;

  case 340: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2452 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 7109 "parser.cpp"
    break;

  case 341: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2460 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 7123 "parser.cpp"
    break;

  case 342: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2469 "parser.y"
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
#line 7138 "parser.cpp"
    break;

  case 343: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2479 "parser.y"
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
#line 7153 "parser.cpp"
    break;

  case 344: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2489 "parser.y"
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
#line 7169 "parser.cpp"
    break;

  case 345: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2500 "parser.y"
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
#line 7185 "parser.cpp"
    break;

  case 346: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2511 "parser.y"
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
#line 7202 "parser.cpp"
    break;

  case 347: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2523 "parser.y"
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
#line 7219 "parser.cpp"
    break;

  case 348: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2535 "parser.y"
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
#line 7234 "parser.cpp"
    break;

  case 349: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2545 "parser.y"
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
#line 7250 "parser.cpp"
    break;

  case 350: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2556 "parser.y"
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
#line 7267 "parser.cpp"
    break;

  case 351: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2568 "parser.y"
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
#line 7285 "parser.cpp"
    break;

  case 352: /* admin_statement: ADMIN SHOW LOGS  */
#line 2581 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 7294 "parser.cpp"
    break;

  case 353: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2585 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 7304 "parser.cpp"
    break;

  case 354: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2590 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 7314 "parser.cpp"
    break;

  case 355: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2595 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 7325 "parser.cpp"
    break;

  case 356: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2601 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 7334 "parser.cpp"
    break;

  case 357: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2605 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 7343 "parser.cpp"
    break;

  case 358: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2609 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     ParserHelper::ToLower((yyvsp[0].str_value));
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7355 "parser.cpp"
    break;

  case 359: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2616 "parser.y"
                        {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
}
#line 7364 "parser.cpp"
    break;

  case 360: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2620 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
}
#line 7373 "parser.cpp"
    break;

  case 361: /* admin_statement: ADMIN SHOW SNAPSHOT IDENTIFIER  */
#line 2624 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSnapshot;
     ParserHelper::ToLower((yyvsp[0].str_value));
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7385 "parser.cpp"
    break;

  case 362: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2631 "parser.y"
                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7396 "parser.cpp"
    break;

  case 363: /* admin_statement: ADMIN EXPORT SNAPSHOT STRING TO STRING  */
#line 2637 "parser.y"
                                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kExportSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[-2].str_value);
     (yyval.admin_stmt)->export_path_ = (yyvsp[0].str_value);
     free((yyvsp[-2].str_value));
     free((yyvsp[0].str_value));
}
#line 7409 "parser.cpp"
    break;

  case 364: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2645 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7420 "parser.cpp"
    break;

  case 365: /* admin_statement: ADMIN SHOW NODES  */
#line 2651 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 7429 "parser.cpp"
    break;

  case 366: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2655 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7440 "parser.cpp"
    break;

  case 367: /* admin_statement: ADMIN SHOW NODE  */
#line 2661 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 7449 "parser.cpp"
    break;

  case 368: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2665 "parser.y"
                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7460 "parser.cpp"
    break;

  case 369: /* admin_statement: ADMIN SET ADMIN  */
#line 2671 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
}
#line 7470 "parser.cpp"
    break;

  case 370: /* admin_statement: ADMIN SET STANDALONE  */
#line 2676 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
}
#line 7480 "parser.cpp"
    break;

  case 371: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2681 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7492 "parser.cpp"
    break;

  case 372: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2688 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7506 "parser.cpp"
    break;

  case 373: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2697 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7520 "parser.cpp"
    break;

  case 374: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2707 "parser.y"
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
#line 7535 "parser.cpp"
    break;

  case 375: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2717 "parser.y"
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
#line 7552 "parser.cpp"
    break;

  case 376: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2729 "parser.y"
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
#line 7568 "parser.cpp"
    break;

  case 377: /* alter_statement: ALTER IDENTIFIER ON table_name with_index_param_list  */
#line 2740 "parser.y"
                                                      {
    auto *ret = new infinity::AlterIndexStatement((yyvsp[-1].table_name_t)->schema_name_ptr_, (yyvsp[-1].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;

    free((yyvsp[-1].table_name_t)->schema_name_ptr_);
    free((yyvsp[-1].table_name_t)->table_name_ptr_);
    delete (yyvsp[-1].table_name_t);

    ParserHelper::ToLower((yyvsp[-3].str_value));
    ret->index_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));

    for (auto *&index_param : *(yyvsp[0].with_index_param_list_t)) {
        ret->opt_params_.emplace_back(std::unique_ptr<infinity::InitParameter>(index_param));
        index_param = nullptr;
    }
    delete (yyvsp[0].with_index_param_list_t);
}
#line 7591 "parser.cpp"
    break;

  case 378: /* check_statement: CHECK SYSTEM  */
#line 2759 "parser.y"
                               {
    (yyval.check_stmt) = new infinity::CheckStatement();
    (yyval.check_stmt)->check_type_ = infinity::CheckStmtType::kSystem;
}
#line 7600 "parser.cpp"
    break;

  case 379: /* check_statement: CHECK TABLE table_name  */
#line 2763 "parser.y"
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
#line 7616 "parser.cpp"
    break;

  case 380: /* expr_array: expr_alias  */
#line 2779 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7625 "parser.cpp"
    break;

  case 381: /* expr_array: expr_array ',' expr_alias  */
#line 2783 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7634 "parser.cpp"
    break;

  case 382: /* insert_row_list: '(' expr_array ')'  */
#line 2788 "parser.y"
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
#line 7649 "parser.cpp"
    break;

  case 383: /* insert_row_list: insert_row_list ',' '(' expr_array ')'  */
#line 2798 "parser.y"
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
#line 7664 "parser.cpp"
    break;

  case 384: /* expr_alias: expr AS IDENTIFIER  */
#line 2820 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7675 "parser.cpp"
    break;

  case 385: /* expr_alias: expr  */
#line 2826 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7683 "parser.cpp"
    break;

  case 391: /* operand: '(' expr ')'  */
#line 2836 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 7691 "parser.cpp"
    break;

  case 392: /* operand: '(' select_without_paren ')'  */
#line 2839 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7702 "parser.cpp"
    break;

  case 393: /* operand: constant_expr  */
#line 2845 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7710 "parser.cpp"
    break;

  case 404: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2861 "parser.y"
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
#line 7730 "parser.cpp"
    break;

  case 405: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')'  */
#line 2877 "parser.y"
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
#line 7752 "parser.cpp"
    break;

  case 406: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' IGNORE INDEX  */
#line 2895 "parser.y"
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
#line 7773 "parser.cpp"
    break;

  case 407: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2913 "parser.y"
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
#line 7835 "parser.cpp"
    break;

  case 408: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2971 "parser.y"
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
#line 7889 "parser.cpp"
    break;

  case 409: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 3021 "parser.y"
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
#line 7947 "parser.cpp"
    break;

  case 410: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3075 "parser.y"
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
#line 8006 "parser.cpp"
    break;

  case 411: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 3133 "parser.y"
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
#line 8035 "parser.cpp"
    break;

  case 412: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 3158 "parser.y"
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
#line 8062 "parser.cpp"
    break;

  case 413: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 3181 "parser.y"
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
#line 8087 "parser.cpp"
    break;

  case 414: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3202 "parser.y"
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
#line 8112 "parser.cpp"
    break;

  case 415: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3223 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8126 "parser.cpp"
    break;

  case 416: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3232 "parser.y"
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
#line 8142 "parser.cpp"
    break;

  case 417: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 3244 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8154 "parser.cpp"
    break;

  case 418: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
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
#line 8168 "parser.cpp"
    break;

  case 419: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3261 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 8179 "parser.cpp"
    break;

  case 420: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
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
#line 8195 "parser.cpp"
    break;

  case 421: /* sub_search: match_vector_expr  */
#line 3279 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8203 "parser.cpp"
    break;

  case 422: /* sub_search: match_text_expr  */
#line 3282 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8211 "parser.cpp"
    break;

  case 423: /* sub_search: match_tensor_expr  */
#line 3285 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8219 "parser.cpp"
    break;

  case 424: /* sub_search: match_sparse_expr  */
#line 3288 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8227 "parser.cpp"
    break;

  case 425: /* sub_search: query_expr  */
#line 3291 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8235 "parser.cpp"
    break;

  case 426: /* sub_search: fusion_expr  */
#line 3294 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8243 "parser.cpp"
    break;

  case 427: /* sub_search_array: sub_search  */
#line 3298 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 8252 "parser.cpp"
    break;

  case 428: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3302 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 8261 "parser.cpp"
    break;

  case 429: /* function_expr: IDENTIFIER '(' ')'  */
#line 3307 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 8274 "parser.cpp"
    break;

  case 430: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3315 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 8287 "parser.cpp"
    break;

  case 431: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
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
#line 8301 "parser.cpp"
    break;

  case 432: /* function_expr: YEAR '(' expr ')'  */
#line 3332 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "year";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8313 "parser.cpp"
    break;

  case 433: /* function_expr: MONTH '(' expr ')'  */
#line 3339 "parser.y"
                     {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "month";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8325 "parser.cpp"
    break;

  case 434: /* function_expr: DAY '(' expr ')'  */
#line 3346 "parser.y"
                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "day";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8337 "parser.cpp"
    break;

  case 435: /* function_expr: HOUR '(' expr ')'  */
#line 3353 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "hour";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8349 "parser.cpp"
    break;

  case 436: /* function_expr: MINUTE '(' expr ')'  */
#line 3360 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "minute";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8361 "parser.cpp"
    break;

  case 437: /* function_expr: SECOND '(' expr ')'  */
#line 3367 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "second";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8373 "parser.cpp"
    break;

  case 438: /* function_expr: operand IS NOT NULLABLE  */
#line 3374 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8385 "parser.cpp"
    break;

  case 439: /* function_expr: operand IS NULLABLE  */
#line 3381 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8397 "parser.cpp"
    break;

  case 440: /* function_expr: NOT operand  */
#line 3388 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8409 "parser.cpp"
    break;

  case 441: /* function_expr: '-' operand  */
#line 3395 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8421 "parser.cpp"
    break;

  case 442: /* function_expr: '+' operand  */
#line 3402 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8433 "parser.cpp"
    break;

  case 443: /* function_expr: operand '-' operand  */
#line 3409 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8446 "parser.cpp"
    break;

  case 444: /* function_expr: operand '+' operand  */
#line 3417 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8459 "parser.cpp"
    break;

  case 445: /* function_expr: operand '*' operand  */
#line 3425 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8472 "parser.cpp"
    break;

  case 446: /* function_expr: operand '/' operand  */
#line 3433 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8485 "parser.cpp"
    break;

  case 447: /* function_expr: operand '%' operand  */
#line 3441 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8498 "parser.cpp"
    break;

  case 448: /* function_expr: operand '=' operand  */
#line 3449 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8511 "parser.cpp"
    break;

  case 449: /* function_expr: operand EQUAL operand  */
#line 3457 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8524 "parser.cpp"
    break;

  case 450: /* function_expr: operand NOT_EQ operand  */
#line 3465 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8537 "parser.cpp"
    break;

  case 451: /* function_expr: operand '<' operand  */
#line 3473 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8550 "parser.cpp"
    break;

  case 452: /* function_expr: operand '>' operand  */
#line 3481 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8563 "parser.cpp"
    break;

  case 453: /* function_expr: operand LESS_EQ operand  */
#line 3489 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8576 "parser.cpp"
    break;

  case 454: /* function_expr: operand GREATER_EQ operand  */
#line 3497 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8589 "parser.cpp"
    break;

  case 455: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
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
#line 8624 "parser.cpp"
    break;

  case 456: /* function_expr: operand LIKE operand  */
#line 3535 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8637 "parser.cpp"
    break;

  case 457: /* function_expr: operand NOT LIKE operand  */
#line 3543 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8650 "parser.cpp"
    break;

  case 458: /* conjunction_expr: expr AND expr  */
#line 3552 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8663 "parser.cpp"
    break;

  case 459: /* conjunction_expr: expr OR expr  */
#line 3560 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8676 "parser.cpp"
    break;

  case 460: /* between_expr: operand BETWEEN operand AND operand  */
#line 3569 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 8688 "parser.cpp"
    break;

  case 461: /* in_expr: operand IN '(' expr_array ')'  */
#line 3577 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8699 "parser.cpp"
    break;

  case 462: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3583 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8710 "parser.cpp"
    break;

  case 463: /* case_expr: CASE expr case_check_array END  */
#line 3590 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8721 "parser.cpp"
    break;

  case 464: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3596 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8733 "parser.cpp"
    break;

  case 465: /* case_expr: CASE case_check_array END  */
#line 3603 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8743 "parser.cpp"
    break;

  case 466: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3608 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8754 "parser.cpp"
    break;

  case 467: /* case_check_array: WHEN expr THEN expr  */
#line 3615 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 8766 "parser.cpp"
    break;

  case 468: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3622 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 8778 "parser.cpp"
    break;

  case 469: /* cast_expr: CAST '(' expr AS column_type ')'  */
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
#line 8795 "parser.cpp"
    break;

  case 470: /* cast_expr: PARSE_JSON '(' expr ')'  */
#line 3642 "parser.y"
                          {
    auto column_type_ptr = new infinity::ColumnType{infinity::LogicalType::kJson, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
    auto [data_type_result, fail_reason] = infinity::ColumnType::GetDataTypeFromColumnType(*column_type_ptr, std::vector<std::unique_ptr<infinity::InitParameter>>{});
    delete column_type_ptr;
    if (!data_type_result) {
        yyerror(&yyloc, scanner, result, fail_reason.c_str());
        delete (yyvsp[-1].expr_t);
        YYERROR;
    }
    infinity::CastExpr* cast_expr = new infinity::CastExpr(std::move(*data_type_result));
    cast_expr->expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = cast_expr;
}
#line 8813 "parser.cpp"
    break;

  case 471: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3656 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8824 "parser.cpp"
    break;

  case 472: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3662 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8835 "parser.cpp"
    break;

  case 473: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3668 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8847 "parser.cpp"
    break;

  case 474: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3675 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8859 "parser.cpp"
    break;

  case 475: /* column_expr: IDENTIFIER  */
#line 3683 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8871 "parser.cpp"
    break;

  case 476: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3690 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8883 "parser.cpp"
    break;

  case 477: /* column_expr: '*'  */
#line 3697 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8893 "parser.cpp"
    break;

  case 478: /* column_expr: column_expr '.' '*'  */
#line 3702 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8907 "parser.cpp"
    break;

  case 479: /* constant_expr: STRING  */
#line 3712 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8917 "parser.cpp"
    break;

  case 480: /* constant_expr: TRUE  */
#line 3717 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8927 "parser.cpp"
    break;

  case 481: /* constant_expr: FALSE  */
#line 3722 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8937 "parser.cpp"
    break;

  case 482: /* constant_expr: DOUBLE_VALUE  */
#line 3727 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8947 "parser.cpp"
    break;

  case 483: /* constant_expr: LONG_VALUE  */
#line 3732 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8957 "parser.cpp"
    break;

  case 484: /* constant_expr: JSON STRING  */
#line 3737 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kJson);
    const_expr->json_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8967 "parser.cpp"
    break;

  case 485: /* constant_expr: DATE STRING  */
#line 3742 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8977 "parser.cpp"
    break;

  case 486: /* constant_expr: TIME STRING  */
#line 3747 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8987 "parser.cpp"
    break;

  case 487: /* constant_expr: DATETIME STRING  */
#line 3752 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8997 "parser.cpp"
    break;

  case 488: /* constant_expr: TIMESTAMP STRING  */
#line 3757 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9007 "parser.cpp"
    break;

  case 489: /* constant_expr: INTERVAL interval_expr  */
#line 3762 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9015 "parser.cpp"
    break;

  case 490: /* constant_expr: interval_expr  */
#line 3765 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9023 "parser.cpp"
    break;

  case 491: /* constant_expr: common_array_expr  */
#line 3768 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9031 "parser.cpp"
    break;

  case 492: /* constant_expr: curly_brackets_expr  */
#line 3771 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9039 "parser.cpp"
    break;

  case 493: /* common_array_expr: array_expr  */
#line 3775 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9047 "parser.cpp"
    break;

  case 494: /* common_array_expr: subarray_array_expr  */
#line 3778 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9055 "parser.cpp"
    break;

  case 495: /* common_array_expr: sparse_array_expr  */
#line 3781 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9063 "parser.cpp"
    break;

  case 496: /* common_array_expr: empty_array_expr  */
#line 3784 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9071 "parser.cpp"
    break;

  case 497: /* common_sparse_array_expr: sparse_array_expr  */
#line 3788 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9079 "parser.cpp"
    break;

  case 498: /* common_sparse_array_expr: array_expr  */
#line 3791 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9087 "parser.cpp"
    break;

  case 499: /* common_sparse_array_expr: empty_array_expr  */
#line 3794 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9095 "parser.cpp"
    break;

  case 500: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3798 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9103 "parser.cpp"
    break;

  case 501: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3802 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 9113 "parser.cpp"
    break;

  case 502: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3807 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9122 "parser.cpp"
    break;

  case 503: /* sparse_array_expr: long_sparse_array_expr  */
#line 3812 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9130 "parser.cpp"
    break;

  case 504: /* sparse_array_expr: double_sparse_array_expr  */
#line 3815 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9138 "parser.cpp"
    break;

  case 505: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3819 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9146 "parser.cpp"
    break;

  case 506: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3823 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 9158 "parser.cpp"
    break;

  case 507: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3830 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9169 "parser.cpp"
    break;

  case 508: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3837 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9177 "parser.cpp"
    break;

  case 509: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3841 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 9189 "parser.cpp"
    break;

  case 510: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3848 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9200 "parser.cpp"
    break;

  case 511: /* empty_array_expr: '[' ']'  */
#line 3855 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 9208 "parser.cpp"
    break;

  case 512: /* curly_brackets_expr: unclosed_curly_brackets_expr '}'  */
#line 3859 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9216 "parser.cpp"
    break;

  case 513: /* curly_brackets_expr: '{' '}'  */
#line 3862 "parser.y"
          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
}
#line 9224 "parser.cpp"
    break;

  case 514: /* unclosed_curly_brackets_expr: '{' constant_expr  */
#line 3866 "parser.y"
                                                {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
    (yyval.const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
}
#line 9233 "parser.cpp"
    break;

  case 515: /* unclosed_curly_brackets_expr: unclosed_curly_brackets_expr ',' constant_expr  */
#line 3870 "parser.y"
                                                 {
    (yyvsp[-2].const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9242 "parser.cpp"
    break;

  case 516: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3875 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 9250 "parser.cpp"
    break;

  case 517: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3879 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 9258 "parser.cpp"
    break;

  case 518: /* array_expr: long_array_expr  */
#line 3883 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9266 "parser.cpp"
    break;

  case 519: /* array_expr: double_array_expr  */
#line 3886 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9274 "parser.cpp"
    break;

  case 520: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3890 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9282 "parser.cpp"
    break;

  case 521: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3894 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9292 "parser.cpp"
    break;

  case 522: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3899 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9301 "parser.cpp"
    break;

  case 523: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3904 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9309 "parser.cpp"
    break;

  case 524: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3908 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9319 "parser.cpp"
    break;

  case 525: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3913 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9328 "parser.cpp"
    break;

  case 526: /* interval_expr: LONG_VALUE SECONDS  */
#line 3918 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9339 "parser.cpp"
    break;

  case 527: /* interval_expr: LONG_VALUE SECOND  */
#line 3924 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9350 "parser.cpp"
    break;

  case 528: /* interval_expr: LONG_VALUE MINUTES  */
#line 3930 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9361 "parser.cpp"
    break;

  case 529: /* interval_expr: LONG_VALUE MINUTE  */
#line 3936 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9372 "parser.cpp"
    break;

  case 530: /* interval_expr: LONG_VALUE HOURS  */
#line 3942 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9383 "parser.cpp"
    break;

  case 531: /* interval_expr: LONG_VALUE HOUR  */
#line 3948 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9394 "parser.cpp"
    break;

  case 532: /* interval_expr: LONG_VALUE DAYS  */
#line 3954 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9405 "parser.cpp"
    break;

  case 533: /* interval_expr: LONG_VALUE DAY  */
#line 3960 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9416 "parser.cpp"
    break;

  case 534: /* interval_expr: LONG_VALUE MONTHS  */
#line 3966 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9427 "parser.cpp"
    break;

  case 535: /* interval_expr: LONG_VALUE MONTH  */
#line 3972 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9438 "parser.cpp"
    break;

  case 536: /* interval_expr: LONG_VALUE YEARS  */
#line 3978 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9449 "parser.cpp"
    break;

  case 537: /* interval_expr: LONG_VALUE YEAR  */
#line 3984 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9460 "parser.cpp"
    break;

  case 538: /* copy_option_list: copy_option  */
#line 3995 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 9469 "parser.cpp"
    break;

  case 539: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3999 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 9478 "parser.cpp"
    break;

  case 540: /* copy_option: FORMAT JSON  */
#line 4004 "parser.y"
                          {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kFormat;
    (yyval.copy_option_t)->file_type_ = infinity::CopyFileType::kJSON;
}
#line 9488 "parser.cpp"
    break;

  case 541: /* copy_option: FORMAT IDENTIFIER  */
#line 4009 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kFormat;
    ParserHelper::ToLower((yyvsp[0].str_value));
    if (strcasecmp((yyvsp[0].str_value), "csv") == 0) {
        (yyval.copy_option_t)->file_type_ = infinity::CopyFileType::kCSV;
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
#line 9522 "parser.cpp"
    break;

  case 542: /* copy_option: DELIMITER STRING  */
#line 4038 "parser.y"
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
#line 9537 "parser.cpp"
    break;

  case 543: /* copy_option: HEADER  */
#line 4048 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 9547 "parser.cpp"
    break;

  case 544: /* copy_option: OFFSET LONG_VALUE  */
#line 4053 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 9557 "parser.cpp"
    break;

  case 545: /* copy_option: LIMIT LONG_VALUE  */
#line 4058 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 9567 "parser.cpp"
    break;

  case 546: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 4063 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 9577 "parser.cpp"
    break;

  case 547: /* file_path: STRING  */
#line 4069 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 9585 "parser.cpp"
    break;

  case 548: /* if_exists: IF EXISTS  */
#line 4073 "parser.y"
                     { (yyval.bool_value) = true; }
#line 9591 "parser.cpp"
    break;

  case 549: /* if_exists: %empty  */
#line 4074 "parser.y"
         { (yyval.bool_value) = false; }
#line 9597 "parser.cpp"
    break;

  case 550: /* if_not_exists: IF NOT EXISTS  */
#line 4076 "parser.y"
                              { (yyval.bool_value) = true; }
#line 9603 "parser.cpp"
    break;

  case 551: /* if_not_exists: %empty  */
#line 4077 "parser.y"
         { (yyval.bool_value) = false; }
#line 9609 "parser.cpp"
    break;

  case 554: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 4092 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9622 "parser.cpp"
    break;

  case 555: /* if_not_exists_info: %empty  */
#line 4100 "parser.y"
         {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 9630 "parser.cpp"
    break;

  case 556: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 4104 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9638 "parser.cpp"
    break;

  case 557: /* with_index_param_list: %empty  */
#line 4107 "parser.y"
         {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 9646 "parser.cpp"
    break;

  case 558: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 4111 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9654 "parser.cpp"
    break;

  case 559: /* optional_table_properties_list: %empty  */
#line 4114 "parser.y"
         {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 9662 "parser.cpp"
    break;

  case 560: /* index_param_list: index_param  */
#line 4118 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 9671 "parser.cpp"
    break;

  case 561: /* index_param_list: index_param_list ',' index_param  */
#line 4122 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 9680 "parser.cpp"
    break;

  case 562: /* index_param: IDENTIFIER  */
#line 4127 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9691 "parser.cpp"
    break;

  case 563: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 4133 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9706 "parser.cpp"
    break;

  case 564: /* index_param: IDENTIFIER '=' STRING  */
#line 4143 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9721 "parser.cpp"
    break;

  case 565: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 4153 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 9734 "parser.cpp"
    break;

  case 566: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 4161 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 9747 "parser.cpp"
    break;

  case 567: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 4172 "parser.y"
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
    } else if(strcmp((yyvsp[-1].str_value), "secondary") == 0){
        index_type = infinity::IndexType::kSecondary;
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
    
    // Handle secondary index cardinality parameter
    if (index_type == infinity::IndexType::kSecondary && (yyval.index_info_t)->index_param_list_ != nullptr) {
        for (auto *param : *((yyval.index_info_t)->index_param_list_)) {
            if (strcasecmp(param->param_name_.c_str(), "cardinality") == 0) {
                if (strcasecmp(param->param_value_.c_str(), "high") == 0) {
                    (yyval.index_info_t)->secondary_index_cardinality_ = infinity::SecondaryIndexCardinality::kHighCardinality;
                } else if (strcasecmp(param->param_value_.c_str(), "low") == 0) {
                    (yyval.index_info_t)->secondary_index_cardinality_ = infinity::SecondaryIndexCardinality::kLowCardinality;
                } else {
                    delete (yyvsp[0].with_index_param_list_t);
                    free((yyvsp[-4].str_value));
                    yyerror(&yyloc, scanner, result, "Invalid cardinality value. Must be 'high' or 'low'");
                    YYERROR;
                }
            }
        }
    }
    
    free((yyvsp[-4].str_value));
}
#line 9805 "parser.cpp"
    break;

  case 568: /* index_info: '(' IDENTIFIER ')'  */
#line 4225 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 9817 "parser.cpp"
    break;


#line 9821 "parser.cpp"

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

#line 4234 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

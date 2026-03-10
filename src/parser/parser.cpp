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
  YYSYMBOL_ESCAPE = 161,                   /* ESCAPE  */
  YYSYMBOL_DATA = 162,                     /* DATA  */
  YYSYMBOL_LOG = 163,                      /* LOG  */
  YYSYMBOL_BUFFER = 164,                   /* BUFFER  */
  YYSYMBOL_TRANSACTIONS = 165,             /* TRANSACTIONS  */
  YYSYMBOL_TRANSACTION = 166,              /* TRANSACTION  */
  YYSYMBOL_MEMINDEX = 167,                 /* MEMINDEX  */
  YYSYMBOL_USING = 168,                    /* USING  */
  YYSYMBOL_SESSION = 169,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 170,                   /* GLOBAL  */
  YYSYMBOL_OFF = 171,                      /* OFF  */
  YYSYMBOL_EXPORT = 172,                   /* EXPORT  */
  YYSYMBOL_CONFIGS = 173,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 174,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 175,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 176,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 177,                 /* VARIABLE  */
  YYSYMBOL_LOGS = 178,                     /* LOGS  */
  YYSYMBOL_CATALOGS = 179,                 /* CATALOGS  */
  YYSYMBOL_CATALOG = 180,                  /* CATALOG  */
  YYSYMBOL_SEARCH = 181,                   /* SEARCH  */
  YYSYMBOL_MATCH = 182,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 183,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 184,                    /* QUERY  */
  YYSYMBOL_QUERIES = 185,                  /* QUERIES  */
  YYSYMBOL_FUSION = 186,                   /* FUSION  */
  YYSYMBOL_ROWLIMIT = 187,                 /* ROWLIMIT  */
  YYSYMBOL_ADMIN = 188,                    /* ADMIN  */
  YYSYMBOL_LEADER = 189,                   /* LEADER  */
  YYSYMBOL_FOLLOWER = 190,                 /* FOLLOWER  */
  YYSYMBOL_LEARNER = 191,                  /* LEARNER  */
  YYSYMBOL_CONNECT = 192,                  /* CONNECT  */
  YYSYMBOL_STANDALONE = 193,               /* STANDALONE  */
  YYSYMBOL_NODES = 194,                    /* NODES  */
  YYSYMBOL_NODE = 195,                     /* NODE  */
  YYSYMBOL_REMOVE = 196,                   /* REMOVE  */
  YYSYMBOL_SNAPSHOT = 197,                 /* SNAPSHOT  */
  YYSYMBOL_SNAPSHOTS = 198,                /* SNAPSHOTS  */
  YYSYMBOL_RECOVER = 199,                  /* RECOVER  */
  YYSYMBOL_RESTORE = 200,                  /* RESTORE  */
  YYSYMBOL_CACHES = 201,                   /* CACHES  */
  YYSYMBOL_CACHE = 202,                    /* CACHE  */
  YYSYMBOL_PERSISTENCE = 203,              /* PERSISTENCE  */
  YYSYMBOL_OBJECT = 204,                   /* OBJECT  */
  YYSYMBOL_OBJECTS = 205,                  /* OBJECTS  */
  YYSYMBOL_FILES = 206,                    /* FILES  */
  YYSYMBOL_MEMORY = 207,                   /* MEMORY  */
  YYSYMBOL_ALLOCATION = 208,               /* ALLOCATION  */
  YYSYMBOL_HISTORY = 209,                  /* HISTORY  */
  YYSYMBOL_CHECK = 210,                    /* CHECK  */
  YYSYMBOL_CLEAN = 211,                    /* CLEAN  */
  YYSYMBOL_CHECKPOINT = 212,               /* CHECKPOINT  */
  YYSYMBOL_IMPORT = 213,                   /* IMPORT  */
  YYSYMBOL_PARSE_JSON = 214,               /* PARSE_JSON  */
  YYSYMBOL_NUMBER = 215,                   /* NUMBER  */
  YYSYMBOL_216_ = 216,                     /* '='  */
  YYSYMBOL_217_ = 217,                     /* '<'  */
  YYSYMBOL_218_ = 218,                     /* '>'  */
  YYSYMBOL_219_ = 219,                     /* '+'  */
  YYSYMBOL_220_ = 220,                     /* '-'  */
  YYSYMBOL_221_ = 221,                     /* '*'  */
  YYSYMBOL_222_ = 222,                     /* '/'  */
  YYSYMBOL_223_ = 223,                     /* '%'  */
  YYSYMBOL_224_ = 224,                     /* ';'  */
  YYSYMBOL_225_ = 225,                     /* '('  */
  YYSYMBOL_226_ = 226,                     /* ')'  */
  YYSYMBOL_227_ = 227,                     /* ','  */
  YYSYMBOL_228_ = 228,                     /* '.'  */
  YYSYMBOL_229_ = 229,                     /* ']'  */
  YYSYMBOL_230_ = 230,                     /* '['  */
  YYSYMBOL_231_ = 231,                     /* '}'  */
  YYSYMBOL_232_ = 232,                     /* '{'  */
  YYSYMBOL_233_ = 233,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 234,                 /* $accept  */
  YYSYMBOL_input_pattern = 235,            /* input_pattern  */
  YYSYMBOL_statement_list = 236,           /* statement_list  */
  YYSYMBOL_statement = 237,                /* statement  */
  YYSYMBOL_explainable_statement = 238,    /* explainable_statement  */
  YYSYMBOL_create_statement = 239,         /* create_statement  */
  YYSYMBOL_table_element_array = 240,      /* table_element_array  */
  YYSYMBOL_column_def_array = 241,         /* column_def_array  */
  YYSYMBOL_table_element = 242,            /* table_element  */
  YYSYMBOL_table_column = 243,             /* table_column  */
  YYSYMBOL_column_type_array = 244,        /* column_type_array  */
  YYSYMBOL_column_type = 245,              /* column_type  */
  YYSYMBOL_column_constraints = 246,       /* column_constraints  */
  YYSYMBOL_column_constraint = 247,        /* column_constraint  */
  YYSYMBOL_default_expr = 248,             /* default_expr  */
  YYSYMBOL_table_constraint = 249,         /* table_constraint  */
  YYSYMBOL_identifier_array = 250,         /* identifier_array  */
  YYSYMBOL_delete_statement = 251,         /* delete_statement  */
  YYSYMBOL_insert_statement = 252,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 253, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 254,        /* explain_statement  */
  YYSYMBOL_update_statement = 255,         /* update_statement  */
  YYSYMBOL_update_expr_array = 256,        /* update_expr_array  */
  YYSYMBOL_update_expr = 257,              /* update_expr  */
  YYSYMBOL_drop_statement = 258,           /* drop_statement  */
  YYSYMBOL_copy_statement = 259,           /* copy_statement  */
  YYSYMBOL_select_statement = 260,         /* select_statement  */
  YYSYMBOL_select_with_paren = 261,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 262,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 263, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 264, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 265, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 266,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 267,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 268,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 269,            /* order_by_type  */
  YYSYMBOL_limit_expr = 270,               /* limit_expr  */
  YYSYMBOL_offset_expr = 271,              /* offset_expr  */
  YYSYMBOL_distinct = 272,                 /* distinct  */
  YYSYMBOL_highlight_clause = 273,         /* highlight_clause  */
  YYSYMBOL_from_clause = 274,              /* from_clause  */
  YYSYMBOL_search_clause = 275,            /* search_clause  */
  YYSYMBOL_optional_search_filter_expr = 276, /* optional_search_filter_expr  */
  YYSYMBOL_where_clause = 277,             /* where_clause  */
  YYSYMBOL_having_clause = 278,            /* having_clause  */
  YYSYMBOL_group_by_clause = 279,          /* group_by_clause  */
  YYSYMBOL_set_operator = 280,             /* set_operator  */
  YYSYMBOL_table_reference = 281,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 282,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 283,     /* table_reference_name  */
  YYSYMBOL_table_name = 284,               /* table_name  */
  YYSYMBOL_table_alias = 285,              /* table_alias  */
  YYSYMBOL_with_clause = 286,              /* with_clause  */
  YYSYMBOL_with_expr_list = 287,           /* with_expr_list  */
  YYSYMBOL_with_expr = 288,                /* with_expr  */
  YYSYMBOL_join_clause = 289,              /* join_clause  */
  YYSYMBOL_join_type = 290,                /* join_type  */
  YYSYMBOL_show_statement = 291,           /* show_statement  */
  YYSYMBOL_flush_statement = 292,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 293,       /* optimize_statement  */
  YYSYMBOL_command_statement = 294,        /* command_statement  */
  YYSYMBOL_compact_statement = 295,        /* compact_statement  */
  YYSYMBOL_admin_statement = 296,          /* admin_statement  */
  YYSYMBOL_alter_statement = 297,          /* alter_statement  */
  YYSYMBOL_check_statement = 298,          /* check_statement  */
  YYSYMBOL_expr_array = 299,               /* expr_array  */
  YYSYMBOL_insert_row_list = 300,          /* insert_row_list  */
  YYSYMBOL_expr_alias = 301,               /* expr_alias  */
  YYSYMBOL_expr = 302,                     /* expr  */
  YYSYMBOL_operand = 303,                  /* operand  */
  YYSYMBOL_match_tensor_expr = 304,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 305,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 306,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 307,          /* match_text_expr  */
  YYSYMBOL_query_expr = 308,               /* query_expr  */
  YYSYMBOL_fusion_expr = 309,              /* fusion_expr  */
  YYSYMBOL_sub_search = 310,               /* sub_search  */
  YYSYMBOL_sub_search_array = 311,         /* sub_search_array  */
  YYSYMBOL_function_expr = 312,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 313,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 314,             /* between_expr  */
  YYSYMBOL_in_expr = 315,                  /* in_expr  */
  YYSYMBOL_case_expr = 316,                /* case_expr  */
  YYSYMBOL_case_check_array = 317,         /* case_check_array  */
  YYSYMBOL_cast_expr = 318,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 319,            /* subquery_expr  */
  YYSYMBOL_column_expr = 320,              /* column_expr  */
  YYSYMBOL_constant_expr = 321,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 322,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 323, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 324,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 325, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 326,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 327,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 328, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 329, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 330, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 331,         /* empty_array_expr  */
  YYSYMBOL_curly_brackets_expr = 332,      /* curly_brackets_expr  */
  YYSYMBOL_unclosed_curly_brackets_expr = 333, /* unclosed_curly_brackets_expr  */
  YYSYMBOL_int_sparse_ele = 334,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 335,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 336,               /* array_expr  */
  YYSYMBOL_long_array_expr = 337,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 338, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 339,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 340, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 341,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 342,         /* copy_option_list  */
  YYSYMBOL_copy_option = 343,              /* copy_option  */
  YYSYMBOL_file_path = 344,                /* file_path  */
  YYSYMBOL_if_exists = 345,                /* if_exists  */
  YYSYMBOL_if_not_exists = 346,            /* if_not_exists  */
  YYSYMBOL_semicolon = 347,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 348,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 349,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 350, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 351,         /* index_param_list  */
  YYSYMBOL_index_param = 352,              /* index_param  */
  YYSYMBOL_index_info = 353                /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 98 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 480 "parser.cpp"

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
#define YYLAST   1721

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  234
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  120
/* YYNRULES -- Number of rules.  */
#define YYNRULES  574
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1287

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   470


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
       2,     2,     2,     2,     2,     2,     2,   223,     2,     2,
     225,   226,   221,   219,   227,   220,   228,   222,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   233,   224,
     217,   216,   218,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   230,     2,   229,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   232,     2,   231,     2,     2,     2,     2,
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
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   518,   518,   522,   528,   535,   536,   537,   538,   539,
     540,   541,   542,   543,   544,   545,   546,   547,   548,   549,
     550,   552,   553,   554,   555,   556,   557,   558,   559,   560,
     561,   562,   563,   570,   587,   604,   620,   649,   664,   696,
     714,   732,   760,   791,   795,   800,   804,   810,   813,   820,
     840,   862,   886,   912,   916,   922,   923,   924,   925,   926,
     927,   928,   929,   930,   931,   932,   933,   934,   935,   936,
     937,   938,   939,   940,   941,   942,   943,   946,   948,   949,
     950,   951,   954,   955,   956,   957,   958,   959,   960,   961,
     962,   963,   964,   965,   966,   967,   968,   969,   970,   971,
     972,   973,   974,   975,   976,   977,   978,   979,   980,   981,
     982,   983,   984,   985,   986,   987,   988,   989,   990,   991,
     992,   993,   994,   995,   996,   997,   998,   999,  1000,  1001,
    1002,  1003,  1004,  1005,  1006,  1007,  1008,  1009,  1010,  1011,
    1012,  1013,  1014,  1015,  1016,  1017,  1018,  1019,  1020,  1024,
    1047,  1051,  1061,  1064,  1067,  1070,  1074,  1077,  1082,  1087,
    1094,  1100,  1110,  1126,  1164,  1180,  1183,  1190,  1207,  1216,
    1229,  1233,  1238,  1251,  1264,  1279,  1294,  1309,  1332,  1385,
    1440,  1491,  1494,  1497,  1506,  1516,  1519,  1523,  1528,  1555,
    1558,  1563,  1580,  1583,  1587,  1591,  1596,  1602,  1605,  1608,
    1612,  1615,  1618,  1621,  1624,  1627,  1631,  1634,  1638,  1641,
    1645,  1650,  1654,  1657,  1661,  1664,  1668,  1671,  1675,  1678,
    1682,  1685,  1688,  1691,  1699,  1702,  1717,  1717,  1719,  1733,
    1742,  1747,  1756,  1761,  1766,  1772,  1779,  1782,  1786,  1789,
    1794,  1806,  1813,  1827,  1830,  1833,  1836,  1839,  1842,  1845,
    1852,  1856,  1860,  1864,  1868,  1875,  1879,  1883,  1887,  1891,
    1896,  1900,  1905,  1909,  1913,  1917,  1923,  1929,  1936,  1947,
    1958,  1969,  1981,  1993,  2006,  2020,  2031,  2046,  2063,  2080,
    2098,  2102,  2106,  2113,  2119,  2123,  2127,  2133,  2137,  2141,
    2145,  2152,  2156,  2163,  2167,  2171,  2175,  2180,  2184,  2189,
    2194,  2198,  2203,  2207,  2211,  2216,  2225,  2229,  2233,  2237,
    2245,  2259,  2265,  2270,  2276,  2282,  2290,  2296,  2302,  2308,
    2314,  2322,  2328,  2334,  2340,  2346,  2354,  2360,  2366,  2374,
    2382,  2388,  2394,  2400,  2406,  2412,  2416,  2428,  2441,  2447,
    2454,  2462,  2471,  2481,  2491,  2502,  2513,  2525,  2537,  2547,
    2558,  2570,  2583,  2587,  2592,  2597,  2603,  2607,  2614,  2618,
    2622,  2629,  2633,  2637,  2644,  2650,  2658,  2664,  2668,  2674,
    2678,  2684,  2689,  2694,  2701,  2710,  2720,  2730,  2742,  2753,
    2772,  2776,  2792,  2796,  2801,  2811,  2833,  2839,  2843,  2844,
    2845,  2846,  2847,  2849,  2852,  2858,  2861,  2862,  2863,  2864,
    2865,  2866,  2867,  2868,  2869,  2870,  2874,  2890,  2908,  2926,
    2984,  3034,  3088,  3146,  3171,  3194,  3215,  3236,  3245,  3257,
    3264,  3274,  3280,  3292,  3295,  3298,  3301,  3304,  3307,  3311,
    3315,  3320,  3328,  3336,  3345,  3352,  3359,  3366,  3373,  3380,
    3387,  3394,  3401,  3408,  3415,  3422,  3430,  3438,  3446,  3454,
    3462,  3470,  3478,  3486,  3494,  3502,  3510,  3518,  3548,  3560,
    3578,  3590,  3609,  3617,  3626,  3634,  3640,  3647,  3653,  3660,
    3665,  3672,  3679,  3687,  3699,  3713,  3719,  3725,  3732,  3740,
    3747,  3754,  3759,  3769,  3774,  3779,  3784,  3789,  3794,  3799,
    3804,  3809,  3814,  3819,  3822,  3825,  3828,  3832,  3835,  3838,
    3841,  3845,  3848,  3851,  3855,  3859,  3864,  3869,  3872,  3876,
    3880,  3887,  3894,  3898,  3905,  3912,  3916,  3919,  3923,  3927,
    3932,  3936,  3940,  3943,  3947,  3951,  3956,  3961,  3965,  3970,
    3975,  3981,  3987,  3993,  3999,  4005,  4011,  4017,  4023,  4029,
    4035,  4041,  4052,  4056,  4061,  4066,  4095,  4105,  4110,  4115,
    4120,  4126,  4130,  4131,  4133,  4134,  4136,  4137,  4149,  4157,
    4161,  4164,  4168,  4171,  4175,  4179,  4184,  4190,  4200,  4210,
    4218,  4229,  4282,  4289,  4295
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
  "OR", "EXTRACT", "LIKE", "ESCAPE", "DATA", "LOG", "BUFFER",
  "TRANSACTIONS", "TRANSACTION", "MEMINDEX", "USING", "SESSION", "GLOBAL",
  "OFF", "EXPORT", "CONFIGS", "CONFIG", "PROFILES", "VARIABLES",
  "VARIABLE", "LOGS", "CATALOGS", "CATALOG", "SEARCH", "MATCH", "MAXSIM",
  "QUERY", "QUERIES", "FUSION", "ROWLIMIT", "ADMIN", "LEADER", "FOLLOWER",
  "LEARNER", "CONNECT", "STANDALONE", "NODES", "NODE", "REMOVE",
  "SNAPSHOT", "SNAPSHOTS", "RECOVER", "RESTORE", "CACHES", "CACHE",
  "PERSISTENCE", "OBJECT", "OBJECTS", "FILES", "MEMORY", "ALLOCATION",
  "HISTORY", "CHECK", "CLEAN", "CHECKPOINT", "IMPORT", "PARSE_JSON",
  "NUMBER", "'='", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "';'",
  "'('", "')'", "','", "'.'", "']'", "'['", "'}'", "'{'", "':'", "$accept",
  "input_pattern", "statement_list", "statement", "explainable_statement",
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

#define YYPACT_NINF (-794)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-560)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    1247,    67,   223,   133,    87,   257,    87,   327,   260,  1310,
     232,   294,   306,   290,   389,    87,   408,   226,   121,   377,
     252,   267,   -38,   444,   222,  -794,  -794,  -794,  -794,  -794,
    -794,  -794,  -794,   376,  -794,  -794,   469,  -794,  -794,  -794,
    -794,  -794,  -794,  -794,  -794,   423,   423,   423,   423,   206,
     484,    87,   450,   450,   450,   450,   450,   499,   235,   502,
      87,   -24,   510,   527,   536,  1293,  -794,  -794,  -794,  -794,
    -794,  -794,  -794,   376,  -794,  -794,  -794,  -794,  -794,   347,
     419,   494,   577,    87,  -794,  -794,  -794,  -794,  -794,    22,
    -794,   -80,   238,  -794,   579,  -794,  -794,    66,   570,  -794,
     591,  -794,  -794,  -794,   -31,    86,    59,    68,  -794,   534,
      87,    87,   595,  -794,  -794,  -794,  -794,  -794,  -794,   545,
     383,  -794,   609,   428,   431,   315,   296,   440,   617,   449,
     563,   473,   475,   491,    87,  -794,  -794,   439,   479,  -794,
    1220,  -794,   625,  -794,  -794,     7,   644,  -794,   643,   638,
     709,    87,    87,    87,   710,   652,   653,   497,   642,   714,
      87,    87,    87,   721,  -794,   723,   725,   664,   728,   728,
     784,    57,    63,    76,  -794,   507,   602,   603,  -794,   436,
    -794,  -794,  -794,   733,  -794,   734,  -794,  -794,   728,  -794,
    -794,   737,  -794,  -794,  -794,  -794,  -794,   606,  -794,   610,
      87,   437,  -794,   683,   521,   408,   728,  -794,   743,  -794,
     582,  -794,   748,  -794,   746,  -794,  -794,   751,  -794,   750,
    -794,   753,   755,  -794,   757,   704,   758,   578,   776,   777,
     778,  -794,  -794,  -794,  -794,  -794,     7,  -794,  -794,  -794,
     784,   729,   715,   711,   649,   -36,  -794,   497,  -794,    87,
     398,   783,   357,  -794,  -794,  -794,  -794,  -794,   730,  -794,
     575,   -35,  -794,   784,  -794,  -794,   716,   718,   568,  -794,
    -794,  1422,   873,   572,   580,   581,   794,   799,   800,   811,
     813,  -794,  -794,   815,   594,   598,   600,   601,   605,   615,
     623,   420,   624,   628,   630,   980,   980,  -794,   670,    16,
      19,   179,  -794,    40,   691,  -794,  -794,  -794,  -794,  -794,
    -794,  -794,  -794,  -794,  -794,  -794,  -794,  -794,   613,  -794,
    -794,  -794,  -117,  -794,  -794,    89,  -794,   152,  -794,  -794,
     -41,  -794,  -794,   207,  -794,   229,  -794,  -794,  -794,  -794,
    -794,  -794,  -794,  -794,  -794,  -794,  -794,  -794,  -794,  -794,
    -794,  -794,  -794,  -794,  -794,   824,   822,  -794,  -794,  -794,
    -794,  -794,  -794,  -794,  -794,  -794,   764,   785,   804,   782,
      87,   469,  -794,  -794,  -794,   856,  -794,    50,  -794,   855,
    -794,  -794,   786,   291,  -794,   858,  -794,  -794,  -794,   639,
     640,   -39,   784,   784,   795,  -794,   864,   -38,    77,   816,
     645,   869,   870,  -794,  -794,   285,   650,  -794,    87,   784,
     725,  -794,   363,   655,   657,   415,  -794,  -794,  -794,  -794,
    -794,  -794,  -794,  -794,  -794,  -794,  -794,  -794,   980,   658,
    1194,   807,   784,   784,   -40,   472,  -794,  -794,  -794,  -794,
    -794,  1422,  -794,   784,   784,   784,   784,   784,   784,   889,
     669,   671,   672,   673,   895,   897,   784,   245,   245,   677,
     182,  -794,   682,  -794,  -794,  -794,  -794,  -794,  -794,   827,
     784,   902,   784,   784,    -8,   681,     2,   980,   980,   980,
     980,   980,   980,   980,   980,   980,   980,   980,   980,   980,
     980,    17,   686,  -794,   911,  -794,   912,  -794,    12,  -794,
     913,  -794,   915,  -794,   883,   492,   702,  -794,   706,   708,
     926,  -794,   712,  -794,   929,  -794,    52,   932,   769,   771,
    -794,  -794,  -794,   784,   865,   713,  -794,    21,   363,   784,
    -794,  -794,    56,  1435,   812,   719,   320,  -794,  -794,  -794,
     -38,  1069,   810,  -794,  -794,  -794,   945,   784,   722,  -794,
     363,  -794,   263,   263,   784,  -794,   323,   807,   790,   727,
      69,    72,   489,  -794,   784,   784,   191,   196,   198,   200,
     212,   215,   875,   784,    33,   784,   950,   732,   343,   217,
    -794,  -794,   573,   728,  -794,  -794,  -794,   798,   735,   980,
     670,   825,  -794,   819,   819,   167,   167,   891,   144,   819,
     167,   167,   245,   245,  -794,  -794,  -794,  -794,  -794,  -794,
     736,  -794,   745,  -794,  -794,  -794,  -794,   955,   956,  -794,
     962,   783,   964,  -794,  -794,  -794,   974,  -794,  -794,   978,
     983,   761,    23,   808,   784,  -794,  -794,  -794,   363,   986,
    -794,  -794,  -794,  -794,  -794,  -794,  -794,  -794,  -794,  -794,
    -794,  -794,   766,  -794,  -794,  -794,  -794,  -794,  -794,  -794,
    -794,  -794,  -794,  -794,  -794,   772,   793,   797,   801,   802,
     805,   806,   809,   251,   829,   783,   970,    77,   376,   367,
    1194,   803,   998,  -794,   387,   831,  1014,  1019,  1029,    54,
    -794,  1041,   391,  -794,   393,   397,  -794,   823,  -794,  1435,
     784,  -794,   784,   123,   149,  -794,  -794,  -794,  -794,  -794,
    -794,   980,   -72,  -794,   399,    82,   833,    42,   826,  -794,
    1046,  -794,  -794,  -794,   984,   670,   616,   832,   416,  -794,
     980,   980,  1055,  1059,   599,  1020,   851,   424,  -794,   426,
     430,  -794,  1062,  -794,  -794,   -38,   842,   468,  -794,    85,
    -794,   380,   664,  -794,  -794,  1065,  1435,  1435,   901,  1042,
    1091,  1267,  1291,  1334,   944,   947,  -794,  -794,   268,  -794,
     946,   783,   432,   853,   951,  -794,   917,   921,  -794,  -794,
     784,  -794,  -794,  -794,  -794,  -794,  -794,  -794,   263,  -794,
    -794,  -794,   871,   363,   177,  -794,   784,   478,   784,   686,
     849,  1077,   863,   784,  -794,   874,   866,   876,   434,   980,
    -794,  -794,  1194,  -794,  1089,  -794,  1090,   585,  -794,   962,
    -794,  -794,   964,   390,    34,    23,  1044,  -794,  -794,  -794,
    -794,  -794,  -794,  1047,  -794,  1096,  -794,  -794,  -794,  -794,
    -794,  -794,  -794,  -794,   879,  1063,   441,   890,   451,  -794,
     894,   896,   898,   900,   904,   905,   916,   918,   919,  1036,
     923,   924,   928,   930,   931,   933,   934,   936,   938,   940,
    1045,   941,   943,   948,   949,   952,   953,   954,   960,   961,
     965,  1051,   966,   968,   975,   976,   977,   987,   989,   994,
     995,   996,  1060,   997,   999,  1011,  1012,  1013,  1015,  1017,
    1018,  1025,  1038,  1081,  1039,  1052,  1053,  1054,  1057,  1058,
    1064,  1066,  1068,  1070,  1085,  1083,  -794,  -794,    12,  -794,
    1021,  1056,   453,  -794,   962,  1221,  1238,  1244,   455,  -794,
    -794,  -794,   363,  -794,   168,  1084,    28,  1087,  -794,  -794,
    -794,  1088,  1202,  1043,   363,  -794,   263,  -794,  -794,  -794,
    -794,  -794,  -794,  -794,  -794,  -794,  -794,  -794,  1265,  -794,
      85,   468,    23,    23,  1049,   380,  1219,  1224,  -794,  1276,
    -794,  -794,  1435,  1280,  1281,  1286,  1290,  1302,  1306,  1311,
    1312,  1314,  1097,  1320,  1322,  1323,  1324,  1325,  1326,  1328,
    1329,  1330,  1331,  1111,  1333,  1335,  1338,  1346,  1347,  1349,
    1350,  1351,  1357,  1358,  1140,  1362,  1363,  1364,  1369,  1370,
    1371,  1372,  1373,  1374,  1375,  1160,  1382,  1385,  1387,  1389,
    1393,  1394,  1395,  1396,  1397,  1398,  1178,  1400,  1401,  1403,
    1415,  1416,  1417,  1418,  1428,  1430,  1431,  1211,  1436,  -794,
    1439,  1440,  -794,   460,  -794,   764,   764,  -794,  1442,  1444,
    1445,    73,  1225,  -794,   464,  1446,  -794,  -794,  1388,   783,
    -794,   784,   784,  -794,  1227,  -794,  1228,  1229,  1230,  1233,
    1234,  1236,  1237,  1240,  1241,  1458,  1242,  1243,  1245,  1252,
    1255,  1256,  1260,  1261,  1263,  1266,  1464,  1270,  1271,  1272,
    1273,  1274,  1275,  1277,  1279,  1283,  1284,  1467,  1288,  1289,
    1294,  1316,  1327,  1348,  1352,  1353,  1354,  1355,  1485,  1356,
    1359,  1360,  1361,  1365,  1366,  1367,  1368,  1376,  1377,  1496,
    1378,  1379,  1380,  1381,  1383,  1384,  1386,  1390,  1391,  1392,
    1500,  1399,  -794,  -794,  -794,  -794,  -794,  1292,  1402,  1404,
     863,  -794,  -794,   467,   784,   466,   761,   363,  -794,  -794,
    -794,  -794,  -794,  -794,  -794,  -794,  -794,  -794,  1406,  -794,
    -794,  -794,  -794,  -794,  -794,  -794,  -794,  -794,  -794,  1407,
    -794,  -794,  -794,  -794,  -794,  -794,  -794,  -794,  -794,  -794,
    1408,  -794,  -794,  -794,  -794,  -794,  -794,  -794,  -794,  -794,
    -794,  1409,  -794,  -794,  -794,  -794,  -794,  -794,  -794,  -794,
    -794,  -794,  1410,  -794,  -794,  -794,  -794,  -794,  -794,  -794,
    -794,  -794,  -794,  1411,  -794,  1512,  1545,    45,  1412,  1413,
    1544,  1546,  -794,  -794,  -794,   363,  -794,  -794,  -794,  -794,
    -794,  -794,  -794,  1414,  1419,   863,   764,  -794,  1569,   618,
      81,  1421,  1572,  1423,  -794,   660,  1571,  -794,   863,   764,
     863,   -23,  1577,  -794,  1537,  1424,  -794,  1425,  1549,  1550,
    -794,  -794,  -794,    -5,   146,  -794,  1405,  1551,  1556,  -794,
    1557,  1558,  1595,  -794,  1420,  -794,  1427,  1429,  1596,  1597,
     764,  1432,  1433,  -794,   764,  -794,  -794
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     237,     0,     0,     0,     0,     0,     0,     0,   237,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   237,     0,   557,     3,     5,    10,    12,    13,
      11,     6,     7,     9,   182,   181,     0,     8,    14,    15,
      16,    17,    18,    19,    20,   555,   555,   555,   555,   555,
       0,     0,   553,   553,   553,   553,   553,     0,   230,     0,
       0,     0,     0,     0,     0,   237,   168,    21,    26,    28,
      27,    22,    23,    25,    24,    29,    30,    31,    32,     0,
     295,   300,     0,     0,   251,   252,   250,   256,   260,     0,
     257,     0,     0,   253,     0,   255,   280,   281,     0,   258,
       0,   291,   293,   294,     0,   287,   303,   297,   302,     0,
       0,     0,     0,   306,   307,   308,   309,   311,   310,     0,
     236,   238,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   380,   335,     0,     0,     1,
     237,     2,   220,   222,   223,     0,   205,   187,   193,     0,
       0,     0,     0,     0,     0,     0,     0,   166,     0,     0,
       0,     0,     0,     0,   331,     0,     0,   215,     0,     0,
       0,     0,     0,     0,   167,     0,     0,     0,   267,   268,
     261,   262,   263,     0,   264,     0,   254,   282,     0,   259,
     292,     0,   285,   284,   288,   289,   305,     0,   299,     0,
       0,     0,   337,     0,     0,     0,     0,   361,     0,   371,
       0,   372,     0,   356,     0,   358,   359,     0,   352,     0,
     367,   369,     0,   362,     0,     0,     0,     0,     0,     0,
       0,   381,   186,   185,     4,   221,     0,   183,   184,   204,
       0,     0,   201,     0,    34,     0,    35,   166,   558,     0,
       0,     0,   237,   552,   173,   175,   174,   176,     0,   231,
       0,   215,   170,     0,   162,   551,     0,     0,   479,   483,
     486,   487,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   484,   485,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   481,   237,     0,
       0,     0,   382,   387,   388,   402,   400,   403,   401,   404,
     405,   397,   392,   391,   390,   398,   399,   389,   396,   395,
     495,   498,     0,   499,   507,     0,   508,     0,   500,   496,
       0,   497,   522,     0,   523,     0,   494,   315,   317,   316,
     313,   314,   320,   322,   321,   318,   319,   325,   327,   326,
     323,   324,   290,   296,   301,     0,     0,   270,   269,   275,
     265,   266,   283,   286,   304,   298,   561,     0,     0,     0,
       0,     0,   239,   312,   364,     0,   357,   353,   360,     0,
     368,   363,     0,     0,   370,     0,   333,   334,   332,     0,
       0,   207,     0,     0,   203,   554,     0,   237,     0,     0,
       0,     0,     0,   330,   160,     0,     0,   164,     0,     0,
       0,   169,   214,     0,     0,     0,   531,   530,   533,   532,
     535,   534,   537,   536,   539,   538,   541,   540,     0,     0,
     442,   237,     0,     0,     0,     0,   488,   489,   490,   491,
     492,     0,   493,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   444,   443,     0,
       0,   528,   525,   515,   505,   510,   513,   517,   518,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   504,     0,   509,     0,   512,     0,   516,
       0,   524,     0,   527,   276,   271,     0,   379,     0,     0,
       0,   336,     0,   373,     0,   354,     0,     0,     0,     0,
     366,   190,   189,     0,   209,   192,   194,   199,   200,     0,
     188,    33,    37,     0,     0,     0,     0,    43,    47,    48,
     237,     0,    41,   329,   328,   165,     0,     0,   163,   177,
     172,   171,     0,     0,     0,   431,     0,   237,     0,     0,
       0,     0,     0,   469,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   213,     0,     0,
     394,   393,     0,     0,   383,   386,   462,   463,     0,     0,
     237,     0,   441,   451,   452,   455,   456,     0,   458,   450,
     453,   454,   446,   445,   447,   448,   449,   480,   482,   506,
       0,   511,     0,   514,   519,   526,   529,     0,     0,   272,
       0,     0,     0,   376,   240,   355,     0,   338,   365,     0,
       0,   206,     0,   211,     0,   197,   198,   196,   202,     0,
      55,    56,    59,    60,    57,    58,    61,    62,    78,    63,
      65,    64,    81,    68,    69,    70,    66,    67,    71,    72,
      73,    74,    75,    76,    77,     0,     0,     0,     0,     0,
       0,     0,     0,   561,     0,     0,   563,     0,    40,   479,
       0,   397,     0,   161,     0,     0,     0,     0,     0,     0,
     547,     0,     0,   542,     0,     0,   432,     0,   475,     0,
       0,   467,     0,     0,     0,   439,   438,   437,   436,   435,
     434,     0,     0,   479,     0,     0,     0,     0,     0,   421,
       0,   474,   521,   520,     0,   237,   460,     0,     0,   440,
       0,     0,     0,     0,   277,   273,   566,     0,   564,     0,
       0,    45,   339,   374,   375,   237,   208,   224,   226,   235,
     227,     0,   215,   195,    39,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   153,   154,   157,   150,
     157,     0,     0,     0,    36,    44,   572,   573,    42,   384,
       0,   549,   548,   546,   545,   544,   550,   180,     0,   178,
     433,   476,     0,   471,     0,   470,     0,     0,     0,     0,
       0,     0,   213,     0,   419,     0,     0,     0,     0,     0,
     477,   465,   464,   459,     0,   278,     0,     0,   560,     0,
     378,   377,     0,     0,     0,     0,     0,   244,   245,   246,
     247,   243,   248,     0,   233,     0,   228,   425,   423,   426,
     424,   427,   428,   429,   210,   219,     0,     0,     0,    53,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   155,   152,     0,   151,
      50,    49,     0,   159,     0,     0,     0,     0,     0,   543,
     473,   468,   472,   457,     0,     0,     0,     0,   501,   503,
     502,   213,     0,     0,   212,   422,     0,   478,   466,   461,
     279,   274,   567,   568,   570,   569,   565,    46,     0,   340,
     235,   225,     0,     0,   232,     0,     0,   217,    80,     0,
     148,   149,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   156,
       0,     0,   158,     0,    38,   561,   561,   385,     0,     0,
       0,     0,     0,   420,     0,   341,   229,   241,     0,     0,
     430,     0,     0,   191,     0,    54,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    52,    51,   562,   571,   574,     0,     0,   213,
     213,   417,   179,     0,     0,     0,   218,   216,    79,    85,
      86,    83,    84,    87,    88,    89,    90,    91,     0,    82,
     129,   130,   127,   128,   131,   132,   133,   134,   135,     0,
     126,    96,    97,    94,    95,    98,    99,   100,   101,   102,
       0,    93,   107,   108,   105,   106,   109,   110,   111,   112,
     113,     0,   104,   140,   141,   138,   139,   142,   143,   144,
     145,   146,     0,   137,   118,   119,   116,   117,   120,   121,
     122,   123,   124,     0,   115,     0,     0,     0,     0,     0,
       0,     0,   343,   342,   348,   242,   234,    92,   136,   103,
     114,   147,   125,   213,     0,   213,   561,   418,   349,   344,
       0,     0,     0,     0,   416,     0,     0,   345,   213,   561,
     213,   561,     0,   350,   346,     0,   412,     0,     0,     0,
     415,   351,   347,   561,   406,   414,     0,     0,     0,   411,
       0,     0,     0,   410,     0,   408,     0,     0,     0,     0,
     561,     0,     0,   413,   561,   407,   409
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -794,  -794,  -794,  1461,  1543,    96,  -794,  -794,   937,  -585,
    -794,  -685,  -794,   843,   845,  -794,  -611,    99,   134,  1426,
    -794,   160,  -794,  1203,   254,   287,    -6,  1598,   -19,  1248,
    1434,  -100,  -794,  -794,   988,  -794,  -794,  -794,  -794,  -794,
    -794,  -794,  -793,  -253,  -794,  -794,  -794,  -794,   796,  -255,
      43,   663,  -794,  -794,  1437,  -794,  -794,   303,   328,   342,
     351,   368,  -794,  -794,  -794,  -239,  -794,  1154,  -263,  -266,
    -724,  -713,  -712,  -710,  -709,  -707,   661,  -794,  1086,  -794,
    -794,  -794,  -794,  1206,  -794,  -794,  1074,  -296,  -292,  -794,
    -794,  -794,   828,  -794,  -794,  -794,  -794,   844,  -794,  -794,
    1149,  1157,   854,  -794,  -794,  -794,  -794,  1438,  -542,   868,
    -156,   549,   560,  -794,  -794,  -668,  -794,   738,   838,  -794
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    23,    24,    25,    66,    26,   536,   740,   537,   538,
     848,   673,   768,   769,   920,   539,   405,    27,    28,   252,
      29,    30,   261,   262,    31,    32,    33,    34,    35,   147,
     237,   148,   242,   525,   526,   637,   394,   530,   240,   524,
     633,   752,   718,   264,  1063,   967,   145,   746,   747,   748,
     749,   836,    36,   120,   121,   750,   833,    37,    38,    39,
      40,    41,    42,    43,    44,   301,   548,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   843,   844,   311,   312,
     313,   314,   315,   435,   316,   317,   318,   319,   320,   937,
     321,   322,   323,   324,   325,   326,   327,   328,   329,   330,
     465,   466,   331,   332,   333,   334,   335,   336,   692,   693,
     266,   159,   150,   141,   155,   507,   774,   737,   738,   542
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     412,   391,    73,   138,   468,   770,   430,   464,   411,   943,
     739,   694,   434,   267,   792,   146,   269,   270,   271,   397,
     607,   461,   462,   269,   270,   271,    58,   837,   180,   457,
     458,   263,   362,   461,   462,   460,   713,   741,   838,   839,
      16,   840,   841,   523,   842,   238,   802,    59,   433,    61,
     373,  1235,   168,   169,   142,   506,   143,   784,   118,    73,
     337,   144,   338,   339,   772,   196,   342,   588,   343,   344,
     187,   847,   849,   506,   198,   591,   142,  1140,   143,   347,
     533,   348,   349,   144,   626,   472,   473,  1248,   834,   514,
      58,   635,   636,   627,   157,   471,   182,   183,   515,    45,
      46,    47,   276,   167,    67,    48,    49,    68,   803,   276,
     492,   803,   493,  1258,   277,   278,   279,   472,   473,   340,
     280,   277,   278,   279,   699,   345,   179,   280,   123,   527,
     528,  1267,   197,   124,   592,   125,   390,   126,   350,   803,
     835,   199,    69,   188,   785,  1259,   550,   803,  1052,   281,
     282,   283,   589,   201,   202,   798,   281,   282,   283,   700,
     922,    67,   430,  1268,    68,    52,    53,    54,    70,   560,
     561,    55,    56,   191,   192,   193,   556,   231,   472,   473,
     566,   567,   568,   569,   570,   571,   498,    22,   470,   398,
     499,   639,   410,   579,   245,   246,   247,   472,   473,    69,
     609,   170,   614,   255,   256,   257,   534,   795,   535,   586,
     587,   593,   594,   595,   596,   597,   598,   599,   600,   601,
     602,   603,   604,   605,   606,    70,   472,   473,   341,   472,
     473,   181,   236,   407,   346,   109,   796,   957,   608,   472,
     473,   837,   299,   366,   300,   463,   299,   351,   745,   299,
     467,   300,   838,   839,   297,   840,   841,   463,   842,    51,
     960,   931,    71,    65,    50,   110,   638,     1,  -559,     2,
       3,     4,     5,     6,     7,   680,     9,   476,   149,   459,
     472,   473,  1270,    11,   631,   134,    12,  1065,    13,    14,
      15,   194,   400,   127,   195,    72,  -560,  -560,   479,   480,
     476,   703,   704,   135,  -560,   731,   472,   473,   684,   800,
     712,    74,   715,   128,  1271,   695,   494,   129,   495,    71,
     130,  -560,  -560,   726,   764,   472,   473,   111,   212,   506,
      57,   686,   687,    60,   472,   473,    75,   213,    16,   472,
     473,   764,   688,   689,   690,   112,  1218,  1219,   472,   473,
      76,   728,    72,   472,   473,   472,   473,   472,   473,    77,
    -560,   484,   485,   486,   487,   488,   489,   490,    74,   472,
     473,   527,   472,   473,   472,   473,    78,  1135,  1136,   496,
     765,   497,   766,   767,  -560,  -560,   486,   487,   488,   489,
     490,   532,   117,    75,   406,  1048,   142,   765,   143,   766,
     767,   122,   918,   144,  1054,   469,   470,    76,   581,   131,
     132,   119,   559,   511,   184,   185,    77,   705,   268,   269,
     270,   271,   706,   958,   707,   959,   708,   724,   133,   136,
     401,   402,    17,    78,   500,    16,   501,   793,   709,   794,
    1241,   710,  1243,   721,   139,   797,   140,   367,  1145,   403,
     691,   549,   113,   114,   115,  1255,   502,  1257,   503,   214,
      19,   368,   369,   165,   812,   813,   488,   489,   490,   215,
     116,    21,   216,   217,   218,   355,   219,   146,   356,   357,
     554,   518,   519,   358,   359,    22,   808,   156,   272,   273,
     220,   221,   176,   222,   223,   149,    62,    63,   274,   845,
     275,    64,   164,   209,   210,   276,  1220,   935,   211,  1221,
    1222,   545,   546,   171,  1223,  1224,   166,   277,   278,   279,
     472,   473,   158,   280,   826,  -249,   827,   828,   829,   830,
     172,   831,   832,   932,   678,   934,   618,   619,   697,   173,
     944,   928,   450,   949,   451,   452,   676,   677,   453,   696,
     470,   558,   281,   282,   283,   284,   563,   285,   564,   286,
     565,   287,   291,   288,   292,   289,   293,   177,  1244,   719,
     720,   727,   175,   701,   290,   702,   189,   565,   722,   723,
     178,  1256,   186,  1260,   268,   269,   270,   271,   952,   953,
     954,   955,   415,   776,   190,  1269,   200,   291,   203,   292,
     204,   293,   160,   161,   162,   163,   151,   152,   153,   154,
     205,   476,  1283,   779,   470,   206,  1286,   787,   788,   789,
     788,   225,  1039,   790,   470,   207,   799,   491,   208,   294,
     477,   478,   479,   480,   295,   296,   297,   224,   482,   227,
     298,   555,   811,   470,   226,   299,   235,   300,   814,   815,
     818,   819,   820,   546,   272,   273,   821,   822,   923,   546,
     948,   470,  1246,  1247,   274,   232,   275,   968,   969,   433,
     228,   276,   229,   268,   269,   270,   271,   971,   972,  1042,
     546,  1047,   470,   277,   278,   279,  1134,   819,   230,   280,
    1142,   788,  1226,   546,   483,   484,   485,   486,   487,   488,
     489,   490,  1252,  1253,   933,   233,   807,  1057,  1058,   239,
     241,   243,   244,   248,   249,   250,   253,   254,   281,   282,
     283,   284,   251,   285,   258,   286,   259,   287,   260,   288,
     263,   289,   265,   352,   353,   354,   360,   361,   364,   824,
     290,   363,   365,   272,   273,   370,   371,   374,    16,   476,
     375,   376,   377,   274,   378,   275,   379,   380,   381,   383,
     276,   382,   384,   291,   474,   292,   475,   293,  -560,  -560,
     479,   480,   277,   278,   279,   385,  -560,   809,   280,   386,
     387,   388,   392,   393,   396,   395,   404,   268,   269,   270,
     271,   409,   408,   415,   413,   294,   414,   431,   436,  1147,
     295,   296,   297,   437,   438,   432,   298,   281,   282,   283,
     284,   299,   285,   300,   286,   439,   287,   440,   288,   443,
     289,   441,  1146,   444,   476,   445,   446,   504,   505,   290,
     447,   508,  -560,   484,   485,   486,   487,   488,   489,   490,
     448,   491,   506,   477,   478,   479,   480,   481,   449,   454,
     509,   482,   291,   455,   292,   456,   293,   272,   273,   510,
     513,   516,   520,   517,   529,   521,   522,   274,   531,   275,
     541,   540,   543,   544,   276,   547,   268,   269,   270,   271,
     552,  1225,   553,   557,   294,    16,   277,   278,   279,   295,
     296,   297,   280,   572,   573,   298,   574,   575,   576,   577,
     299,   578,   300,   580,   583,   585,   590,   483,   484,   485,
     486,   487,   488,   489,   490,   582,   299,   610,   612,   615,
     616,   281,   282,   283,   284,   617,   285,   620,   286,   623,
     287,   621,   288,   622,   289,   625,   628,   629,   624,   630,
     634,   632,   674,   290,   675,   682,   428,   429,   683,   685,
     589,   711,   476,   698,   716,   472,   274,   729,   275,   717,
     725,   734,   735,   276,   558,   736,   291,   533,   292,   732,
     293,  -560,  -560,   479,   480,   277,   278,   279,   733,  -560,
     742,   280,   743,   268,   269,   270,   271,   744,   470,   751,
     754,   755,   850,   851,   852,   853,   854,   756,   294,   855,
     856,   773,   778,   295,   296,   297,   857,   858,   859,   298,
     281,   282,   283,   284,   299,   285,   300,   286,   757,   287,
     781,   288,   758,   289,   476,   782,   759,   760,   860,   777,
     761,   762,   290,   783,   763,  -560,   484,   485,   486,   487,
     488,   489,   490,   477,   478,   479,   480,   786,   730,   791,
     805,   482,   804,   428,   771,   291,   780,   292,   810,   293,
     801,   723,   806,   274,   722,   275,   816,   817,   823,   825,
     276,   846,   679,   269,   270,   271,   916,   917,   924,   936,
     918,   941,   277,   278,   279,   926,   925,   294,   280,   927,
     942,   946,   295,   296,   297,   950,   951,   930,   298,   964,
     945,   962,   947,   299,   963,   300,   965,   483,   484,   485,
     486,   487,   488,   489,   490,   966,   970,   281,   282,   283,
     284,   973,   285,   974,   286,   975,   287,   976,   288,   982,
     289,   977,   978,   861,   862,   863,   864,   865,   993,   290,
     866,   867,   428,   979,  1004,   980,   981,   868,   869,   870,
     983,   984,   274,  1015,   275,   985,  1040,   986,   987,   276,
     988,   989,   291,   990,   292,   991,   293,   992,   994,   871,
     995,   277,   278,   279,  1026,   996,   997,   280,  1037,   998,
     999,  1000,   872,   873,   874,   875,   876,  1001,  1002,   877,
     878,  1041,  1003,  1005,   294,  1006,   879,   880,   881,   295,
     296,   297,  1007,  1008,  1009,   298,   281,   282,   283,   284,
     299,   285,   300,   286,  1010,   287,  1011,   288,   882,   289,
    -556,  1012,  1013,  1014,  1016,  1044,  1017,     1,   290,     2,
       3,     4,     5,     6,     7,     8,     9,    10,  1018,  1019,
    1020,  1045,  1021,    11,  1022,  1023,    12,  1046,    13,    14,
      15,   291,  1024,   292,     1,   293,     2,     3,     4,     5,
       6,     7,     8,     9,    10,  1025,  1027,   558,   803,  1053,
      11,  1055,  1061,    12,  1059,    13,    14,    15,  1062,  1028,
    1029,  1030,  1064,   294,  1031,  1032,  1066,  1067,   295,   296,
     297,  1033,  1068,  1034,   298,  1035,  1069,  1036,    16,   299,
       1,   300,     2,     3,     4,     5,     6,     7,  1070,     9,
    1038,  1049,  1071,    79,  1050,  1051,    11,  1072,  1073,    12,
    1074,    13,    14,    15,  1075,    16,  1076,   476,  1077,  1078,
    1079,  1080,  1081,    80,  1082,  1083,  1084,  1085,  1086,  1087,
      81,  1088,    82,    83,  1089,    84,   477,   478,   479,   480,
      85,    86,  1090,  1091,   482,  1092,  1093,  1094,   883,   884,
     885,   886,   887,  1095,  1096,   888,   889,  1097,  1098,  1099,
    1100,    16,   890,   891,   892,  1101,  1102,  1103,  1104,  1105,
    1106,  1107,   894,   895,   896,   897,   898,  1108,  1109,   899,
     900,  1110,    17,  1111,   893,  1112,   901,   902,   903,  1113,
    1114,  1115,  1116,  1117,  1118,  1119,  1120,  1121,    18,  1122,
     483,   484,   485,   486,   487,   488,   489,   490,   904,    17,
      19,  1123,  1124,  1125,  1126,   905,   906,   907,   908,   909,
      20,    21,   910,   911,  1127,    18,  1128,  1129,  1130,   912,
     913,   914,  1131,  1132,  1133,    22,  1137,    19,  1138,  1139,
    1144,  1141,  1143,  1148,  1149,  1150,  1151,    20,    21,  1152,
    1153,   915,  1154,  1155,  1158,    17,  1156,  1157,  1159,  1160,
    1169,  1161,    22,  1180,    87,    88,    89,    90,  1162,    91,
      92,  1163,  1164,    93,    94,    95,  1165,  1166,    96,  1167,
      97,  1191,  1168,    19,    98,    99,  1170,  1171,  1172,  1173,
    1174,  1175,  1202,  1176,    21,  1177,  1213,   100,   101,  1178,
    1179,   102,   103,   104,  1181,  1182,  1233,   105,    22,  1215,
    1183,   106,   107,   108,   640,   641,   642,   643,   644,   645,
     646,   647,   648,   649,   650,   651,   652,   653,   654,   655,
     656,   657,  1184,   658,   659,   660,   661,   662,   663,  1234,
    1238,   664,  1239,  1185,   665,   666,   667,   668,   669,   670,
     671,   672,   416,   417,   418,   419,   420,   421,   422,   423,
     424,   425,   426,   427,  1186,  1245,  1250,  1254,  1187,  1188,
    1189,  1190,  1192,  1261,  1262,  1193,  1194,  1195,  1265,  1266,
    1273,  1196,  1197,  1198,  1199,  1274,  1275,  1276,  1277,  1281,
    1282,   234,  1200,  1201,  1203,  1204,  1205,  1206,   174,  1207,
    1208,   919,  1209,   551,   775,   921,  1210,  1211,  1212,   512,
     137,   961,   753,  1056,   584,  1214,  1060,   681,   938,  1216,
    1272,  1217,  1227,  1228,  1229,  1230,  1231,  1232,  1236,  1237,
     562,  1240,   372,   611,   939,  1278,  1242,  1249,   714,  1251,
    1263,  1264,  1279,   613,   940,  1280,   929,   956,  1284,  1285,
       0,     0,  1043,     0,     0,     0,     0,     0,     0,     0,
     389,     0,     0,   399,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   442
};

static const yytype_int16 yycheck[] =
{
     263,   240,     8,    22,   300,   673,   272,   299,   261,   802,
     621,   553,   275,   169,   699,     8,     4,     5,     6,    55,
       3,     5,     6,     4,     5,     6,     3,   751,     6,   295,
     296,    66,   188,     5,     6,   298,     3,   622,   751,   751,
      78,   751,   751,    82,   751,   145,     4,     4,    88,     6,
     206,     6,    76,    77,    20,    78,    22,     3,    15,    65,
       3,    27,     5,     6,   675,     6,     3,    75,     5,     6,
       4,   756,   757,    78,     6,    73,    20,     4,    22,     3,
       3,     5,     6,    27,    32,   157,   158,     6,     3,    39,
       3,    70,    71,    41,    51,    55,   176,   177,    48,    32,
      33,    34,    90,    60,     8,    38,    39,     8,    66,    90,
     227,    66,   229,   136,   102,   103,   104,   157,   158,    62,
     108,   102,   103,   104,    55,    62,    83,   108,     7,   392,
     393,   136,    73,    12,   132,    14,   236,    16,    62,    66,
      55,    73,     8,    77,    90,   168,   409,    66,   941,   137,
     138,   139,   160,   110,   111,   227,   137,   138,   139,    87,
     771,    65,   428,   168,    65,    32,    33,    34,     8,   432,
     433,    38,    39,   204,   205,   206,   415,   134,   157,   158,
     443,   444,   445,   446,   447,   448,   227,   225,   227,   225,
     231,   135,   227,   456,   151,   152,   153,   157,   158,    65,
     492,   225,   498,   160,   161,   162,   129,    84,   131,   472,
     473,   477,   478,   479,   480,   481,   482,   483,   484,   485,
     486,   487,   488,   489,   490,    65,   157,   158,   171,   157,
     158,   209,   225,   252,   171,     3,    87,   822,   221,   157,
     158,   965,   230,   200,   232,   229,   230,   171,   225,   230,
     231,   232,   965,   965,   221,   965,   965,   229,   965,    36,
     226,    84,     8,     3,   197,    33,   529,     7,    62,     9,
      10,    11,    12,    13,    14,   541,    16,   133,    72,   298,
     157,   158,   136,    23,   523,    33,    26,   972,    28,    29,
      30,   205,   249,   172,   208,     8,   152,   153,   154,   155,
     133,   564,   565,    51,   160,   161,   157,   158,   547,   227,
     573,     8,   575,   192,   168,   554,   227,   196,   229,    65,
     199,   154,   155,   589,    73,   157,   158,    33,    32,    78,
     197,    68,    69,    76,   157,   158,     8,    41,    78,   157,
     158,    73,    79,    80,    81,    39,  1139,  1140,   157,   158,
       8,   590,    65,   157,   158,   157,   158,   157,   158,     8,
     216,   217,   218,   219,   220,   221,   222,   223,    65,   157,
     158,   634,   157,   158,   157,   158,     8,  1045,  1046,   227,
     129,   229,   131,   132,   217,   218,   219,   220,   221,   222,
     223,   397,     3,    65,    37,   227,    20,   129,    22,   131,
     132,   175,   134,    27,   946,   226,   227,    65,   226,    32,
      33,     3,   431,   370,   176,   177,    65,   226,     3,     4,
       5,     6,   226,    33,   226,    35,   226,   583,    51,   162,
      32,    33,   172,    65,   227,    78,   229,   700,   226,   702,
    1233,   226,  1235,   226,     0,   711,   224,    10,  1059,    51,
     187,   408,   162,   163,   164,  1248,   227,  1250,   229,   163,
     200,    24,    25,   228,   730,   731,   221,   222,   223,   173,
     180,   211,   176,   177,   178,    39,   180,     8,    42,    43,
      65,   190,   191,    47,    48,   225,   725,     3,    73,    74,
     194,   195,    73,   197,   198,    72,   169,   170,    83,   752,
      85,   174,     3,   188,   189,    90,    39,   799,   193,    42,
      43,   226,   227,     3,    47,    48,    14,   102,   103,   104,
     157,   158,    72,   108,    56,    57,    58,    59,    60,    61,
       3,    63,    64,   796,   540,   798,    44,    45,   557,     3,
     803,   780,   122,   809,   124,   125,   226,   227,   128,   226,
     227,    73,   137,   138,   139,   140,    84,   142,    86,   144,
      88,   146,   182,   148,   184,   150,   186,    73,  1236,   226,
     227,   590,   225,    84,   159,    86,     6,    88,     5,     6,
       3,  1249,     3,  1251,     3,     4,     5,     6,     3,     4,
       5,     6,   225,   226,     3,  1263,    62,   182,     3,   184,
      55,   186,    53,    54,    55,    56,    46,    47,    48,    49,
     227,   133,  1280,   226,   227,     6,  1284,   226,   227,   226,
     227,     4,   918,   226,   227,   197,   227,   228,   197,   214,
     152,   153,   154,   155,   219,   220,   221,   197,   160,    76,
     225,   226,   226,   227,   195,   230,    21,   232,    49,    50,
     226,   227,   226,   227,    73,    74,   226,   227,   226,   227,
     226,   227,    44,    45,    83,   226,    85,   226,   227,    88,
     197,    90,   197,     3,     4,     5,     6,   226,   227,   226,
     227,   226,   227,   102,   103,   104,   226,   227,   197,   108,
     226,   227,   226,   227,   216,   217,   218,   219,   220,   221,
     222,   223,    42,    43,   226,   226,   725,   962,   963,    65,
      67,    73,     3,     3,    62,    62,    74,     3,   137,   138,
     139,   140,   225,   142,     3,   144,     3,   146,     3,   148,
      66,   150,     4,   226,   132,   132,     3,     3,   132,   745,
     159,     4,   132,    73,    74,    62,   225,     4,    78,   133,
     168,     3,     6,    83,     3,    85,     6,     4,     3,    55,
      90,     4,     4,   182,    73,   184,    75,   186,   152,   153,
     154,   155,   102,   103,   104,   197,   160,   161,   108,     3,
       3,     3,    53,    68,   135,    74,     3,     3,     4,     5,
       6,   216,    62,   225,    78,   214,    78,   225,     4,  1062,
     219,   220,   221,     4,     4,   225,   225,   137,   138,   139,
     140,   230,   142,   232,   144,     4,   146,     4,   148,   225,
     150,     6,  1061,   225,   133,   225,   225,     3,     6,   159,
     225,    46,   216,   217,   218,   219,   220,   221,   222,   223,
     225,   228,    78,   152,   153,   154,   155,   156,   225,   225,
      46,   160,   182,   225,   184,   225,   186,    73,    74,    77,
       4,     6,     4,    77,    69,   226,   226,    83,     4,    85,
     225,    55,     3,     3,    90,   225,     3,     4,     5,     6,
     225,  1144,   225,   225,   214,    78,   102,   103,   104,   219,
     220,   221,   108,     4,   225,   225,   225,   225,   225,     4,
     230,     4,   232,   226,    77,     3,   225,   216,   217,   218,
     219,   220,   221,   222,   223,   233,   230,     6,     6,     6,
       5,   137,   138,   139,   140,    42,   142,   225,   144,     3,
     146,   225,   148,   225,   150,     6,     4,   168,   226,   168,
     227,    76,   130,   159,   225,   135,    73,    74,     3,   227,
     160,    76,   133,   226,     4,   157,    83,   132,    85,   227,
     225,     6,     6,    90,    73,     3,   182,     3,   184,   233,
     186,   152,   153,   154,   155,   102,   103,   104,   233,   160,
       6,   108,     4,     3,     4,     5,     6,     4,   227,   181,
       4,   225,    91,    92,    93,    94,    95,   225,   214,    98,
      99,    31,     4,   219,   220,   221,   105,   106,   107,   225,
     137,   138,   139,   140,   230,   142,   232,   144,   225,   146,
       6,   148,   225,   150,   133,     6,   225,   225,   127,   226,
     225,   225,   159,     4,   225,   216,   217,   218,   219,   220,
     221,   222,   223,   152,   153,   154,   155,     6,   157,   226,
       4,   160,   226,    73,   225,   182,   225,   184,   226,   186,
     227,     6,    78,    83,     5,    85,    46,   216,     6,   227,
      90,     6,     3,     4,     5,     6,   132,   130,   225,   230,
     134,     4,   102,   103,   104,   168,   135,   214,   108,   168,
     227,   225,   219,   220,   221,     6,     6,   226,   225,     3,
     226,    57,   226,   230,    57,   232,   227,   216,   217,   218,
     219,   220,   221,   222,   223,    52,   226,   137,   138,   139,
     140,   227,   142,   227,   144,   227,   146,   227,   148,    93,
     150,   227,   227,    91,    92,    93,    94,    95,    93,   159,
      98,    99,    73,   227,    93,   227,   227,   105,   106,   107,
     227,   227,    83,    93,    85,   227,   135,   227,   227,    90,
     227,   227,   182,   227,   184,   227,   186,   227,   227,   127,
     227,   102,   103,   104,    93,   227,   227,   108,    93,   227,
     227,   227,    91,    92,    93,    94,    95,   227,   227,    98,
      99,   135,   227,   227,   214,   227,   105,   106,   107,   219,
     220,   221,   227,   227,   227,   225,   137,   138,   139,   140,
     230,   142,   232,   144,   227,   146,   227,   148,   127,   150,
       0,   227,   227,   227,   227,     4,   227,     7,   159,     9,
      10,    11,    12,    13,    14,    15,    16,    17,   227,   227,
     227,     3,   227,    23,   227,   227,    26,     3,    28,    29,
      30,   182,   227,   184,     7,   186,     9,    10,    11,    12,
      13,    14,    15,    16,    17,   227,   227,    73,    66,   226,
      23,     6,    53,    26,   225,    28,    29,    30,    54,   227,
     227,   227,     6,   214,   227,   227,     6,     6,   219,   220,
     221,   227,     6,   227,   225,   227,     6,   227,    78,   230,
       7,   232,     9,    10,    11,    12,    13,    14,     6,    16,
     227,   227,     6,     3,   227,   227,    23,     6,     6,    26,
       6,    28,    29,    30,   227,    78,     6,   133,     6,     6,
       6,     6,     6,    23,     6,     6,     6,     6,   227,     6,
      30,     6,    32,    33,     6,    35,   152,   153,   154,   155,
      40,    41,     6,     6,   160,     6,     6,     6,    91,    92,
      93,    94,    95,     6,     6,    98,    99,   227,     6,     6,
       6,    78,   105,   106,   107,     6,     6,     6,     6,     6,
       6,     6,    91,    92,    93,    94,    95,   227,     6,    98,
      99,     6,   172,     6,   127,     6,   105,   106,   107,     6,
       6,     6,     6,     6,     6,   227,     6,     6,   188,     6,
     216,   217,   218,   219,   220,   221,   222,   223,   127,   172,
     200,     6,     6,     6,     6,    91,    92,    93,    94,    95,
     210,   211,    98,    99,     6,   188,     6,     6,   227,   105,
     106,   107,     6,     4,     4,   225,     4,   200,     4,     4,
      62,   226,     6,   226,   226,   226,   226,   210,   211,   226,
     226,   127,   226,   226,     6,   172,   226,   226,   226,   226,
       6,   226,   225,     6,   164,   165,   166,   167,   226,   169,
     170,   226,   226,   173,   174,   175,   226,   226,   178,   226,
     180,     6,   226,   200,   184,   185,   226,   226,   226,   226,
     226,   226,     6,   226,   211,   226,     6,   197,   198,   226,
     226,   201,   202,   203,   226,   226,     4,   207,   225,   227,
     226,   211,   212,   213,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   226,   108,   109,   110,   111,   112,   113,     4,
       6,   116,     6,   226,   119,   120,   121,   122,   123,   124,
     125,   126,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   226,     6,     4,     6,   226,   226,
     226,   226,   226,     6,    47,   226,   226,   226,    39,    39,
      39,   226,   226,   226,   226,    39,    39,    39,     3,     3,
       3,   140,   226,   226,   226,   226,   226,   226,    65,   226,
     226,   768,   226,   410,   677,   770,   226,   226,   226,   371,
      22,   825,   634,   960,   470,   226,   965,   541,   800,   227,
     225,   227,   226,   226,   226,   226,   226,   226,   226,   226,
     434,   227,   205,   494,   800,   225,   227,   226,   574,   226,
     226,   226,   225,   496,   800,   226,   788,   819,   226,   226,
      -1,    -1,   924,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     236,    -1,    -1,   247,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   283
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    23,    26,    28,    29,    30,    78,   172,   188,   200,
     210,   211,   225,   235,   236,   237,   239,   251,   252,   254,
     255,   258,   259,   260,   261,   262,   286,   291,   292,   293,
     294,   295,   296,   297,   298,    32,    33,    34,    38,    39,
     197,    36,    32,    33,    34,    38,    39,   197,     3,   284,
      76,   284,   169,   170,   174,     3,   238,   239,   251,   252,
     255,   258,   259,   260,   291,   292,   293,   294,   295,     3,
      23,    30,    32,    33,    35,    40,    41,   164,   165,   166,
     167,   169,   170,   173,   174,   175,   178,   180,   184,   185,
     197,   198,   201,   202,   203,   207,   211,   212,   213,     3,
      33,    33,    39,   162,   163,   164,   180,     3,   284,     3,
     287,   288,   175,     7,    12,    14,    16,   172,   192,   196,
     199,    32,    33,    51,    33,    51,   162,   261,   262,     0,
     224,   347,    20,    22,    27,   280,     8,   263,   265,    72,
     346,   346,   346,   346,   346,   348,     3,   284,    72,   345,
     345,   345,   345,   345,     3,   228,    14,   284,    76,    77,
     225,     3,     3,     3,   238,   225,    73,    73,     3,   284,
       6,   209,   176,   177,   176,   177,     3,     4,    77,     6,
       3,   204,   205,   206,   205,   208,     6,    73,     6,    73,
      62,   284,   284,     3,    55,   227,     6,   197,   197,   188,
     189,   193,    32,    41,   163,   173,   176,   177,   178,   180,
     194,   195,   197,   198,   197,     4,   195,    76,   197,   197,
     197,   284,   226,   226,   237,    21,   225,   264,   265,    65,
     272,    67,   266,    73,     3,   284,   284,   284,     3,    62,
      62,   225,   253,    74,     3,   284,   284,   284,     3,     3,
       3,   256,   257,    66,   277,     4,   344,   344,     3,     4,
       5,     6,    73,    74,    83,    85,    90,   102,   103,   104,
     108,   137,   138,   139,   140,   142,   144,   146,   148,   150,
     159,   182,   184,   186,   214,   219,   220,   221,   225,   230,
     232,   299,   301,   302,   303,   304,   305,   306,   307,   308,
     309,   312,   313,   314,   315,   316,   318,   319,   320,   321,
     322,   324,   325,   326,   327,   328,   329,   330,   331,   332,
     333,   336,   337,   338,   339,   340,   341,     3,     5,     6,
      62,   171,     3,     5,     6,    62,   171,     3,     5,     6,
      62,   171,   226,   132,   132,    39,    42,    43,    47,    48,
       3,     3,   344,     4,   132,   132,   284,    10,    24,    25,
      62,   225,   288,   344,     4,   168,     3,     6,     3,     6,
       4,     3,     4,    55,     4,   197,     3,     3,     3,   264,
     265,   299,    53,    68,   270,    74,   135,    55,   225,   253,
     284,    32,    33,    51,     3,   250,    37,   262,    62,   216,
     227,   277,   302,    78,    78,   225,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,    73,    74,
     303,   225,   225,    88,   302,   317,     4,     4,     4,     4,
       4,     6,   341,   225,   225,   225,   225,   225,   225,   225,
     122,   124,   125,   128,   225,   225,   225,   303,   303,   262,
     302,     5,     6,   229,   322,   334,   335,   231,   321,   226,
     227,    55,   157,   158,    73,    75,   133,   152,   153,   154,
     155,   156,   160,   216,   217,   218,   219,   220,   221,   222,
     223,   228,   227,   229,   227,   229,   227,   229,   227,   231,
     227,   229,   227,   229,     3,     6,    78,   349,    46,    46,
      77,   284,   263,     4,    39,    48,     6,    77,   190,   191,
       4,   226,   226,    82,   273,   267,   268,   302,   302,    69,
     271,     4,   260,     3,   129,   131,   240,   242,   243,   249,
      55,   225,   353,     3,     3,   226,   227,   225,   300,   284,
     302,   257,   225,   225,    65,   226,   299,   225,    73,   262,
     302,   302,   317,    84,    86,    88,   302,   302,   302,   302,
     302,   302,     4,   225,   225,   225,   225,     4,     4,   302,
     226,   226,   233,    77,   301,     3,   302,   302,    75,   160,
     225,    73,   132,   303,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   303,   303,   303,   303,     3,   221,   322,
       6,   334,     6,   335,   321,     6,     5,    42,    44,    45,
     225,   225,   225,     3,   226,     6,    32,    41,     4,   168,
     168,   299,    76,   274,   227,    70,    71,   269,   302,   135,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   108,   109,
     110,   111,   112,   113,   116,   119,   120,   121,   122,   123,
     124,   125,   126,   245,   130,   225,   226,   227,   260,     3,
     303,   312,   135,     3,   299,   227,    68,    69,    79,    80,
      81,   187,   342,   343,   342,   299,   226,   262,   226,    55,
      87,    84,    86,   302,   302,   226,   226,   226,   226,   226,
     226,    76,   302,     3,   320,   302,     4,   227,   276,   226,
     227,   226,     5,     6,   344,   225,   303,   262,   299,   132,
     157,   161,   233,   233,     6,     6,     3,   351,   352,   250,
     241,   243,     6,     4,     4,   225,   281,   282,   283,   284,
     289,   181,   275,   268,     4,   225,   225,   225,   225,   225,
     225,   225,   225,   225,    73,   129,   131,   132,   246,   247,
     349,   225,   250,    31,   350,   242,   226,   226,     4,   226,
     225,     6,     6,     4,     3,    90,     6,   226,   227,   226,
     226,   226,   245,   302,   302,    84,    87,   303,   227,   227,
     227,   227,     4,    66,   226,     4,    78,   262,   299,   161,
     226,   226,   303,   303,    49,    50,    46,   216,   226,   227,
     226,   226,   227,     6,   260,   227,    56,    58,    59,    60,
      61,    63,    64,   290,     3,    55,   285,   304,   305,   306,
     307,   308,   309,   310,   311,   277,     6,   245,   244,   245,
      91,    92,    93,    94,    95,    98,    99,   105,   106,   107,
     127,    91,    92,    93,    94,    95,    98,    99,   105,   106,
     107,   127,    91,    92,    93,    94,    95,    98,    99,   105,
     106,   107,   127,    91,    92,    93,    94,    95,    98,    99,
     105,   106,   107,   127,    91,    92,    93,    94,    95,    98,
      99,   105,   106,   107,   127,    91,    92,    93,    94,    95,
      98,    99,   105,   106,   107,   127,   132,   130,   134,   247,
     248,   248,   250,   226,   225,   135,   168,   168,   299,   343,
     226,    84,   302,   226,   302,   322,   230,   323,   326,   331,
     336,     4,   227,   276,   302,   226,   225,   226,   226,   303,
       6,     6,     3,     4,     5,     6,   352,   243,    33,    35,
     226,   282,    57,    57,     3,   227,    52,   279,   226,   227,
     226,   226,   227,   227,   227,   227,   227,   227,   227,   227,
     227,   227,    93,   227,   227,   227,   227,   227,   227,   227,
     227,   227,   227,    93,   227,   227,   227,   227,   227,   227,
     227,   227,   227,   227,    93,   227,   227,   227,   227,   227,
     227,   227,   227,   227,   227,    93,   227,   227,   227,   227,
     227,   227,   227,   227,   227,   227,    93,   227,   227,   227,
     227,   227,   227,   227,   227,   227,   227,    93,   227,   321,
     135,   135,   226,   351,     4,     3,     3,   226,   227,   227,
     227,   227,   276,   226,   342,     6,   285,   283,   283,   225,
     310,    53,    54,   278,     6,   245,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   227,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   227,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   227,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   227,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   227,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     227,     6,     4,     4,   226,   349,   349,     4,     4,     4,
       4,   226,   226,     6,    62,   250,   299,   302,   226,   226,
     226,   226,   226,   226,   226,   226,   226,   226,     6,   226,
     226,   226,   226,   226,   226,   226,   226,   226,   226,     6,
     226,   226,   226,   226,   226,   226,   226,   226,   226,   226,
       6,   226,   226,   226,   226,   226,   226,   226,   226,   226,
     226,     6,   226,   226,   226,   226,   226,   226,   226,   226,
     226,   226,     6,   226,   226,   226,   226,   226,   226,   226,
     226,   226,   226,     6,   226,   227,   227,   227,   276,   276,
      39,    42,    43,    47,    48,   302,   226,   226,   226,   226,
     226,   226,   226,     4,     4,     6,   226,   226,     6,     6,
     227,   276,   227,   276,   349,     6,    44,    45,     6,   226,
       4,   226,    42,    43,     6,   276,   349,   276,   136,   168,
     349,     6,    47,   226,   226,    39,    39,   136,   168,   349,
     136,   168,   225,    39,    39,    39,    39,     3,   225,   225,
     226,     3,     3,   349,   226,   226,   349
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   234,   235,   236,   236,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   239,   239,   239,   239,   239,   239,   239,
     239,   239,   239,   240,   240,   241,   241,   242,   242,   243,
     243,   243,   243,   244,   244,   245,   245,   245,   245,   245,
     245,   245,   245,   245,   245,   245,   245,   245,   245,   245,
     245,   245,   245,   245,   245,   245,   245,   245,   245,   245,
     245,   245,   245,   245,   245,   245,   245,   245,   245,   245,
     245,   245,   245,   245,   245,   245,   245,   245,   245,   245,
     245,   245,   245,   245,   245,   245,   245,   245,   245,   245,
     245,   245,   245,   245,   245,   245,   245,   245,   245,   245,
     245,   245,   245,   245,   245,   245,   245,   245,   245,   245,
     245,   245,   245,   245,   245,   245,   245,   245,   245,   245,
     245,   245,   245,   245,   245,   245,   245,   245,   245,   245,
     246,   246,   247,   247,   247,   247,   248,   248,   249,   249,
     250,   250,   251,   252,   252,   253,   253,   254,   254,   255,
     256,   256,   257,   258,   258,   258,   258,   258,   259,   259,
     259,   260,   260,   260,   260,   261,   261,   262,   263,   264,
     264,   265,   266,   266,   267,   267,   268,   269,   269,   269,
     270,   270,   271,   271,   272,   272,   273,   273,   274,   274,
     275,   275,   276,   276,   277,   277,   278,   278,   279,   279,
     280,   280,   280,   280,   281,   281,   282,   282,   283,   283,
     284,   284,   285,   285,   285,   285,   286,   286,   287,   287,
     288,   289,   289,   290,   290,   290,   290,   290,   290,   290,
     291,   291,   291,   291,   291,   291,   291,   291,   291,   291,
     291,   291,   291,   291,   291,   291,   291,   291,   291,   291,
     291,   291,   291,   291,   291,   291,   291,   291,   291,   291,
     291,   291,   291,   291,   291,   291,   291,   291,   291,   291,
     291,   291,   291,   291,   291,   291,   291,   291,   291,   291,
     291,   291,   291,   291,   291,   291,   292,   292,   292,   292,
     293,   294,   294,   294,   294,   294,   294,   294,   294,   294,
     294,   294,   294,   294,   294,   294,   294,   294,   294,   294,
     294,   294,   294,   294,   294,   294,   294,   295,   296,   296,
     296,   296,   296,   296,   296,   296,   296,   296,   296,   296,
     296,   296,   296,   296,   296,   296,   296,   296,   296,   296,
     296,   296,   296,   296,   296,   296,   296,   296,   296,   296,
     296,   296,   296,   296,   296,   296,   297,   297,   297,   297,
     298,   298,   299,   299,   300,   300,   301,   301,   302,   302,
     302,   302,   302,   303,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   303,   303,   303,   304,   304,   304,   305,
     305,   305,   305,   306,   306,   306,   306,   307,   307,   308,
     308,   309,   309,   310,   310,   310,   310,   310,   310,   311,
     311,   312,   312,   312,   312,   312,   312,   312,   312,   312,
     312,   312,   312,   312,   312,   312,   312,   312,   312,   312,
     312,   312,   312,   312,   312,   312,   312,   312,   312,   312,
     312,   312,   313,   313,   314,   315,   315,   316,   316,   316,
     316,   317,   317,   318,   318,   319,   319,   319,   319,   320,
     320,   320,   320,   321,   321,   321,   321,   321,   321,   321,
     321,   321,   321,   321,   321,   321,   321,   322,   322,   322,
     322,   323,   323,   323,   324,   325,   325,   326,   326,   327,
     328,   328,   329,   330,   330,   331,   332,   332,   333,   333,
     334,   335,   336,   336,   337,   338,   338,   339,   340,   340,
     341,   341,   341,   341,   341,   341,   341,   341,   341,   341,
     341,   341,   342,   342,   343,   343,   343,   343,   343,   343,
     343,   344,   345,   345,   346,   346,   347,   347,   348,   348,
     349,   349,   350,   350,   351,   351,   352,   352,   352,   352,
     352,   353,   353,   353,   353
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
      15,    16,     3,     4,     5,     6,     3,     4,     3,     3,
       4,     3,     3,     4,     4,     6,     5,     3,     4,     3,
       4,     3,     3,     5,     7,     7,     6,     8,     8,     5,
       2,     3,     1,     3,     3,     5,     3,     1,     1,     1,
       1,     1,     1,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,    14,    19,    16,    20,
      16,    15,    13,    18,    14,    13,    11,     8,    10,     5,
       7,     4,     6,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     4,     5,     4,     4,     4,     4,     4,     4,
       4,     3,     2,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     6,     3,     5,
       4,     6,     3,     3,     5,     5,     6,     4,     6,     3,
       5,     4,     5,     6,     4,     4,     5,     5,     6,     1,
       3,     1,     3,     1,     1,     1,     1,     1,     2,     2,
       2,     2,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     3,     1,     1,     2,
       2,     3,     2,     2,     3,     2,     2,     2,     2,     3,
       3,     3,     1,     1,     2,     2,     3,     2,     2,     3,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     1,     3,     2,     2,     2,     1,     2,     2,
       2,     1,     2,     0,     3,     0,     1,     0,     2,     0,
       4,     0,     4,     0,     1,     3,     1,     3,     3,     3,
       3,     6,     3,     3,     6
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
#line 2537 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 330 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2545 "parser.cpp"
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
#line 2559 "parser.cpp"
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
#line 2573 "parser.cpp"
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
#line 2587 "parser.cpp"
        break;

    case YYSYMBOL_column_type_array: /* column_type_array  */
#line 191 "parser.y"
            {
    fprintf(stderr, "destroy column_type_array\n");
    delete (((*yyvaluep).column_type_array_t));
}
#line 2596 "parser.cpp"
        break;

    case YYSYMBOL_column_type: /* column_type  */
#line 186 "parser.y"
            {
    fprintf(stderr, "destroy column_type\n");
    delete (((*yyvaluep).column_type_t));
}
#line 2605 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 323 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2616 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2624 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2633 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2642 "parser.cpp"
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
#line 2656 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 287 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2667 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2677 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2687 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2697 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2707 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2717 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2727 "parser.cpp"
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
#line 2741 "parser.cpp"
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
#line 2755 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 357 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2765 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2773 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2781 "parser.cpp"
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
#line 2795 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2804 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2812 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2820 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2828 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2836 "parser.cpp"
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
#line 2850 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2859 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2868 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2877 "parser.cpp"
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
#line 2890 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 347 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2899 "parser.cpp"
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
#line 2913 "parser.cpp"
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
#line 2927 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 363 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2937 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2946 "parser.cpp"
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
#line 2960 "parser.cpp"
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
#line 2974 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2982 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2990 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2998 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3006 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3014 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3022 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3030 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3038 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3046 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3054 "parser.cpp"
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
#line 3068 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3076 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3084 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3092 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3100 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3108 "parser.cpp"
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
#line 3121 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3129 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3137 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3145 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3153 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3161 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3169 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3177 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3185 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3193 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3201 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3209 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3217 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3225 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3233 "parser.cpp"
        break;

    case YYSYMBOL_curly_brackets_expr: /* curly_brackets_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3241 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_curly_brackets_expr: /* unclosed_curly_brackets_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3249 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 384 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3257 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 388 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3265 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3273 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3281 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3289 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3297 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3305 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3313 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 330 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3321 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 220 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3332 "parser.cpp"
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
#line 3346 "parser.cpp"
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
#line 3360 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 196 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3371 "parser.cpp"
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

#line 3479 "parser.cpp"

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
#line 518 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3694 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 522 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3705 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 528 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3716 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 535 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3722 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 536 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3728 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 537 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3734 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 538 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3740 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 539 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3746 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 540 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3752 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 541 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3758 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 542 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3764 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 543 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3770 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 544 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3776 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 545 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3782 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 546 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3788 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 547 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3794 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 548 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3800 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 549 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3806 "parser.cpp"
    break;

  case 20: /* statement: check_statement  */
#line 550 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].check_stmt); }
#line 3812 "parser.cpp"
    break;

  case 21: /* explainable_statement: create_statement  */
#line 552 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3818 "parser.cpp"
    break;

  case 22: /* explainable_statement: drop_statement  */
#line 553 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3824 "parser.cpp"
    break;

  case 23: /* explainable_statement: copy_statement  */
#line 554 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3830 "parser.cpp"
    break;

  case 24: /* explainable_statement: show_statement  */
#line 555 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3836 "parser.cpp"
    break;

  case 25: /* explainable_statement: select_statement  */
#line 556 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3842 "parser.cpp"
    break;

  case 26: /* explainable_statement: delete_statement  */
#line 557 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3848 "parser.cpp"
    break;

  case 27: /* explainable_statement: update_statement  */
#line 558 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3854 "parser.cpp"
    break;

  case 28: /* explainable_statement: insert_statement  */
#line 559 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3860 "parser.cpp"
    break;

  case 29: /* explainable_statement: flush_statement  */
#line 560 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3866 "parser.cpp"
    break;

  case 30: /* explainable_statement: optimize_statement  */
#line 561 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3872 "parser.cpp"
    break;

  case 31: /* explainable_statement: command_statement  */
#line 562 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3878 "parser.cpp"
    break;

  case 32: /* explainable_statement: compact_statement  */
#line 563 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3884 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER COMMENT STRING  */
#line 570 "parser.y"
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
#line 3906 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 587 "parser.y"
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
#line 3926 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 604 "parser.y"
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
#line 3944 "parser.cpp"
    break;

  case 36: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 620 "parser.y"
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
#line 3977 "parser.cpp"
    break;

  case 37: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 649 "parser.y"
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
#line 3997 "parser.cpp"
    break;

  case 38: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list COMMENT STRING  */
#line 664 "parser.y"
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
#line 4033 "parser.cpp"
    break;

  case 39: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement COMMENT STRING  */
#line 696 "parser.y"
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
#line 4055 "parser.cpp"
    break;

  case 40: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 714 "parser.y"
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
#line 4076 "parser.cpp"
    break;

  case 41: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info  */
#line 732 "parser.y"
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
#line 4109 "parser.cpp"
    break;

  case 42: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info COMMENT STRING  */
#line 760 "parser.y"
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
#line 4144 "parser.cpp"
    break;

  case 43: /* table_element_array: table_element  */
#line 791 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 4153 "parser.cpp"
    break;

  case 44: /* table_element_array: table_element_array ',' table_element  */
#line 795 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 4162 "parser.cpp"
    break;

  case 45: /* column_def_array: table_column  */
#line 800 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 4171 "parser.cpp"
    break;

  case 46: /* column_def_array: column_def_array ',' table_column  */
#line 804 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 4180 "parser.cpp"
    break;

  case 47: /* table_element: table_column  */
#line 810 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 4188 "parser.cpp"
    break;

  case 48: /* table_element: table_constraint  */
#line 813 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 4196 "parser.cpp"
    break;

  case 49: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 820 "parser.y"
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
#line 4221 "parser.cpp"
    break;

  case 50: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 840 "parser.y"
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
#line 4248 "parser.cpp"
    break;

  case 51: /* table_column: IDENTIFIER column_type with_index_param_list default_expr COMMENT STRING  */
#line 862 "parser.y"
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
#line 4277 "parser.cpp"
    break;

  case 52: /* table_column: IDENTIFIER column_type column_constraints default_expr COMMENT STRING  */
#line 886 "parser.y"
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
#line 4307 "parser.cpp"
    break;

  case 53: /* column_type_array: column_type  */
#line 912 "parser.y"
                                {
    (yyval.column_type_array_t) = new std::vector<std::unique_ptr<infinity::ColumnType>>();
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4316 "parser.cpp"
    break;

  case 54: /* column_type_array: column_type_array ',' column_type  */
#line 916 "parser.y"
                                    {
    (yyval.column_type_array_t) = (yyvsp[-2].column_type_array_t);
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4325 "parser.cpp"
    break;

  case 55: /* column_type: BOOLEAN  */
#line 922 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4331 "parser.cpp"
    break;

  case 56: /* column_type: JSON  */
#line 923 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kJson, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4337 "parser.cpp"
    break;

  case 57: /* column_type: TINYINT  */
#line 924 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4343 "parser.cpp"
    break;

  case 58: /* column_type: SMALLINT  */
#line 925 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4349 "parser.cpp"
    break;

  case 59: /* column_type: INTEGER  */
#line 926 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4355 "parser.cpp"
    break;

  case 60: /* column_type: INT  */
#line 927 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4361 "parser.cpp"
    break;

  case 61: /* column_type: BIGINT  */
#line 928 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4367 "parser.cpp"
    break;

  case 62: /* column_type: HUGEINT  */
#line 929 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4373 "parser.cpp"
    break;

  case 63: /* column_type: FLOAT  */
#line 930 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4379 "parser.cpp"
    break;

  case 64: /* column_type: REAL  */
#line 931 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4385 "parser.cpp"
    break;

  case 65: /* column_type: DOUBLE  */
#line 932 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4391 "parser.cpp"
    break;

  case 66: /* column_type: FLOAT16  */
#line 933 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4397 "parser.cpp"
    break;

  case 67: /* column_type: BFLOAT16  */
#line 934 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4403 "parser.cpp"
    break;

  case 68: /* column_type: DATE  */
#line 935 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4409 "parser.cpp"
    break;

  case 69: /* column_type: TIME  */
#line 936 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4415 "parser.cpp"
    break;

  case 70: /* column_type: DATETIME  */
#line 937 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4421 "parser.cpp"
    break;

  case 71: /* column_type: TIMESTAMP  */
#line 938 "parser.y"
            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4427 "parser.cpp"
    break;

  case 72: /* column_type: UUID  */
#line 939 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4433 "parser.cpp"
    break;

  case 73: /* column_type: POINT  */
#line 940 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4439 "parser.cpp"
    break;

  case 74: /* column_type: LINE  */
#line 941 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4445 "parser.cpp"
    break;

  case 75: /* column_type: LSEG  */
#line 942 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4451 "parser.cpp"
    break;

  case 76: /* column_type: BOX  */
#line 943 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4457 "parser.cpp"
    break;

  case 77: /* column_type: CIRCLE  */
#line 946 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4463 "parser.cpp"
    break;

  case 78: /* column_type: VARCHAR  */
#line 948 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4469 "parser.cpp"
    break;

  case 79: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 949 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4475 "parser.cpp"
    break;

  case 80: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 950 "parser.y"
                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4481 "parser.cpp"
    break;

  case 81: /* column_type: DECIMAL  */
#line 951 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4487 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 954 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4493 "parser.cpp"
    break;

  case 83: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 955 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4499 "parser.cpp"
    break;

  case 84: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 956 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4505 "parser.cpp"
    break;

  case 85: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 957 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4511 "parser.cpp"
    break;

  case 86: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 958 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4517 "parser.cpp"
    break;

  case 87: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 959 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4523 "parser.cpp"
    break;

  case 88: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 960 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4529 "parser.cpp"
    break;

  case 89: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 961 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4535 "parser.cpp"
    break;

  case 90: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 962 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4541 "parser.cpp"
    break;

  case 91: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 963 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4547 "parser.cpp"
    break;

  case 92: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 964 "parser.y"
                                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4553 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 965 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4559 "parser.cpp"
    break;

  case 94: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 966 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4565 "parser.cpp"
    break;

  case 95: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 967 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4571 "parser.cpp"
    break;

  case 96: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 968 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4577 "parser.cpp"
    break;

  case 97: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 969 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4583 "parser.cpp"
    break;

  case 98: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 970 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4589 "parser.cpp"
    break;

  case 99: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 971 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4595 "parser.cpp"
    break;

  case 100: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 972 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4601 "parser.cpp"
    break;

  case 101: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 973 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4607 "parser.cpp"
    break;

  case 102: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 974 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4613 "parser.cpp"
    break;

  case 103: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 975 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4619 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 976 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4625 "parser.cpp"
    break;

  case 105: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 977 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4631 "parser.cpp"
    break;

  case 106: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 978 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4637 "parser.cpp"
    break;

  case 107: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 979 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4643 "parser.cpp"
    break;

  case 108: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 980 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4649 "parser.cpp"
    break;

  case 109: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 981 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4655 "parser.cpp"
    break;

  case 110: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 982 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4661 "parser.cpp"
    break;

  case 111: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 983 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4667 "parser.cpp"
    break;

  case 112: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 984 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4673 "parser.cpp"
    break;

  case 113: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 985 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4679 "parser.cpp"
    break;

  case 114: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 986 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4685 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 987 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4691 "parser.cpp"
    break;

  case 116: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 988 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4697 "parser.cpp"
    break;

  case 117: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 989 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4703 "parser.cpp"
    break;

  case 118: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 990 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4709 "parser.cpp"
    break;

  case 119: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 991 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4715 "parser.cpp"
    break;

  case 120: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 992 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4721 "parser.cpp"
    break;

  case 121: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 993 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4727 "parser.cpp"
    break;

  case 122: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 994 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4733 "parser.cpp"
    break;

  case 123: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 995 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4739 "parser.cpp"
    break;

  case 124: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 996 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4745 "parser.cpp"
    break;

  case 125: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 997 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4751 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 998 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4757 "parser.cpp"
    break;

  case 127: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 999 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4763 "parser.cpp"
    break;

  case 128: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1000 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4769 "parser.cpp"
    break;

  case 129: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1001 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4775 "parser.cpp"
    break;

  case 130: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1002 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4781 "parser.cpp"
    break;

  case 131: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1003 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4787 "parser.cpp"
    break;

  case 132: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1004 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4793 "parser.cpp"
    break;

  case 133: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1005 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4799 "parser.cpp"
    break;

  case 134: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1006 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4805 "parser.cpp"
    break;

  case 135: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1007 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4811 "parser.cpp"
    break;

  case 136: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1008 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4817 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1009 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4823 "parser.cpp"
    break;

  case 138: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4829 "parser.cpp"
    break;

  case 139: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1011 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4835 "parser.cpp"
    break;

  case 140: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1012 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4841 "parser.cpp"
    break;

  case 141: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1013 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4847 "parser.cpp"
    break;

  case 142: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1014 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4853 "parser.cpp"
    break;

  case 143: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4859 "parser.cpp"
    break;

  case 144: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4865 "parser.cpp"
    break;

  case 145: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1017 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4871 "parser.cpp"
    break;

  case 146: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1018 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4877 "parser.cpp"
    break;

  case 147: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1019 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4883 "parser.cpp"
    break;

  case 148: /* column_type: ARRAY '(' column_type ')'  */
#line 1020 "parser.y"
                            {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kArray, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_.emplace_back((yyvsp[-1].column_type_t));
}
#line 4892 "parser.cpp"
    break;

  case 149: /* column_type: TUPLE '(' column_type_array ')'  */
#line 1024 "parser.y"
                                  {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTuple, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_ = std::move(*((yyvsp[-1].column_type_array_t)));
  delete (yyvsp[-1].column_type_array_t);
}
#line 4902 "parser.cpp"
    break;

  case 150: /* column_constraints: column_constraint  */
#line 1047 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4911 "parser.cpp"
    break;

  case 151: /* column_constraints: column_constraints column_constraint  */
#line 1051 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4925 "parser.cpp"
    break;

  case 152: /* column_constraint: PRIMARY KEY  */
#line 1061 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4933 "parser.cpp"
    break;

  case 153: /* column_constraint: UNIQUE  */
#line 1064 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4941 "parser.cpp"
    break;

  case 154: /* column_constraint: NULLABLE  */
#line 1067 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4949 "parser.cpp"
    break;

  case 155: /* column_constraint: NOT NULLABLE  */
#line 1070 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4957 "parser.cpp"
    break;

  case 156: /* default_expr: DEFAULT constant_expr  */
#line 1074 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4965 "parser.cpp"
    break;

  case 157: /* default_expr: %empty  */
#line 1077 "parser.y"
                                   {
    (yyval.const_expr_t) = nullptr;
}
#line 4973 "parser.cpp"
    break;

  case 158: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1082 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4983 "parser.cpp"
    break;

  case 159: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1087 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4993 "parser.cpp"
    break;

  case 160: /* identifier_array: IDENTIFIER  */
#line 1094 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5004 "parser.cpp"
    break;

  case 161: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1100 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 5015 "parser.cpp"
    break;

  case 162: /* delete_statement: DELETE FROM table_name where_clause  */
#line 1110 "parser.y"
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
#line 5032 "parser.cpp"
    break;

  case 163: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES insert_row_list  */
#line 1126 "parser.y"
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
#line 5075 "parser.cpp"
    break;

  case 164: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1164 "parser.y"
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
#line 5095 "parser.cpp"
    break;

  case 165: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1180 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 5103 "parser.cpp"
    break;

  case 166: /* optional_identifier_array: %empty  */
#line 1183 "parser.y"
         {
    (yyval.identifier_array_t) = nullptr;
}
#line 5111 "parser.cpp"
    break;

  case 167: /* explain_statement: EXPLAIN IDENTIFIER explainable_statement  */
#line 1190 "parser.y"
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
#line 5134 "parser.cpp"
    break;

  case 168: /* explain_statement: EXPLAIN explainable_statement  */
#line 1207 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 5144 "parser.cpp"
    break;

  case 169: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1216 "parser.y"
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
#line 5161 "parser.cpp"
    break;

  case 170: /* update_expr_array: update_expr  */
#line 1229 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 5170 "parser.cpp"
    break;

  case 171: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1233 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 5179 "parser.cpp"
    break;

  case 172: /* update_expr: IDENTIFIER '=' expr  */
#line 1238 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 5191 "parser.cpp"
    break;

  case 173: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1251 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 5207 "parser.cpp"
    break;

  case 174: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1264 "parser.y"
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
#line 5225 "parser.cpp"
    break;

  case 175: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1279 "parser.y"
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
#line 5243 "parser.cpp"
    break;

  case 176: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1294 "parser.y"
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
#line 5261 "parser.cpp"
    break;

  case 177: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1309 "parser.y"
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
#line 5284 "parser.cpp"
    break;

  case 178: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1332 "parser.y"
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
#line 5342 "parser.cpp"
    break;

  case 179: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1385 "parser.y"
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
#line 5402 "parser.cpp"
    break;

  case 180: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1440 "parser.y"
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
#line 5454 "parser.cpp"
    break;

  case 181: /* select_statement: select_without_paren  */
#line 1491 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5462 "parser.cpp"
    break;

  case 182: /* select_statement: select_with_paren  */
#line 1494 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5470 "parser.cpp"
    break;

  case 183: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1497 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5484 "parser.cpp"
    break;

  case 184: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1506 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5498 "parser.cpp"
    break;

  case 185: /* select_with_paren: '(' select_without_paren ')'  */
#line 1516 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5506 "parser.cpp"
    break;

  case 186: /* select_with_paren: '(' select_with_paren ')'  */
#line 1519 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5514 "parser.cpp"
    break;

  case 187: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1523 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5523 "parser.cpp"
    break;

  case 188: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1528 "parser.y"
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
#line 5554 "parser.cpp"
    break;

  case 189: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1555 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5562 "parser.cpp"
    break;

  case 190: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1558 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5570 "parser.cpp"
    break;

  case 191: /* select_clause_without_modifier: SELECT distinct expr_array highlight_clause from_clause search_clause where_clause group_by_clause having_clause  */
#line 1563 "parser.y"
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
#line 5591 "parser.cpp"
    break;

  case 192: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1580 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5599 "parser.cpp"
    break;

  case 193: /* order_by_clause: %empty  */
#line 1583 "parser.y"
                              {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5607 "parser.cpp"
    break;

  case 194: /* order_by_expr_list: order_by_expr  */
#line 1587 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5616 "parser.cpp"
    break;

  case 195: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1591 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5625 "parser.cpp"
    break;

  case 196: /* order_by_expr: expr order_by_type  */
#line 1596 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5635 "parser.cpp"
    break;

  case 197: /* order_by_type: ASC  */
#line 1602 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5643 "parser.cpp"
    break;

  case 198: /* order_by_type: DESC  */
#line 1605 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5651 "parser.cpp"
    break;

  case 199: /* order_by_type: %empty  */
#line 1608 "parser.y"
         {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5659 "parser.cpp"
    break;

  case 200: /* limit_expr: LIMIT expr  */
#line 1612 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5667 "parser.cpp"
    break;

  case 201: /* limit_expr: %empty  */
#line 1616 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5673 "parser.cpp"
    break;

  case 202: /* offset_expr: OFFSET expr  */
#line 1618 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5681 "parser.cpp"
    break;

  case 203: /* offset_expr: %empty  */
#line 1622 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5687 "parser.cpp"
    break;

  case 204: /* distinct: DISTINCT  */
#line 1624 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5695 "parser.cpp"
    break;

  case 205: /* distinct: %empty  */
#line 1627 "parser.y"
         {
    (yyval.bool_value) = false;
}
#line 5703 "parser.cpp"
    break;

  case 206: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1631 "parser.y"
                                       {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5711 "parser.cpp"
    break;

  case 207: /* highlight_clause: %empty  */
#line 1634 "parser.y"
         {
    (yyval.expr_array_t) = nullptr;
}
#line 5719 "parser.cpp"
    break;

  case 208: /* from_clause: FROM table_reference  */
#line 1638 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5727 "parser.cpp"
    break;

  case 209: /* from_clause: %empty  */
#line 1641 "parser.y"
                              {
    (yyval.table_reference_t) = nullptr;
}
#line 5735 "parser.cpp"
    break;

  case 210: /* search_clause: SEARCH sub_search_array  */
#line 1645 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5745 "parser.cpp"
    break;

  case 211: /* search_clause: %empty  */
#line 1650 "parser.y"
                                {
    (yyval.expr_t) = nullptr;
}
#line 5753 "parser.cpp"
    break;

  case 212: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1654 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5761 "parser.cpp"
    break;

  case 213: /* optional_search_filter_expr: %empty  */
#line 1657 "parser.y"
                               {
    (yyval.expr_t) = nullptr;
}
#line 5769 "parser.cpp"
    break;

  case 214: /* where_clause: WHERE expr  */
#line 1661 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5777 "parser.cpp"
    break;

  case 215: /* where_clause: %empty  */
#line 1664 "parser.y"
                               {
    (yyval.expr_t) = nullptr;
}
#line 5785 "parser.cpp"
    break;

  case 216: /* having_clause: HAVING expr  */
#line 1668 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5793 "parser.cpp"
    break;

  case 217: /* having_clause: %empty  */
#line 1671 "parser.y"
                               {
    (yyval.expr_t) = nullptr;
}
#line 5801 "parser.cpp"
    break;

  case 218: /* group_by_clause: GROUP BY expr_array  */
#line 1675 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5809 "parser.cpp"
    break;

  case 219: /* group_by_clause: %empty  */
#line 1678 "parser.y"
         {
    (yyval.expr_array_t) = nullptr;
}
#line 5817 "parser.cpp"
    break;

  case 220: /* set_operator: UNION  */
#line 1682 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5825 "parser.cpp"
    break;

  case 221: /* set_operator: UNION ALL  */
#line 1685 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5833 "parser.cpp"
    break;

  case 222: /* set_operator: INTERSECT  */
#line 1688 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5841 "parser.cpp"
    break;

  case 223: /* set_operator: EXCEPT  */
#line 1691 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5849 "parser.cpp"
    break;

  case 224: /* table_reference: table_reference_unit  */
#line 1699 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5857 "parser.cpp"
    break;

  case 225: /* table_reference: table_reference ',' table_reference_unit  */
#line 1702 "parser.y"
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
#line 5875 "parser.cpp"
    break;

  case 228: /* table_reference_name: table_name table_alias  */
#line 1719 "parser.y"
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
#line 5893 "parser.cpp"
    break;

  case 229: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1733 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5904 "parser.cpp"
    break;

  case 230: /* table_name: IDENTIFIER  */
#line 1742 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5914 "parser.cpp"
    break;

  case 231: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1747 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5926 "parser.cpp"
    break;

  case 232: /* table_alias: AS IDENTIFIER  */
#line 1756 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5936 "parser.cpp"
    break;

  case 233: /* table_alias: IDENTIFIER  */
#line 1761 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5946 "parser.cpp"
    break;

  case 234: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1766 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5957 "parser.cpp"
    break;

  case 235: /* table_alias: %empty  */
#line 1772 "parser.y"
         {
    (yyval.table_alias_t) = nullptr;
}
#line 5965 "parser.cpp"
    break;

  case 236: /* with_clause: WITH with_expr_list  */
#line 1779 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5973 "parser.cpp"
    break;

  case 237: /* with_clause: %empty  */
#line 1782 "parser.y"
                                 {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5981 "parser.cpp"
    break;

  case 238: /* with_expr_list: with_expr  */
#line 1786 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5990 "parser.cpp"
    break;

  case 239: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1789 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5999 "parser.cpp"
    break;

  case 240: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1794 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 6011 "parser.cpp"
    break;

  case 241: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1806 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 6023 "parser.cpp"
    break;

  case 242: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1813 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 6036 "parser.cpp"
    break;

  case 243: /* join_type: INNER  */
#line 1827 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 6044 "parser.cpp"
    break;

  case 244: /* join_type: LEFT  */
#line 1830 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 6052 "parser.cpp"
    break;

  case 245: /* join_type: RIGHT  */
#line 1833 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 6060 "parser.cpp"
    break;

  case 246: /* join_type: OUTER  */
#line 1836 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6068 "parser.cpp"
    break;

  case 247: /* join_type: FULL  */
#line 1839 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6076 "parser.cpp"
    break;

  case 248: /* join_type: CROSS  */
#line 1842 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 6084 "parser.cpp"
    break;

  case 249: /* join_type: %empty  */
#line 1845 "parser.y"
                       {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 6092 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW DATABASES  */
#line 1852 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 6101 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW TABLES  */
#line 1856 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 6110 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW TASKS  */
#line 1860 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTasks;
}
#line 6119 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW CONFIGS  */
#line 1864 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 6128 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1868 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6140 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW PROFILES  */
#line 1875 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 6149 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW BUFFER  */
#line 1879 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 6158 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW MEMINDEX  */
#line 1883 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 6167 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW QUERIES  */
#line 1887 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 6176 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1891 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 6186 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW TRANSACTIONS  */
#line 1896 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 6195 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1900 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6205 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW TRANSACTION HISTORY  */
#line 1905 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactionHistory;
}
#line 6214 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW SESSION VARIABLES  */
#line 1909 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 6223 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1913 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 6232 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1917 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6243 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1923 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6254 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1929 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6266 "parser.cpp"
    break;

  case 268: /* show_statement: SHOW TABLE table_name  */
#line 1936 "parser.y"
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
#line 6282 "parser.cpp"
    break;

  case 269: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1947 "parser.y"
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
#line 6298 "parser.cpp"
    break;

  case 270: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1958 "parser.y"
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
#line 6314 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1969 "parser.y"
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
#line 6331 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1981 "parser.y"
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
#line 6348 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1993 "parser.y"
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
#line 6366 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 2006 "parser.y"
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
#line 6385 "parser.cpp"
    break;

  case 275: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 2020 "parser.y"
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
#line 6401 "parser.cpp"
    break;

  case 276: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 2031 "parser.y"
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
#line 6421 "parser.cpp"
    break;

  case 277: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 2046 "parser.y"
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
#line 6443 "parser.cpp"
    break;

  case 278: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNKS  */
#line 2063 "parser.y"
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
#line 6465 "parser.cpp"
    break;

  case 279: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 2080 "parser.y"
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
#line 6488 "parser.cpp"
    break;

  case 280: /* show_statement: SHOW LOGS  */
#line 2098 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6497 "parser.cpp"
    break;

  case 281: /* show_statement: SHOW CATALOG  */
#line 2102 "parser.y"
               {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalog;
}
#line 6506 "parser.cpp"
    break;

  case 282: /* show_statement: SHOW CATALOG STRING  */
#line 2106 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCatalogKey;
      ParserHelper::ToLower((yyvsp[0].str_value));
      (yyval.show_stmt)->var_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6518 "parser.cpp"
    break;

  case 283: /* show_statement: SHOW CATALOG TO file_path  */
#line 2113 "parser.y"
                            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogToFile;
      (yyval.show_stmt)->file_path_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6529 "parser.cpp"
    break;

  case 284: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2119 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6538 "parser.cpp"
    break;

  case 285: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2123 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6547 "parser.cpp"
    break;

  case 286: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2127 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6558 "parser.cpp"
    break;

  case 287: /* show_statement: SHOW MEMORY  */
#line 2133 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6567 "parser.cpp"
    break;

  case 288: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2137 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6576 "parser.cpp"
    break;

  case 289: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2141 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6585 "parser.cpp"
    break;

  case 290: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2145 "parser.y"
                          {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
      ParserHelper::ToLower((yyvsp[-2].str_value));
      (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
      free((yyvsp[-2].str_value));
}
#line 6597 "parser.cpp"
    break;

  case 291: /* show_statement: SHOW SNAPSHOTS  */
#line 2152 "parser.y"
                 {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListSnapshots;
}
#line 6606 "parser.cpp"
    break;

  case 292: /* show_statement: SHOW SNAPSHOT IDENTIFIER  */
#line 2156 "parser.y"
                           {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowSnapshot;
     ParserHelper::ToLower((yyvsp[0].str_value));
     (yyval.show_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6618 "parser.cpp"
    break;

  case 293: /* show_statement: SHOW CACHES  */
#line 2163 "parser.y"
              {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCaches;
}
#line 6627 "parser.cpp"
    break;

  case 294: /* show_statement: SHOW CACHE  */
#line 2167 "parser.y"
             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowCache;
}
#line 6636 "parser.cpp"
    break;

  case 295: /* show_statement: SHOW COMPACT  */
#line 2171 "parser.y"
               {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCompact;
}
#line 6645 "parser.cpp"
    break;

  case 296: /* show_statement: SHOW COMPACT NOT NULLABLE  */
#line 2175 "parser.y"
                            {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCompact;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6655 "parser.cpp"
    break;

  case 297: /* show_statement: SHOW CHECKPOINT  */
#line 2180 "parser.y"
                  {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCheckpoint;
}
#line 6664 "parser.cpp"
    break;

  case 298: /* show_statement: SHOW CHECKPOINT NOT NULLABLE  */
#line 2184 "parser.y"
                               {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCheckpoint;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6674 "parser.cpp"
    break;

  case 299: /* show_statement: SHOW CHECKPOINT LONG_VALUE  */
#line 2189 "parser.y"
                             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowCheckpoint;
     (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6684 "parser.cpp"
    break;

  case 300: /* show_statement: SHOW OPTIMIZE  */
#line 2194 "parser.y"
                {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListOptimize;
}
#line 6693 "parser.cpp"
    break;

  case 301: /* show_statement: SHOW OPTIMIZE NOT NULLABLE  */
#line 2198 "parser.y"
                             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListOptimize;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6703 "parser.cpp"
    break;

  case 302: /* show_statement: SHOW IMPORT  */
#line 2203 "parser.y"
              {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListImport;
}
#line 6712 "parser.cpp"
    break;

  case 303: /* show_statement: SHOW CLEAN  */
#line 2207 "parser.y"
             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListClean;
}
#line 6721 "parser.cpp"
    break;

  case 304: /* show_statement: SHOW CLEAN NOT NULLABLE  */
#line 2211 "parser.y"
                          {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListClean;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6731 "parser.cpp"
    break;

  case 305: /* show_statement: SHOW CLEAN LONG_VALUE  */
#line 2216 "parser.y"
                        {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowClean;
     (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6741 "parser.cpp"
    break;

  case 306: /* flush_statement: FLUSH DATA  */
#line 2225 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6750 "parser.cpp"
    break;

  case 307: /* flush_statement: FLUSH LOG  */
#line 2229 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6759 "parser.cpp"
    break;

  case 308: /* flush_statement: FLUSH BUFFER  */
#line 2233 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6768 "parser.cpp"
    break;

  case 309: /* flush_statement: FLUSH CATALOG  */
#line 2237 "parser.y"
                {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kCatalog;
}
#line 6777 "parser.cpp"
    break;

  case 310: /* optimize_statement: OPTIMIZE table_name  */
#line 2245 "parser.y"
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
#line 6792 "parser.cpp"
    break;

  case 311: /* command_statement: USE IDENTIFIER  */
#line 2259 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6803 "parser.cpp"
    break;

  case 312: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2265 "parser.y"
                                       {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6813 "parser.cpp"
    break;

  case 313: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2270 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6824 "parser.cpp"
    break;

  case 314: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2276 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6835 "parser.cpp"
    break;

  case 315: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2282 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6848 "parser.cpp"
    break;

  case 316: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2290 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6859 "parser.cpp"
    break;

  case 317: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2296 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6870 "parser.cpp"
    break;

  case 318: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2302 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6881 "parser.cpp"
    break;

  case 319: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2308 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6892 "parser.cpp"
    break;

  case 320: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2314 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6905 "parser.cpp"
    break;

  case 321: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2322 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6916 "parser.cpp"
    break;

  case 322: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2328 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6927 "parser.cpp"
    break;

  case 323: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2334 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6938 "parser.cpp"
    break;

  case 324: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2340 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6949 "parser.cpp"
    break;

  case 325: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2346 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6962 "parser.cpp"
    break;

  case 326: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2354 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6973 "parser.cpp"
    break;

  case 327: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2360 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6984 "parser.cpp"
    break;

  case 328: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON TABLE IDENTIFIER  */
#line 2366 "parser.y"
                                                 {
    ParserHelper::ToLower((yyvsp[-3].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kTable, (yyvsp[0].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[0].str_value));
}
#line 6997 "parser.cpp"
    break;

  case 329: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON DATABASE IDENTIFIER  */
#line 2374 "parser.y"
                                                    {
    ParserHelper::ToLower((yyvsp[-3].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kDatabase, (yyvsp[0].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[0].str_value));
}
#line 7010 "parser.cpp"
    break;

  case 330: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON SYSTEM  */
#line 2382 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-2].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kSystem);
    free((yyvsp[-2].str_value));
}
#line 7021 "parser.cpp"
    break;

  case 331: /* command_statement: DROP SNAPSHOT IDENTIFIER  */
#line 2388 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kDrop, infinity::SnapshotScope::kIgnore);
    free((yyvsp[0].str_value));
}
#line 7032 "parser.cpp"
    break;

  case 332: /* command_statement: RESTORE SYSTEM SNAPSHOT IDENTIFIER  */
#line 2394 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kSystem);
    free((yyvsp[0].str_value));
}
#line 7043 "parser.cpp"
    break;

  case 333: /* command_statement: RESTORE DATABASE SNAPSHOT IDENTIFIER  */
#line 2400 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kDatabase);
    free((yyvsp[0].str_value));
}
#line 7054 "parser.cpp"
    break;

  case 334: /* command_statement: RESTORE TABLE SNAPSHOT IDENTIFIER  */
#line 2406 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kTable);
    free((yyvsp[0].str_value));
}
#line 7065 "parser.cpp"
    break;

  case 335: /* command_statement: CLEAN DATA  */
#line 2412 "parser.y"
             {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::CleanupCmd>();
}
#line 7074 "parser.cpp"
    break;

  case 336: /* command_statement: DUMP INDEX IDENTIFIER ON table_name  */
#line 2416 "parser.y"
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
#line 7090 "parser.cpp"
    break;

  case 337: /* compact_statement: COMPACT TABLE table_name  */
#line 2428 "parser.y"
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
#line 7107 "parser.cpp"
    break;

  case 338: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2441 "parser.y"
                                                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 7118 "parser.cpp"
    break;

  case 339: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2447 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 7130 "parser.cpp"
    break;

  case 340: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2454 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 7143 "parser.cpp"
    break;

  case 341: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2462 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 7157 "parser.cpp"
    break;

  case 342: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2471 "parser.y"
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
#line 7172 "parser.cpp"
    break;

  case 343: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2481 "parser.y"
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
#line 7187 "parser.cpp"
    break;

  case 344: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2491 "parser.y"
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
#line 7203 "parser.cpp"
    break;

  case 345: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2502 "parser.y"
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
#line 7219 "parser.cpp"
    break;

  case 346: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2513 "parser.y"
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
#line 7236 "parser.cpp"
    break;

  case 347: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2525 "parser.y"
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
#line 7253 "parser.cpp"
    break;

  case 348: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2537 "parser.y"
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
#line 7268 "parser.cpp"
    break;

  case 349: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2547 "parser.y"
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
#line 7284 "parser.cpp"
    break;

  case 350: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2558 "parser.y"
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
#line 7301 "parser.cpp"
    break;

  case 351: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2570 "parser.y"
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
#line 7319 "parser.cpp"
    break;

  case 352: /* admin_statement: ADMIN SHOW LOGS  */
#line 2583 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 7328 "parser.cpp"
    break;

  case 353: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2587 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 7338 "parser.cpp"
    break;

  case 354: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2592 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 7348 "parser.cpp"
    break;

  case 355: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2597 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 7359 "parser.cpp"
    break;

  case 356: /* admin_statement: ADMIN SHOW DATABASES  */
#line 2603 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
}
#line 7368 "parser.cpp"
    break;

  case 357: /* admin_statement: ADMIN SHOW DATABASE IDENTIFIER  */
#line 2607 "parser.y"
                                 {
    (yyval.admin_stmt) = new infinity::AdminStatement();
    (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.admin_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7380 "parser.cpp"
    break;

  case 358: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2614 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 7389 "parser.cpp"
    break;

  case 359: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2618 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 7398 "parser.cpp"
    break;

  case 360: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2622 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     ParserHelper::ToLower((yyvsp[0].str_value));
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7410 "parser.cpp"
    break;

  case 361: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2629 "parser.y"
                        {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
}
#line 7419 "parser.cpp"
    break;

  case 362: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2633 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
}
#line 7428 "parser.cpp"
    break;

  case 363: /* admin_statement: ADMIN SHOW SNAPSHOT IDENTIFIER  */
#line 2637 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSnapshot;
     ParserHelper::ToLower((yyvsp[0].str_value));
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7440 "parser.cpp"
    break;

  case 364: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2644 "parser.y"
                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7451 "parser.cpp"
    break;

  case 365: /* admin_statement: ADMIN EXPORT SNAPSHOT STRING TO STRING  */
#line 2650 "parser.y"
                                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kExportSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[-2].str_value);
     (yyval.admin_stmt)->export_path_ = (yyvsp[0].str_value);
     free((yyvsp[-2].str_value));
     free((yyvsp[0].str_value));
}
#line 7464 "parser.cpp"
    break;

  case 366: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2658 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7475 "parser.cpp"
    break;

  case 367: /* admin_statement: ADMIN SHOW NODES  */
#line 2664 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 7484 "parser.cpp"
    break;

  case 368: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2668 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7495 "parser.cpp"
    break;

  case 369: /* admin_statement: ADMIN SHOW NODE  */
#line 2674 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 7504 "parser.cpp"
    break;

  case 370: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2678 "parser.y"
                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7515 "parser.cpp"
    break;

  case 371: /* admin_statement: ADMIN SET ADMIN  */
#line 2684 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
}
#line 7525 "parser.cpp"
    break;

  case 372: /* admin_statement: ADMIN SET STANDALONE  */
#line 2689 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
}
#line 7535 "parser.cpp"
    break;

  case 373: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2694 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7547 "parser.cpp"
    break;

  case 374: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2701 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7561 "parser.cpp"
    break;

  case 375: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2710 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7575 "parser.cpp"
    break;

  case 376: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2720 "parser.y"
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
#line 7590 "parser.cpp"
    break;

  case 377: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2730 "parser.y"
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
#line 7607 "parser.cpp"
    break;

  case 378: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2742 "parser.y"
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
#line 7623 "parser.cpp"
    break;

  case 379: /* alter_statement: ALTER IDENTIFIER ON table_name with_index_param_list  */
#line 2753 "parser.y"
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
#line 7646 "parser.cpp"
    break;

  case 380: /* check_statement: CHECK SYSTEM  */
#line 2772 "parser.y"
                               {
    (yyval.check_stmt) = new infinity::CheckStatement();
    (yyval.check_stmt)->check_type_ = infinity::CheckStmtType::kSystem;
}
#line 7655 "parser.cpp"
    break;

  case 381: /* check_statement: CHECK TABLE table_name  */
#line 2776 "parser.y"
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
#line 7671 "parser.cpp"
    break;

  case 382: /* expr_array: expr_alias  */
#line 2792 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7680 "parser.cpp"
    break;

  case 383: /* expr_array: expr_array ',' expr_alias  */
#line 2796 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7689 "parser.cpp"
    break;

  case 384: /* insert_row_list: '(' expr_array ')'  */
#line 2801 "parser.y"
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
#line 7704 "parser.cpp"
    break;

  case 385: /* insert_row_list: insert_row_list ',' '(' expr_array ')'  */
#line 2811 "parser.y"
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
#line 7719 "parser.cpp"
    break;

  case 386: /* expr_alias: expr AS IDENTIFIER  */
#line 2833 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7730 "parser.cpp"
    break;

  case 387: /* expr_alias: expr  */
#line 2839 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7738 "parser.cpp"
    break;

  case 393: /* operand: '(' expr ')'  */
#line 2849 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 7746 "parser.cpp"
    break;

  case 394: /* operand: '(' select_without_paren ')'  */
#line 2852 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7757 "parser.cpp"
    break;

  case 395: /* operand: constant_expr  */
#line 2858 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7765 "parser.cpp"
    break;

  case 406: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2874 "parser.y"
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
#line 7785 "parser.cpp"
    break;

  case 407: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')'  */
#line 2890 "parser.y"
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
#line 7807 "parser.cpp"
    break;

  case 408: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' IGNORE INDEX  */
#line 2908 "parser.y"
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
#line 7828 "parser.cpp"
    break;

  case 409: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2926 "parser.y"
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
#line 7890 "parser.cpp"
    break;

  case 410: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2984 "parser.y"
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
#line 7944 "parser.cpp"
    break;

  case 411: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 3034 "parser.y"
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
#line 8002 "parser.cpp"
    break;

  case 412: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3088 "parser.y"
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
#line 8061 "parser.cpp"
    break;

  case 413: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 3146 "parser.y"
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
#line 8090 "parser.cpp"
    break;

  case 414: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 3171 "parser.y"
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
#line 8117 "parser.cpp"
    break;

  case 415: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 3194 "parser.y"
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
#line 8142 "parser.cpp"
    break;

  case 416: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3215 "parser.y"
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
#line 8167 "parser.cpp"
    break;

  case 417: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3236 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8181 "parser.cpp"
    break;

  case 418: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3245 "parser.y"
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
#line 8197 "parser.cpp"
    break;

  case 419: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 3257 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8209 "parser.cpp"
    break;

  case 420: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3264 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8223 "parser.cpp"
    break;

  case 421: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3274 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 8234 "parser.cpp"
    break;

  case 422: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 3280 "parser.y"
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
#line 8250 "parser.cpp"
    break;

  case 423: /* sub_search: match_vector_expr  */
#line 3292 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8258 "parser.cpp"
    break;

  case 424: /* sub_search: match_text_expr  */
#line 3295 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8266 "parser.cpp"
    break;

  case 425: /* sub_search: match_tensor_expr  */
#line 3298 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8274 "parser.cpp"
    break;

  case 426: /* sub_search: match_sparse_expr  */
#line 3301 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8282 "parser.cpp"
    break;

  case 427: /* sub_search: query_expr  */
#line 3304 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8290 "parser.cpp"
    break;

  case 428: /* sub_search: fusion_expr  */
#line 3307 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8298 "parser.cpp"
    break;

  case 429: /* sub_search_array: sub_search  */
#line 3311 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 8307 "parser.cpp"
    break;

  case 430: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3315 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 8316 "parser.cpp"
    break;

  case 431: /* function_expr: IDENTIFIER '(' ')'  */
#line 3320 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 8329 "parser.cpp"
    break;

  case 432: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3328 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 8342 "parser.cpp"
    break;

  case 433: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 3336 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 8356 "parser.cpp"
    break;

  case 434: /* function_expr: YEAR '(' expr ')'  */
#line 3345 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "year";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8368 "parser.cpp"
    break;

  case 435: /* function_expr: MONTH '(' expr ')'  */
#line 3352 "parser.y"
                     {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "month";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8380 "parser.cpp"
    break;

  case 436: /* function_expr: DAY '(' expr ')'  */
#line 3359 "parser.y"
                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "day";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8392 "parser.cpp"
    break;

  case 437: /* function_expr: HOUR '(' expr ')'  */
#line 3366 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "hour";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8404 "parser.cpp"
    break;

  case 438: /* function_expr: MINUTE '(' expr ')'  */
#line 3373 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "minute";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8416 "parser.cpp"
    break;

  case 439: /* function_expr: SECOND '(' expr ')'  */
#line 3380 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "second";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8428 "parser.cpp"
    break;

  case 440: /* function_expr: operand IS NOT NULLABLE  */
#line 3387 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8440 "parser.cpp"
    break;

  case 441: /* function_expr: operand IS NULLABLE  */
#line 3394 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8452 "parser.cpp"
    break;

  case 442: /* function_expr: NOT operand  */
#line 3401 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8464 "parser.cpp"
    break;

  case 443: /* function_expr: '-' operand  */
#line 3408 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8476 "parser.cpp"
    break;

  case 444: /* function_expr: '+' operand  */
#line 3415 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8488 "parser.cpp"
    break;

  case 445: /* function_expr: operand '-' operand  */
#line 3422 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8501 "parser.cpp"
    break;

  case 446: /* function_expr: operand '+' operand  */
#line 3430 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8514 "parser.cpp"
    break;

  case 447: /* function_expr: operand '*' operand  */
#line 3438 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8527 "parser.cpp"
    break;

  case 448: /* function_expr: operand '/' operand  */
#line 3446 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8540 "parser.cpp"
    break;

  case 449: /* function_expr: operand '%' operand  */
#line 3454 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8553 "parser.cpp"
    break;

  case 450: /* function_expr: operand '=' operand  */
#line 3462 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8566 "parser.cpp"
    break;

  case 451: /* function_expr: operand EQUAL operand  */
#line 3470 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8579 "parser.cpp"
    break;

  case 452: /* function_expr: operand NOT_EQ operand  */
#line 3478 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8592 "parser.cpp"
    break;

  case 453: /* function_expr: operand '<' operand  */
#line 3486 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8605 "parser.cpp"
    break;

  case 454: /* function_expr: operand '>' operand  */
#line 3494 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8618 "parser.cpp"
    break;

  case 455: /* function_expr: operand LESS_EQ operand  */
#line 3502 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8631 "parser.cpp"
    break;

  case 456: /* function_expr: operand GREATER_EQ operand  */
#line 3510 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8644 "parser.cpp"
    break;

  case 457: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3518 "parser.y"
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
#line 8679 "parser.cpp"
    break;

  case 458: /* function_expr: operand LIKE operand  */
#line 3548 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    // Add default escape character ('\')
    infinity::ConstantExpr* escape_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    escape_expr->str_value_ = strdup("\\");
    func_expr->arguments_->emplace_back(escape_expr);
    (yyval.expr_t) = func_expr;
}
#line 8696 "parser.cpp"
    break;

  case 459: /* function_expr: operand LIKE operand ESCAPE operand  */
#line 3560 "parser.y"
                                      {
    // Validate that escape character is a constant string of exactly one character
    infinity::ConstantExpr* escape_expr = (infinity::ConstantExpr*)(yyvsp[0].expr_t);
    if (   (yyvsp[0].expr_t)->type_ != infinity::ParsedExprType::kConstant
        || escape_expr->literal_type_ != infinity::LiteralType::kString
        || strlen(escape_expr->str_value_) != 1) {
        yyerror(&(yyloc), scanner, result, "ESCAPE clause must be exactly one character");
        YYERROR;
    }
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-4].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    // Use the provided escape character
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8719 "parser.cpp"
    break;

  case 460: /* function_expr: operand NOT LIKE operand  */
#line 3578 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    // Add default escape character ('\')
    infinity::ConstantExpr* escape_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    escape_expr->str_value_ = strdup("\\");
    func_expr->arguments_->emplace_back(escape_expr);
    (yyval.expr_t) = func_expr;
}
#line 8736 "parser.cpp"
    break;

  case 461: /* function_expr: operand NOT LIKE operand ESCAPE operand  */
#line 3590 "parser.y"
                                          {
    // Validate that escape character is a constant string of exactly one character
    infinity::ConstantExpr* escape_expr = (infinity::ConstantExpr*)(yyvsp[0].expr_t);
    if (   (yyvsp[0].expr_t)->type_ != infinity::ParsedExprType::kConstant
        || escape_expr->literal_type_ != infinity::LiteralType::kString
        || strlen(escape_expr->str_value_) != 1) {
        yyerror(&(yyloc), scanner, result, "ESCAPE clause must be exactly one character");
        YYERROR;
    }
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-5].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    // Use the provided escape character
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8759 "parser.cpp"
    break;

  case 462: /* conjunction_expr: expr AND expr  */
#line 3609 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8772 "parser.cpp"
    break;

  case 463: /* conjunction_expr: expr OR expr  */
#line 3617 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8785 "parser.cpp"
    break;

  case 464: /* between_expr: operand BETWEEN operand AND operand  */
#line 3626 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 8797 "parser.cpp"
    break;

  case 465: /* in_expr: operand IN '(' expr_array ')'  */
#line 3634 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8808 "parser.cpp"
    break;

  case 466: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3640 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8819 "parser.cpp"
    break;

  case 467: /* case_expr: CASE expr case_check_array END  */
#line 3647 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8830 "parser.cpp"
    break;

  case 468: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3653 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8842 "parser.cpp"
    break;

  case 469: /* case_expr: CASE case_check_array END  */
#line 3660 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8852 "parser.cpp"
    break;

  case 470: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3665 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8863 "parser.cpp"
    break;

  case 471: /* case_check_array: WHEN expr THEN expr  */
#line 3672 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 8875 "parser.cpp"
    break;

  case 472: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3679 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 8887 "parser.cpp"
    break;

  case 473: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3687 "parser.y"
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
#line 8904 "parser.cpp"
    break;

  case 474: /* cast_expr: PARSE_JSON '(' expr ')'  */
#line 3699 "parser.y"
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
#line 8922 "parser.cpp"
    break;

  case 475: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3713 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8933 "parser.cpp"
    break;

  case 476: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3719 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8944 "parser.cpp"
    break;

  case 477: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3725 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8956 "parser.cpp"
    break;

  case 478: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3732 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8968 "parser.cpp"
    break;

  case 479: /* column_expr: IDENTIFIER  */
#line 3740 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8980 "parser.cpp"
    break;

  case 480: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3747 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8992 "parser.cpp"
    break;

  case 481: /* column_expr: '*'  */
#line 3754 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 9002 "parser.cpp"
    break;

  case 482: /* column_expr: column_expr '.' '*'  */
#line 3759 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 9016 "parser.cpp"
    break;

  case 483: /* constant_expr: STRING  */
#line 3769 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9026 "parser.cpp"
    break;

  case 484: /* constant_expr: TRUE  */
#line 3774 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 9036 "parser.cpp"
    break;

  case 485: /* constant_expr: FALSE  */
#line 3779 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 9046 "parser.cpp"
    break;

  case 486: /* constant_expr: DOUBLE_VALUE  */
#line 3784 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9056 "parser.cpp"
    break;

  case 487: /* constant_expr: LONG_VALUE  */
#line 3789 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9066 "parser.cpp"
    break;

  case 488: /* constant_expr: JSON STRING  */
#line 3794 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kJson);
    const_expr->json_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9076 "parser.cpp"
    break;

  case 489: /* constant_expr: DATE STRING  */
#line 3799 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9086 "parser.cpp"
    break;

  case 490: /* constant_expr: TIME STRING  */
#line 3804 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9096 "parser.cpp"
    break;

  case 491: /* constant_expr: DATETIME STRING  */
#line 3809 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9106 "parser.cpp"
    break;

  case 492: /* constant_expr: TIMESTAMP STRING  */
#line 3814 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9116 "parser.cpp"
    break;

  case 493: /* constant_expr: INTERVAL interval_expr  */
#line 3819 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9124 "parser.cpp"
    break;

  case 494: /* constant_expr: interval_expr  */
#line 3822 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9132 "parser.cpp"
    break;

  case 495: /* constant_expr: common_array_expr  */
#line 3825 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9140 "parser.cpp"
    break;

  case 496: /* constant_expr: curly_brackets_expr  */
#line 3828 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9148 "parser.cpp"
    break;

  case 497: /* common_array_expr: array_expr  */
#line 3832 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9156 "parser.cpp"
    break;

  case 498: /* common_array_expr: subarray_array_expr  */
#line 3835 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9164 "parser.cpp"
    break;

  case 499: /* common_array_expr: sparse_array_expr  */
#line 3838 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9172 "parser.cpp"
    break;

  case 500: /* common_array_expr: empty_array_expr  */
#line 3841 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9180 "parser.cpp"
    break;

  case 501: /* common_sparse_array_expr: sparse_array_expr  */
#line 3845 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9188 "parser.cpp"
    break;

  case 502: /* common_sparse_array_expr: array_expr  */
#line 3848 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9196 "parser.cpp"
    break;

  case 503: /* common_sparse_array_expr: empty_array_expr  */
#line 3851 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9204 "parser.cpp"
    break;

  case 504: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3855 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9212 "parser.cpp"
    break;

  case 505: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3859 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 9222 "parser.cpp"
    break;

  case 506: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3864 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9231 "parser.cpp"
    break;

  case 507: /* sparse_array_expr: long_sparse_array_expr  */
#line 3869 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9239 "parser.cpp"
    break;

  case 508: /* sparse_array_expr: double_sparse_array_expr  */
#line 3872 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9247 "parser.cpp"
    break;

  case 509: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3876 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9255 "parser.cpp"
    break;

  case 510: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3880 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 9267 "parser.cpp"
    break;

  case 511: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3887 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9278 "parser.cpp"
    break;

  case 512: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3894 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9286 "parser.cpp"
    break;

  case 513: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3898 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 9298 "parser.cpp"
    break;

  case 514: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3905 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9309 "parser.cpp"
    break;

  case 515: /* empty_array_expr: '[' ']'  */
#line 3912 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 9317 "parser.cpp"
    break;

  case 516: /* curly_brackets_expr: unclosed_curly_brackets_expr '}'  */
#line 3916 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9325 "parser.cpp"
    break;

  case 517: /* curly_brackets_expr: '{' '}'  */
#line 3919 "parser.y"
          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
}
#line 9333 "parser.cpp"
    break;

  case 518: /* unclosed_curly_brackets_expr: '{' constant_expr  */
#line 3923 "parser.y"
                                                {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
    (yyval.const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
}
#line 9342 "parser.cpp"
    break;

  case 519: /* unclosed_curly_brackets_expr: unclosed_curly_brackets_expr ',' constant_expr  */
#line 3927 "parser.y"
                                                 {
    (yyvsp[-2].const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9351 "parser.cpp"
    break;

  case 520: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3932 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 9359 "parser.cpp"
    break;

  case 521: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3936 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 9367 "parser.cpp"
    break;

  case 522: /* array_expr: long_array_expr  */
#line 3940 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9375 "parser.cpp"
    break;

  case 523: /* array_expr: double_array_expr  */
#line 3943 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9383 "parser.cpp"
    break;

  case 524: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3947 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9391 "parser.cpp"
    break;

  case 525: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3951 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9401 "parser.cpp"
    break;

  case 526: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3956 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9410 "parser.cpp"
    break;

  case 527: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3961 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9418 "parser.cpp"
    break;

  case 528: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3965 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9428 "parser.cpp"
    break;

  case 529: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3970 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9437 "parser.cpp"
    break;

  case 530: /* interval_expr: LONG_VALUE SECONDS  */
#line 3975 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9448 "parser.cpp"
    break;

  case 531: /* interval_expr: LONG_VALUE SECOND  */
#line 3981 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9459 "parser.cpp"
    break;

  case 532: /* interval_expr: LONG_VALUE MINUTES  */
#line 3987 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9470 "parser.cpp"
    break;

  case 533: /* interval_expr: LONG_VALUE MINUTE  */
#line 3993 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9481 "parser.cpp"
    break;

  case 534: /* interval_expr: LONG_VALUE HOURS  */
#line 3999 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9492 "parser.cpp"
    break;

  case 535: /* interval_expr: LONG_VALUE HOUR  */
#line 4005 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9503 "parser.cpp"
    break;

  case 536: /* interval_expr: LONG_VALUE DAYS  */
#line 4011 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9514 "parser.cpp"
    break;

  case 537: /* interval_expr: LONG_VALUE DAY  */
#line 4017 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9525 "parser.cpp"
    break;

  case 538: /* interval_expr: LONG_VALUE MONTHS  */
#line 4023 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9536 "parser.cpp"
    break;

  case 539: /* interval_expr: LONG_VALUE MONTH  */
#line 4029 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9547 "parser.cpp"
    break;

  case 540: /* interval_expr: LONG_VALUE YEARS  */
#line 4035 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9558 "parser.cpp"
    break;

  case 541: /* interval_expr: LONG_VALUE YEAR  */
#line 4041 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9569 "parser.cpp"
    break;

  case 542: /* copy_option_list: copy_option  */
#line 4052 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 9578 "parser.cpp"
    break;

  case 543: /* copy_option_list: copy_option_list ',' copy_option  */
#line 4056 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 9587 "parser.cpp"
    break;

  case 544: /* copy_option: FORMAT JSON  */
#line 4061 "parser.y"
                          {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kFormat;
    (yyval.copy_option_t)->file_type_ = infinity::CopyFileType::kJSON;
}
#line 9597 "parser.cpp"
    break;

  case 545: /* copy_option: FORMAT IDENTIFIER  */
#line 4066 "parser.y"
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
#line 9631 "parser.cpp"
    break;

  case 546: /* copy_option: DELIMITER STRING  */
#line 4095 "parser.y"
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
#line 9646 "parser.cpp"
    break;

  case 547: /* copy_option: HEADER  */
#line 4105 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 9656 "parser.cpp"
    break;

  case 548: /* copy_option: OFFSET LONG_VALUE  */
#line 4110 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 9666 "parser.cpp"
    break;

  case 549: /* copy_option: LIMIT LONG_VALUE  */
#line 4115 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 9676 "parser.cpp"
    break;

  case 550: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 4120 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 9686 "parser.cpp"
    break;

  case 551: /* file_path: STRING  */
#line 4126 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 9694 "parser.cpp"
    break;

  case 552: /* if_exists: IF EXISTS  */
#line 4130 "parser.y"
                     { (yyval.bool_value) = true; }
#line 9700 "parser.cpp"
    break;

  case 553: /* if_exists: %empty  */
#line 4131 "parser.y"
         { (yyval.bool_value) = false; }
#line 9706 "parser.cpp"
    break;

  case 554: /* if_not_exists: IF NOT EXISTS  */
#line 4133 "parser.y"
                              { (yyval.bool_value) = true; }
#line 9712 "parser.cpp"
    break;

  case 555: /* if_not_exists: %empty  */
#line 4134 "parser.y"
         { (yyval.bool_value) = false; }
#line 9718 "parser.cpp"
    break;

  case 558: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 4149 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9731 "parser.cpp"
    break;

  case 559: /* if_not_exists_info: %empty  */
#line 4157 "parser.y"
         {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 9739 "parser.cpp"
    break;

  case 560: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 4161 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9747 "parser.cpp"
    break;

  case 561: /* with_index_param_list: %empty  */
#line 4164 "parser.y"
         {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 9755 "parser.cpp"
    break;

  case 562: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 4168 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9763 "parser.cpp"
    break;

  case 563: /* optional_table_properties_list: %empty  */
#line 4171 "parser.y"
         {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 9771 "parser.cpp"
    break;

  case 564: /* index_param_list: index_param  */
#line 4175 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 9780 "parser.cpp"
    break;

  case 565: /* index_param_list: index_param_list ',' index_param  */
#line 4179 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 9789 "parser.cpp"
    break;

  case 566: /* index_param: IDENTIFIER  */
#line 4184 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9800 "parser.cpp"
    break;

  case 567: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 4190 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9815 "parser.cpp"
    break;

  case 568: /* index_param: IDENTIFIER '=' STRING  */
#line 4200 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9830 "parser.cpp"
    break;

  case 569: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 4210 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 9843 "parser.cpp"
    break;

  case 570: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 4218 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 9856 "parser.cpp"
    break;

  case 571: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 4229 "parser.y"
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
#line 9914 "parser.cpp"
    break;

  case 572: /* index_info: '(' IDENTIFIER ')'  */
#line 4282 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 9926 "parser.cpp"
    break;

  case 573: /* index_info: '(' function_expr ')'  */
#line 4289 "parser.y"
                        {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondaryFunctional;
    (yyval.index_info_t)->function_expr_ = (yyvsp[-1].expr_t);
}
#line 9936 "parser.cpp"
    break;

  case 574: /* index_info: '(' function_expr ')' USING IDENTIFIER with_index_param_list  */
#line 4295 "parser.y"
                                                               {
ParserHelper::ToLower((yyvsp[-1].str_value));
    infinity::IndexType index_type = infinity::IndexType::kInvalid;
    if(strcmp((yyvsp[-1].str_value), "secondary") == 0){
        index_type = infinity::IndexType::kSecondary;
        free((yyvsp[-1].str_value));
    } else {
        free((yyvsp[-1].str_value));
        free((yyvsp[-4].expr_t));
        delete (yyvsp[0].with_index_param_list_t);
        yyerror(&yyloc, scanner, result, "functional index must be secondary index");
        YYERROR;
    }

    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondaryFunctional;
    (yyval.index_info_t)->function_expr_ = (yyvsp[-4].expr_t);
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
                    free((yyvsp[-4].expr_t));
                    yyerror(&yyloc, scanner, result, "Invalid cardinality value. Must be 'high' or 'low'");
                    YYERROR;
                }
            }
        }
    }
}
#line 9978 "parser.cpp"
    break;


#line 9982 "parser.cpp"

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

#line 4334 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

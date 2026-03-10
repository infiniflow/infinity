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
#define YYLAST   1718

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  234
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  120
/* YYNRULES -- Number of rules.  */
#define YYNRULES  572
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1284

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
    2558,  2570,  2583,  2587,  2592,  2597,  2603,  2607,  2611,  2618,
    2622,  2626,  2633,  2639,  2647,  2653,  2657,  2663,  2667,  2673,
    2678,  2683,  2690,  2699,  2709,  2719,  2731,  2742,  2761,  2765,
    2781,  2785,  2790,  2800,  2822,  2828,  2832,  2833,  2834,  2835,
    2836,  2838,  2841,  2847,  2850,  2851,  2852,  2853,  2854,  2855,
    2856,  2857,  2858,  2859,  2863,  2879,  2897,  2915,  2973,  3023,
    3077,  3135,  3160,  3183,  3204,  3225,  3234,  3246,  3253,  3263,
    3269,  3281,  3284,  3287,  3290,  3293,  3296,  3300,  3304,  3309,
    3317,  3325,  3334,  3341,  3348,  3355,  3362,  3369,  3376,  3383,
    3390,  3397,  3404,  3411,  3419,  3427,  3435,  3443,  3451,  3459,
    3467,  3475,  3483,  3491,  3499,  3507,  3537,  3549,  3567,  3579,
    3598,  3606,  3615,  3623,  3629,  3636,  3642,  3649,  3654,  3661,
    3668,  3676,  3688,  3702,  3708,  3714,  3721,  3729,  3736,  3743,
    3748,  3758,  3763,  3768,  3773,  3778,  3783,  3788,  3793,  3798,
    3803,  3808,  3811,  3814,  3817,  3821,  3824,  3827,  3830,  3834,
    3837,  3840,  3844,  3848,  3853,  3858,  3861,  3865,  3869,  3876,
    3883,  3887,  3894,  3901,  3905,  3908,  3912,  3916,  3921,  3925,
    3929,  3932,  3936,  3940,  3945,  3950,  3954,  3959,  3964,  3970,
    3976,  3982,  3988,  3994,  4000,  4006,  4012,  4018,  4024,  4030,
    4041,  4045,  4050,  4055,  4084,  4094,  4099,  4104,  4109,  4115,
    4119,  4120,  4122,  4123,  4125,  4126,  4138,  4146,  4150,  4153,
    4157,  4160,  4164,  4168,  4173,  4179,  4189,  4199,  4207,  4218,
    4273,  4280,  4286
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

#define YYPACT_NINF (-791)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-558)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    1190,    72,    38,   252,    66,    19,    66,   253,   325,  1116,
     163,    57,    64,   320,   115,    66,   132,    63,   260,   368,
     275,   111,     8,   288,    75,  -791,  -791,  -791,  -791,  -791,
    -791,  -791,  -791,   474,  -791,  -791,   284,  -791,  -791,  -791,
    -791,  -791,  -791,  -791,  -791,   246,   246,   246,   246,    92,
     349,    66,   301,   301,   301,   301,   301,   377,   133,   384,
      66,   -25,   399,   422,   458,   670,  -791,  -791,  -791,  -791,
    -791,  -791,  -791,   474,  -791,  -791,  -791,  -791,  -791,   241,
     406,   436,   492,    66,  -791,  -791,  -791,  -791,  -791,    28,
    -791,   136,   231,  -791,   530,  -791,  -791,    62,   513,  -791,
     534,  -791,  -791,  -791,   210,   238,    41,    61,  -791,   531,
      66,    66,   599,  -791,  -791,  -791,  -791,  -791,  -791,   555,
     393,  -791,   598,   419,   431,   314,  1319,   433,   628,   439,
     560,   452,   473,   475,    66,  -791,  -791,   459,   471,  -791,
      68,  -791,   624,  -791,  -791,     9,   613,  -791,   635,   638,
     710,    66,    66,    66,   713,   656,   662,   506,   661,   751,
      66,    66,    66,   753,  -791,   773,   785,   724,   788,   788,
     754,    55,   127,   144,  -791,   568,   664,   673,  -791,   499,
    -791,  -791,  -791,   810,  -791,   814,  -791,  -791,   788,  -791,
    -791,   815,  -791,  -791,  -791,  -791,  -791,   686,  -791,   688,
      66,   462,  -791,   760,   600,   132,   788,  -791,   819,  -791,
     658,  -791,   818,  -791,  -791,   827,  -791,   826,  -791,   830,
     832,  -791,   834,   786,   836,   646,   842,   847,   848,  -791,
    -791,  -791,  -791,  -791,     9,  -791,  -791,  -791,   754,   799,
     787,   779,   719,     0,  -791,   506,  -791,    66,   448,   856,
     204,  -791,  -791,  -791,  -791,  -791,   798,  -791,   648,   -17,
    -791,   754,  -791,  -791,   791,   793,   640,  -791,  -791,  1378,
     843,   649,   650,   487,   859,   869,   872,   874,   878,  -791,
    -791,   877,   659,   660,   663,   665,   674,   676,   678,   346,
     680,   681,   682,   950,   950,  -791,   647,    14,    18,   -28,
    -791,     1,   445,  -791,  -791,  -791,  -791,  -791,  -791,  -791,
    -791,  -791,  -791,  -791,  -791,  -791,   669,  -791,  -791,  -791,
     249,  -791,  -791,   279,  -791,   344,  -791,  -791,   -57,  -791,
    -791,   347,  -791,   354,  -791,  -791,  -791,  -791,  -791,  -791,
    -791,  -791,  -791,  -791,  -791,  -791,  -791,  -791,  -791,  -791,
    -791,  -791,  -791,   883,   881,  -791,  -791,  -791,  -791,  -791,
    -791,  -791,  -791,  -791,   811,   862,   863,   833,    66,   284,
    -791,  -791,  -791,   907,   285,  -791,   906,  -791,  -791,   837,
     358,  -791,   911,  -791,  -791,  -791,   692,   693,   -40,   754,
     754,   851,  -791,   917,     8,    59,   868,   699,   922,   924,
    -791,  -791,   318,   704,  -791,    66,   754,   785,  -791,   401,
     705,   706,   291,  -791,  -791,  -791,  -791,  -791,  -791,  -791,
    -791,  -791,  -791,  -791,  -791,   950,   707,  1188,   857,   754,
     754,   164,   438,  -791,  -791,  -791,  -791,  -791,  1378,  -791,
     754,   754,   754,   754,   754,   754,   933,   714,   723,   727,
     732,   954,   955,   754,   292,   292,   734,   123,  -791,   728,
    -791,  -791,  -791,  -791,  -791,  -791,   890,   754,   968,   754,
     754,    17,   747,    54,   950,   950,   950,   950,   950,   950,
     950,   950,   950,   950,   950,   950,   950,   950,    15,   758,
    -791,   984,  -791,   986,  -791,    34,  -791,   989,  -791,   991,
    -791,   957,   535,   772,  -791,   775,   776,  1000,  -791,   778,
    -791,  1011,  -791,   331,  1015,   852,   854,  -791,  -791,  -791,
     754,   948,   801,  -791,    70,   401,   754,  -791,  -791,    43,
    1346,   896,   805,   382,  -791,  -791,  -791,     8,  1039,   897,
    -791,  -791,  -791,  1028,   754,   807,  -791,   401,  -791,    20,
      20,   754,  -791,   392,   857,   876,   812,    96,   104,   478,
    -791,   754,   754,   159,   185,   187,   200,   225,   232,   961,
     754,    24,   754,  1035,   820,   396,   234,  -791,  -791,   609,
     788,  -791,  -791,  -791,   884,   821,   950,   647,   916,  -791,
    1211,  1211,   334,   334,   861,   789,  1211,   334,   334,   292,
     292,  -791,  -791,  -791,  -791,  -791,  -791,   816,  -791,   817,
    -791,  -791,  -791,  -791,  1045,  1049,  -791,  1053,   856,  1056,
    -791,  -791,  -791,  1054,  -791,  -791,  1057,  1061,   839,    22,
     886,   754,  -791,  -791,  -791,   401,  1065,  -791,  -791,  -791,
    -791,  -791,  -791,  -791,  -791,  -791,  -791,  -791,  -791,   845,
    -791,  -791,  -791,  -791,  -791,  -791,  -791,  -791,  -791,  -791,
    -791,  -791,   846,   849,   860,   866,   870,   879,   880,   882,
     289,   885,   856,  1041,    59,   474,   413,  1188,   850,  1082,
    -791,   483,   888,  1087,  1091,  1095,    33,  -791,  1096,   488,
    -791,   496,   500,  -791,   875,  -791,  1346,   754,  -791,   754,
     146,   173,  -791,  -791,  -791,  -791,  -791,  -791,   950,   -42,
    -791,   447,   113,   887,   139,   889,  -791,  1099,  -791,  -791,
    -791,  1030,   647,  1200,   891,   502,  -791,   950,   950,  1100,
    1106,   655,  1070,   902,   507,  -791,   512,   514,  -791,  1114,
    -791,  -791,     8,   894,   631,  -791,   106,  -791,   381,   724,
    -791,  -791,  1117,  1346,  1346,   549,   672,   709,   871,  1140,
    1382,   993,   996,  -791,  -791,   237,  -791,   994,   856,   516,
     905,   992,  -791,   963,   965,  -791,  -791,   754,  -791,  -791,
    -791,  -791,  -791,  -791,  -791,    20,  -791,  -791,  -791,   909,
     401,   162,  -791,   754,  1089,   754,   758,   908,  1133,   913,
     754,  -791,   918,   920,   926,   518,   950,  -791,  -791,  1188,
    -791,  1144,  -791,  1147,   524,  -791,  1053,  -791,  -791,  1056,
     559,     6,    22,  1097,  -791,  -791,  -791,  -791,  -791,  -791,
    1098,  -791,  1155,  -791,  -791,  -791,  -791,  -791,  -791,  -791,
    -791,   932,  1108,   520,   935,   526,  -791,   936,   938,   939,
     940,   941,   945,   946,   947,   959,  1101,   964,   966,   969,
     981,   982,   983,   985,   987,   988,   990,  1102,   997,   999,
    1001,  1002,  1003,  1009,  1010,  1013,  1021,  1023,  1118,  1024,
    1025,  1027,  1029,  1036,  1038,  1043,  1046,  1047,  1048,  1134,
    1050,  1051,  1052,  1060,  1066,  1068,  1071,  1072,  1075,  1076,
    1162,  1077,  1093,  1103,  1104,  1105,  1107,  1109,  1110,  1111,
    1112,  1164,  1119,  -791,  -791,    34,  -791,  1055,  1127,   542,
    -791,  1053,  1180,  1185,  1189,   546,  -791,  -791,  -791,   401,
    -791,   148,  1120,    10,  1122,  -791,  -791,  -791,  1123,  1206,
    1040,   401,  -791,    20,  -791,  -791,  -791,  -791,  -791,  -791,
    -791,  -791,  -791,  -791,  -791,  1270,  -791,   106,   631,    22,
      22,  1059,   381,  1235,  1238,  -791,  1291,  -791,  -791,  1346,
    1310,  1316,  1318,  1320,  1329,  1339,  1345,  1350,  1351,  1131,
    1353,  1361,  1362,  1363,  1364,  1366,  1367,  1368,  1369,  1370,
    1150,  1373,  1374,  1375,  1376,  1377,  1379,  1380,  1381,  1383,
    1385,  1157,  1386,  1387,  1388,  1389,  1390,  1391,  1392,  1393,
    1396,  1397,  1161,  1406,  1407,  1408,  1418,  1419,  1420,  1447,
    1454,  1455,  1457,  1237,  1472,  1473,  1477,  1478,  1479,  1480,
    1484,  1485,  1487,  1488,  1271,  1494,  -791,  1321,  1497,  -791,
     548,  -791,   811,   811,  -791,  1498,  1499,  1500,   259,  1279,
    -791,   554,  1501,  -791,  -791,  1444,   856,  -791,   754,   754,
    -791,  1282,  -791,  1284,  1285,  1286,  1289,  1304,  1305,  1306,
    1307,  1308,  1529,  1311,  1312,  1313,  1314,  1315,  1317,  1322,
    1323,  1324,  1325,  1530,  1326,  1327,  1328,  1330,  1331,  1332,
    1333,  1334,  1335,  1336,  1536,  1337,  1338,  1340,  1341,  1342,
    1343,  1344,  1347,  1348,  1349,  1538,  1352,  1354,  1355,  1356,
    1357,  1358,  1359,  1360,  1365,  1371,  1539,  1372,  1384,  1394,
    1395,  1398,  1399,  1400,  1401,  1402,  1403,  1540,  1404,  -791,
    -791,  -791,  -791,  -791,  1405,  1409,  1410,   913,  -791,  -791,
     564,   754,   556,   839,   401,  -791,  -791,  -791,  -791,  -791,
    -791,  -791,  -791,  -791,  -791,  1412,  -791,  -791,  -791,  -791,
    -791,  -791,  -791,  -791,  -791,  -791,  1413,  -791,  -791,  -791,
    -791,  -791,  -791,  -791,  -791,  -791,  -791,  1414,  -791,  -791,
    -791,  -791,  -791,  -791,  -791,  -791,  -791,  -791,  1415,  -791,
    -791,  -791,  -791,  -791,  -791,  -791,  -791,  -791,  -791,  1416,
    -791,  -791,  -791,  -791,  -791,  -791,  -791,  -791,  -791,  -791,
    1417,  -791,  1543,  1551,    47,  1421,  1422,  1559,  1565,  -791,
    -791,  -791,   401,  -791,  -791,  -791,  -791,  -791,  -791,  -791,
    1423,  1424,   913,   811,  -791,  1566,   717,   101,  1426,  1572,
    1427,  -791,   767,  1571,  -791,   913,   811,   913,   -24,  1573,
    -791,  1541,  1428,  -791,  1429,  1548,  1550,  -791,  -791,  -791,
      -5,   -49,  -791,  1431,  1553,  1554,  -791,  1555,  1556,  1587,
    -791,  1432,  -791,  1433,  1434,  1593,  1596,   811,  1435,  1436,
    -791,   811,  -791,  -791
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     237,     0,     0,     0,     0,     0,     0,     0,   237,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   237,     0,   555,     3,     5,    10,    12,    13,
      11,     6,     7,     9,   182,   181,     0,     8,    14,    15,
      16,    17,    18,    19,    20,   553,   553,   553,   553,   553,
       0,     0,   551,   551,   551,   551,   551,     0,   230,     0,
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
     201,     0,    34,     0,    35,   166,   556,     0,     0,     0,
     237,   550,   173,   175,   174,   176,     0,   231,     0,   215,
     170,     0,   162,   549,     0,     0,   477,   481,   484,   485,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   482,
     483,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   479,   237,     0,     0,     0,
     380,   385,   386,   400,   398,   401,   399,   402,   403,   395,
     390,   389,   388,   396,   397,   387,   394,   393,   493,   496,
       0,   497,   505,     0,   506,     0,   498,   494,     0,   495,
     520,     0,   521,     0,   492,   315,   317,   316,   313,   314,
     320,   322,   321,   318,   319,   325,   327,   326,   323,   324,
     290,   296,   301,     0,     0,   270,   269,   275,   265,   266,
     283,   286,   304,   298,   559,     0,     0,     0,     0,     0,
     239,   312,   362,     0,   353,   358,     0,   366,   361,     0,
       0,   368,     0,   333,   334,   332,     0,     0,   207,     0,
       0,   203,   552,     0,   237,     0,     0,     0,     0,     0,
     330,   160,     0,     0,   164,     0,     0,     0,   169,   214,
       0,     0,     0,   529,   528,   531,   530,   533,   532,   535,
     534,   537,   536,   539,   538,     0,     0,   440,   237,     0,
       0,     0,     0,   486,   487,   488,   489,   490,     0,   491,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   442,   441,     0,     0,   526,   523,
     513,   503,   508,   511,   515,   516,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     502,     0,   507,     0,   510,     0,   514,     0,   522,     0,
     525,   276,   271,     0,   377,     0,     0,     0,   336,     0,
     371,     0,   354,     0,     0,     0,     0,   364,   190,   189,
       0,   209,   192,   194,   199,   200,     0,   188,    33,    37,
       0,     0,     0,     0,    43,    47,    48,   237,     0,    41,
     329,   328,   165,     0,     0,   163,   177,   172,   171,     0,
       0,     0,   429,     0,   237,     0,     0,     0,     0,     0,
     467,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   213,     0,     0,   392,   391,     0,
       0,   381,   384,   460,   461,     0,     0,   237,     0,   439,
     449,   450,   453,   454,     0,   456,   448,   451,   452,   444,
     443,   445,   446,   447,   478,   480,   504,     0,   509,     0,
     512,   517,   524,   527,     0,     0,   272,     0,     0,     0,
     374,   240,   355,     0,   338,   363,     0,     0,   206,     0,
     211,     0,   197,   198,   196,   202,     0,    55,    56,    59,
      60,    57,    58,    61,    62,    78,    63,    65,    64,    81,
      68,    69,    70,    66,    67,    71,    72,    73,    74,    75,
      76,    77,     0,     0,     0,     0,     0,     0,     0,     0,
     559,     0,     0,   561,     0,    40,   477,     0,   395,     0,
     161,     0,     0,     0,     0,     0,     0,   545,     0,     0,
     540,     0,     0,   430,     0,   473,     0,     0,   465,     0,
       0,     0,   437,   436,   435,   434,   433,   432,     0,     0,
     477,     0,     0,     0,     0,     0,   419,     0,   472,   519,
     518,     0,   237,   458,     0,     0,   438,     0,     0,     0,
       0,   277,   273,   564,     0,   562,     0,     0,    45,   339,
     372,   373,   237,   208,   224,   226,   235,   227,     0,   215,
     195,    39,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   153,   154,   157,   150,   157,     0,     0,
       0,    36,    44,   570,   571,    42,   382,     0,   547,   546,
     544,   543,   542,   548,   180,     0,   178,   431,   474,     0,
     469,     0,   468,     0,     0,     0,     0,     0,     0,   213,
       0,   417,     0,     0,     0,     0,     0,   475,   463,   462,
     457,     0,   278,     0,     0,   558,     0,   376,   375,     0,
       0,     0,     0,     0,   244,   245,   246,   247,   243,   248,
       0,   233,     0,   228,   423,   421,   424,   422,   425,   426,
     427,   210,   219,     0,     0,     0,    53,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   155,   152,     0,   151,    50,    49,     0,
     159,     0,     0,     0,     0,     0,   541,   471,   466,   470,
     455,     0,     0,     0,     0,   499,   501,   500,   213,     0,
       0,   212,   420,     0,   476,   464,   459,   279,   274,   565,
     566,   568,   567,   563,    46,     0,   340,   235,   225,     0,
       0,   232,     0,     0,   217,    80,     0,   148,   149,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   156,     0,     0,   158,
       0,    38,   559,   559,   383,     0,     0,     0,     0,     0,
     418,     0,   341,   229,   241,     0,     0,   428,     0,     0,
     191,     0,    54,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    52,
      51,   560,   569,   572,     0,     0,   213,   213,   415,   179,
       0,     0,     0,   218,   216,    79,    85,    86,    83,    84,
      87,    88,    89,    90,    91,     0,    82,   129,   130,   127,
     128,   131,   132,   133,   134,   135,     0,   126,    96,    97,
      94,    95,    98,    99,   100,   101,   102,     0,    93,   107,
     108,   105,   106,   109,   110,   111,   112,   113,     0,   104,
     140,   141,   138,   139,   142,   143,   144,   145,   146,     0,
     137,   118,   119,   116,   117,   120,   121,   122,   123,   124,
       0,   115,     0,     0,     0,     0,     0,     0,     0,   343,
     342,   348,   242,   234,    92,   136,   103,   114,   147,   125,
     213,     0,   213,   559,   416,   349,   344,     0,     0,     0,
       0,   414,     0,     0,   345,   213,   559,   213,   559,     0,
     350,   346,     0,   410,     0,     0,     0,   413,   351,   347,
     559,   404,   412,     0,     0,     0,   409,     0,     0,     0,
     408,     0,   406,     0,     0,     0,     0,   559,     0,     0,
     411,   559,   405,   407
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -791,  -791,  -791,  1460,  1537,   294,  -791,  -791,   927,  -590,
    -791,  -682,  -791,   838,   840,  -791,  -608,   319,   321,  1425,
    -791,   352,  -791,  1197,   369,   379,    -6,  1583,   -19,  1239,
    1411,   -69,  -791,  -791,   975,  -791,  -791,  -791,  -791,  -791,
    -791,  -791,  -790,  -251,  -791,  -791,  -791,  -791,   790,  -148,
      42,   652,  -791,  -791,  1430,  -791,  -791,   388,   389,   397,
     398,   404,  -791,  -791,  -791,  -237,  -791,  1146,  -261,  -263,
    -727,  -711,  -707,  -705,  -704,  -703,   653,  -791,  1073,  -791,
    -791,  -791,  -791,  1183,  -791,  -791,  1062,  -294,  -292,  -791,
    -791,  -791,   822,  -791,  -791,  -791,  -791,   825,  -791,  -791,
    1125,  1124,   867,  -791,  -791,  -791,  -791,  1437,  -539,   864,
    -156,   532,   611,  -791,  -791,  -664,  -791,   697,   828,  -791
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    23,    24,    25,    66,    26,   533,   737,   534,   535,
     845,   670,   765,   766,   917,   536,   402,    27,    28,   250,
      29,    30,   259,   260,    31,    32,    33,    34,    35,   147,
     235,   148,   240,   522,   523,   634,   391,   527,   238,   521,
     630,   749,   715,   262,  1060,   964,   145,   743,   744,   745,
     746,   833,    36,   120,   121,   747,   830,    37,    38,    39,
      40,    41,    42,    43,    44,   299,   545,   300,   301,   302,
     303,   304,   305,   306,   307,   308,   840,   841,   309,   310,
     311,   312,   313,   432,   314,   315,   316,   317,   318,   934,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     462,   463,   329,   330,   331,   332,   333,   334,   689,   690,
     264,   159,   150,   141,   155,   504,   771,   734,   735,   539
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     409,   388,    73,   138,   465,   461,   767,   427,   408,   940,
     736,   691,   431,   265,   789,   458,   459,   146,   604,   458,
     459,   834,   267,   268,   269,    58,   142,   710,   143,   738,
     454,   455,   360,   144,   180,   457,   781,   835,   267,   268,
     269,   836,   520,   837,   838,   839,    59,   196,    61,   261,
     371,   168,   169,  1232,   503,   394,   468,   118,   335,    73,
     336,   337,   530,   142,   769,   143,   187,   198,  -554,    58,
     144,   844,   846,   503,    51,     1,   236,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    16,  1267,   683,   684,
     111,    11,   585,   157,    12,    60,    13,    14,    15,   685,
     686,   687,   167,   112,    45,    46,    47,  1245,   274,   831,
      48,    49,  1255,   800,   197,   469,   470,   338,   117,  1268,
     275,   276,   277,   782,   274,   179,   278,   588,   524,   525,
     340,  1264,   341,   342,   199,   119,   275,   276,   277,   188,
     632,   633,   278,   799,  1256,   547,    16,   345,  1049,   346,
     347,   696,   201,   202,  -557,   279,   280,   281,   469,   470,
     919,   832,   427,  1265,   149,   387,   109,   800,   557,   558,
     495,   279,   280,   281,   496,   553,   229,   586,   636,   563,
     564,   565,   566,   567,   568,   795,   589,   467,   531,   343,
     532,   697,   576,   243,   244,   245,   110,   606,   466,   467,
     170,   611,   253,   254,   255,   800,   348,   688,   583,   584,
     407,   590,   591,   592,   593,   594,   595,   596,   597,   598,
     599,   600,   601,   602,   603,   395,   339,   469,   470,   954,
     792,   404,   957,    22,   234,   834,   605,   181,   122,   460,
      17,   403,   364,   460,   297,   295,   928,   742,   297,   464,
     298,   835,   430,   469,   470,   836,    18,   837,   838,   839,
     793,   469,   470,  1137,   297,   635,   298,   123,    19,    50,
     469,   470,   124,   136,   125,   677,   126,   456,    20,    21,
     469,   470,    16,   628,    52,    53,    54,  1062,   139,   397,
      55,    56,   146,    22,   266,   267,   268,   269,   344,   140,
     700,   701,    67,   469,   470,   469,   470,   681,   134,   709,
     761,   712,   182,   183,   692,   349,   469,   470,   149,   469,
     470,   469,   470,   723,   511,   800,   135,    68,    65,    69,
     469,   470,     1,   512,     2,     3,     4,     5,     6,     7,
     797,     9,   469,   470,   469,   470,  1215,  1216,    11,   578,
     725,    12,   156,    13,    14,    15,   551,   469,   470,    67,
      70,   165,   761,   623,   270,   271,   762,   503,   763,   764,
     524,   915,   624,   158,   272,  1045,   273,    71,  1132,  1133,
     164,   274,   469,   470,    68,   702,    69,    72,   529,   469,
     470,   469,   470,   275,   276,   277,    74,    75,   166,   278,
     131,   132,   171,    16,  1051,    76,    77,   184,   185,   556,
     508,   703,    78,   704,   191,   192,   193,    70,   762,   133,
     763,   764,    62,    63,   721,   172,   705,    64,   279,   280,
     281,   282,   127,   283,    71,   284,   790,   285,   791,   286,
    1238,   287,  1240,   194,    72,   794,   195,   546,  1142,    57,
     288,   706,   128,    74,    75,  1252,   129,  1254,   707,   130,
     718,   173,    76,    77,   809,   810,   175,   473,   447,    78,
     448,   449,   365,   289,   450,   290,   489,   291,   490,   176,
     398,   399,   113,   114,   115,   805,   366,   367,  -558,  -558,
     266,   267,   268,   269,   142,   178,   143,    17,   842,   400,
     116,   144,   209,   210,   932,   292,   491,   211,   492,   177,
     293,   294,   295,   485,   486,   487,   296,   552,   471,   189,
     472,   297,   560,   298,   561,    19,   562,   949,   950,   951,
     952,   675,   929,   186,   931,   694,    21,   190,   353,   941,
     925,   354,   355,   946,   542,   543,   356,   357,   515,   516,
      22,  -558,  -558,   483,   484,   485,   486,   487,   469,   470,
     270,   271,   698,   289,   699,   290,   562,   291,   724,  1241,
     272,   493,   273,   494,   497,   430,   498,   274,   473,   615,
     616,   499,  1253,   500,  1257,   160,   161,   162,   163,   275,
     276,   277,   955,   200,   956,   278,  1266,   474,   475,   476,
     477,   478,   203,  1217,   206,   479,  1218,  1219,   673,   674,
     204,  1220,  1221,  1280,   719,   720,   207,  1283,   693,   467,
     205,  1036,   716,   717,   279,   280,   281,   282,   208,   283,
     222,   284,   223,   285,   224,   286,   225,   287,   412,   773,
     847,   848,   849,   850,   851,   233,   288,   852,   853,   226,
     266,   267,   268,   269,   854,   855,   856,   151,   152,   153,
     154,   480,   481,   482,   483,   484,   485,   486,   487,   289,
     227,   290,   228,   291,   796,   488,   857,     1,   237,     2,
       3,     4,     5,     6,     7,   230,     9,   823,  -249,   824,
     825,   826,   827,    11,   828,   829,    12,   231,    13,    14,
      15,   292,   239,   804,   811,   812,   293,   294,   295,   776,
     467,   241,   296,   242,   784,   785,   246,   297,   247,   298,
     270,   271,   786,   785,   248,    16,   787,   467,   808,   467,
     272,   249,   273,   815,   816,   251,   821,   274,   817,   543,
     818,   819,   920,   543,   945,   467,   965,   966,    16,   275,
     276,   277,   968,   969,   252,   278,   256,   266,   267,   268,
     269,  1243,  1244,   858,   859,   860,   861,   862,  1039,   543,
     863,   864,  1044,   467,  1131,   816,   257,   865,   866,   867,
    1139,   785,  1223,   543,   279,   280,   281,   282,   258,   283,
     261,   284,   263,   285,   350,   286,   351,   287,  1144,   868,
     869,   870,   871,   872,   873,   352,   288,   874,   875,  1249,
    1250,  1054,  1055,   358,   876,   877,   878,   359,   362,   361,
     363,  1143,   368,   372,   374,   369,   373,   270,   271,   289,
     375,   290,   376,   291,   377,   378,   879,   272,   379,   273,
     381,   380,    17,   382,   274,   383,   266,   267,   268,   269,
     384,   385,   389,   392,   393,   390,   275,   276,   277,   401,
     405,   292,   278,   433,   406,   412,   293,   294,   295,   410,
      19,   411,   296,   434,   428,   429,   435,   297,   436,   298,
    1222,    21,   437,   438,   440,   441,   501,   502,   442,   503,
     443,   279,   280,   281,   282,    22,   283,   488,   284,   444,
     285,   445,   286,   446,   287,   451,   452,   453,   505,   506,
     507,   510,   513,   288,   514,   517,   425,   426,   518,   519,
     526,   528,   473,   537,   538,   540,   272,   541,   273,   544,
     549,   550,   554,   274,   555,    16,   289,   569,   290,   570,
     291,  -558,  -558,   476,   477,   275,   276,   277,   571,  -558,
     728,   278,   572,   266,   267,   268,   269,   573,   574,   575,
     577,   579,   880,   881,   882,   883,   884,   580,   292,   885,
     886,   582,   587,   293,   294,   295,   887,   888,   889,   296,
     279,   280,   281,   282,   297,   283,   298,   284,   297,   285,
     607,   286,   609,   287,   473,   612,   613,   617,   890,   614,
     618,   619,   288,   620,   621,  -558,   481,   482,   483,   484,
     485,   486,   487,   474,   475,   476,   477,   622,   727,   625,
     626,   479,   627,   425,   629,   289,   671,   290,   631,   291,
     672,   680,   679,   272,   682,   273,   586,   708,   695,   713,
     274,   469,   676,   267,   268,   269,   722,   714,   726,   729,
     730,   731,   275,   276,   277,   732,   733,   292,   278,   530,
     739,   740,   293,   294,   295,   741,   467,   748,   296,   751,
     752,   753,   770,   297,   754,   298,   774,   480,   481,   482,
     483,   484,   485,   486,   487,   755,   775,   279,   280,   281,
     282,   756,   283,   778,   284,   757,   285,   779,   286,   780,
     287,   788,   783,   802,   758,   759,   720,   760,   803,   288,
     768,   719,   425,   777,   798,   801,   813,   807,   814,    79,
     820,   822,   272,   843,   273,   913,   914,   922,   915,   274,
     921,   923,   289,   924,   290,   927,   291,   938,   933,    80,
     939,   275,   276,   277,   942,   943,    81,   278,    82,    83,
     947,    84,   944,   948,   959,   960,    85,    86,   961,   962,
     963,   967,   555,   970,   292,   971,   972,   973,   974,   293,
     294,   295,   975,   976,   977,   296,   279,   280,   281,   282,
     297,   283,   298,   284,  1041,   285,   978,   286,  1042,   287,
    1037,   980,  1043,   981,   979,   990,   982,     1,   288,     2,
       3,     4,     5,     6,     7,     8,     9,    10,   983,   984,
     985,  1001,   986,    11,   987,   988,    12,   989,    13,    14,
      15,   289,   473,   290,   991,   291,   992,  1012,   993,   994,
     995,   891,   892,   893,   894,   895,   996,   997,   896,   897,
     998,   474,   475,   476,   477,   898,   899,   900,   999,   479,
    1000,  1002,  1003,   292,  1004,  1023,  1005,  1034,   293,   294,
     295,   555,  1038,  1006,   296,  1007,  1050,   901,    16,   297,
    1008,   298,   800,  1009,  1010,  1011,  1052,  1013,  1014,  1015,
      87,    88,    89,    90,  1056,    91,    92,  1016,  1058,    93,
      94,    95,  1059,  1017,    96,  1018,    97,  1061,  1019,  1020,
      98,    99,  1021,  1022,  1024,   480,   481,   482,   483,   484,
     485,   486,   487,   100,   101,   930,  1063,   102,   103,   104,
    1025,   473,  1064,   105,  1065,  1129,  1066,   106,   107,   108,
    1026,  1027,  1028,   473,  1029,  1067,  1030,  1031,  1032,  1033,
     474,   475,   476,   477,   473,  1068,  1035,  1046,   479,  1047,
    1048,  1069,  -558,  -558,   476,   477,  1070,  1071,  1072,  1073,
    -558,   806,    17,  -558,  -558,   476,   477,  1074,  1075,  1076,
    1077,  -558,  1078,  1079,  1080,  1081,  1082,  1083,    18,  1084,
    1085,  1086,  1087,  1088,  1094,  1089,  1090,  1091,  1105,  1092,
      19,  1093,  1095,  1096,  1097,  1098,  1099,  1100,  1101,  1102,
      20,    21,  1103,  1104,   480,   481,   482,   483,   484,   485,
     486,   487,  1106,  1107,  1108,    22,  -558,   481,   482,   483,
     484,   485,   486,   487,  1109,  1110,  1111,  -558,   481,   482,
     483,   484,   485,   486,   487,   637,   638,   639,   640,   641,
     642,   643,   644,   645,   646,   647,   648,   649,   650,   651,
     652,   653,   654,  1112,   655,   656,   657,   658,   659,   660,
    1113,  1114,   661,  1115,  1116,   662,   663,   664,   665,   666,
     667,   668,   669,   902,   903,   904,   905,   906,  1117,  1118,
     907,   908,   212,  1119,  1120,  1121,  1122,   909,   910,   911,
    1123,  1124,   213,  1125,  1126,   214,   215,   216,  1127,   217,
    1128,  1130,  1134,  1135,  1136,  1138,  1141,  1140,  1145,   912,
    1146,  1147,  1148,   218,   219,  1149,   220,   221,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
    1150,  1151,  1152,  1153,  1154,  1155,  1166,  1156,  1157,  1158,
    1159,  1160,  1177,  1161,  1188,  1199,  1210,  1230,  1162,  1163,
    1164,  1165,  1167,  1168,  1169,  1231,  1170,  1171,  1172,  1173,
    1174,  1175,  1176,  1178,  1179,  1235,  1180,  1181,  1182,  1183,
    1184,  1236,  1242,  1185,  1186,  1187,  1247,  1251,  1189,  1258,
    1190,  1191,  1192,  1193,  1194,  1195,  1196,  1262,  1259,  1263,
    1274,  1197,  1270,  1271,  1272,  1273,  1278,  1198,  1200,  1279,
     232,   772,   174,   916,   548,   137,   750,   918,   509,  1053,
    1201,   678,   958,   581,   559,  1057,   608,   610,  1040,   935,
    1202,  1203,   936,     0,  1204,  1205,  1206,  1207,  1208,  1209,
    1211,     0,  1212,   711,     0,   370,  1213,  1214,  1224,  1225,
    1226,  1227,  1228,  1229,   953,   386,     0,  1233,  1234,   926,
    1237,  1239,  1246,  1248,  1260,  1261,  1269,  1275,  1276,     0,
    1277,  1281,  1282,     0,   937,     0,     0,     0,     0,     0,
     396,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   439
};

static const yytype_int16 yycheck[] =
{
     261,   238,     8,    22,   298,   297,   670,   270,   259,   799,
     618,   550,   273,   169,   696,     5,     6,     8,     3,     5,
       6,   748,     4,     5,     6,     3,    20,     3,    22,   619,
     293,   294,   188,    27,     6,   296,     3,   748,     4,     5,
       6,   748,    82,   748,   748,   748,     4,     6,     6,    66,
     206,    76,    77,     6,    78,    55,    55,    15,     3,    65,
       5,     6,     3,    20,   672,    22,     4,     6,     0,     3,
      27,   753,   754,    78,    36,     7,   145,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    78,   136,    68,    69,
      33,    23,    75,    51,    26,    76,    28,    29,    30,    79,
      80,    81,    60,    39,    32,    33,    34,     6,    90,     3,
      38,    39,   136,    66,    73,   157,   158,    62,     3,   168,
     102,   103,   104,    90,    90,    83,   108,    73,   389,   390,
       3,   136,     5,     6,    73,     3,   102,   103,   104,    77,
      70,    71,   108,     4,   168,   406,    78,     3,   938,     5,
       6,    55,   110,   111,    62,   137,   138,   139,   157,   158,
     768,    55,   425,   168,    72,   234,     3,    66,   429,   430,
     227,   137,   138,   139,   231,   412,   134,   160,   135,   440,
     441,   442,   443,   444,   445,   227,   132,   227,   129,    62,
     131,    87,   453,   151,   152,   153,    33,   489,   226,   227,
     225,   495,   160,   161,   162,    66,    62,   187,   469,   470,
     227,   474,   475,   476,   477,   478,   479,   480,   481,   482,
     483,   484,   485,   486,   487,   225,   171,   157,   158,   819,
      84,   250,   226,   225,   225,   962,   221,   209,   175,   229,
     172,    37,   200,   229,   230,   221,    84,   225,   230,   231,
     232,   962,    88,   157,   158,   962,   188,   962,   962,   962,
      87,   157,   158,     4,   230,   526,   232,     7,   200,   197,
     157,   158,    12,   162,    14,   538,    16,   296,   210,   211,
     157,   158,    78,   520,    32,    33,    34,   969,     0,   247,
      38,    39,     8,   225,     3,     4,     5,     6,   171,   224,
     561,   562,     8,   157,   158,   157,   158,   544,    33,   570,
      73,   572,   176,   177,   551,   171,   157,   158,    72,   157,
     158,   157,   158,   586,    39,    66,    51,     8,     3,     8,
     157,   158,     7,    48,     9,    10,    11,    12,    13,    14,
     227,    16,   157,   158,   157,   158,  1136,  1137,    23,   226,
     587,    26,     3,    28,    29,    30,    65,   157,   158,    65,
       8,   228,    73,    32,    73,    74,   129,    78,   131,   132,
     631,   134,    41,    72,    83,   227,    85,     8,  1042,  1043,
       3,    90,   157,   158,    65,   226,    65,     8,   394,   157,
     158,   157,   158,   102,   103,   104,     8,     8,    14,   108,
      32,    33,     3,    78,   943,     8,     8,   176,   177,   428,
     368,   226,     8,   226,   204,   205,   206,    65,   129,    51,
     131,   132,   169,   170,   580,     3,   226,   174,   137,   138,
     139,   140,   172,   142,    65,   144,   697,   146,   699,   148,
    1230,   150,  1232,   205,    65,   708,   208,   405,  1056,   197,
     159,   226,   192,    65,    65,  1245,   196,  1247,   226,   199,
     226,     3,    65,    65,   727,   728,   225,   133,   122,    65,
     124,   125,    10,   182,   128,   184,   227,   186,   229,    73,
      32,    33,   162,   163,   164,   722,    24,    25,   154,   155,
       3,     4,     5,     6,    20,     3,    22,   172,   749,    51,
     180,    27,   188,   189,   796,   214,   227,   193,   229,    73,
     219,   220,   221,   221,   222,   223,   225,   226,    73,     6,
      75,   230,    84,   232,    86,   200,    88,     3,     4,     5,
       6,   537,   793,     3,   795,   554,   211,     3,    39,   800,
     777,    42,    43,   806,   226,   227,    47,    48,   190,   191,
     225,   217,   218,   219,   220,   221,   222,   223,   157,   158,
      73,    74,    84,   182,    86,   184,    88,   186,   587,  1233,
      83,   227,    85,   229,   227,    88,   229,    90,   133,    44,
      45,   227,  1246,   229,  1248,    53,    54,    55,    56,   102,
     103,   104,    33,    62,    35,   108,  1260,   152,   153,   154,
     155,   156,     3,    39,     6,   160,    42,    43,   226,   227,
      55,    47,    48,  1277,     5,     6,   197,  1281,   226,   227,
     227,   915,   226,   227,   137,   138,   139,   140,   197,   142,
     197,   144,     4,   146,   195,   148,    76,   150,   225,   226,
      91,    92,    93,    94,    95,    21,   159,    98,    99,   197,
       3,     4,     5,     6,   105,   106,   107,    46,    47,    48,
      49,   216,   217,   218,   219,   220,   221,   222,   223,   182,
     197,   184,   197,   186,   227,   228,   127,     7,    65,     9,
      10,    11,    12,    13,    14,   226,    16,    56,    57,    58,
      59,    60,    61,    23,    63,    64,    26,   226,    28,    29,
      30,   214,    67,   722,    49,    50,   219,   220,   221,   226,
     227,    73,   225,     3,   226,   227,     3,   230,    62,   232,
      73,    74,   226,   227,    62,    78,   226,   227,   226,   227,
      83,   225,    85,   226,   227,    74,   742,    90,   226,   227,
     226,   227,   226,   227,   226,   227,   226,   227,    78,   102,
     103,   104,   226,   227,     3,   108,     3,     3,     4,     5,
       6,    44,    45,    91,    92,    93,    94,    95,   226,   227,
      98,    99,   226,   227,   226,   227,     3,   105,   106,   107,
     226,   227,   226,   227,   137,   138,   139,   140,     3,   142,
      66,   144,     4,   146,   226,   148,   132,   150,  1059,   127,
      91,    92,    93,    94,    95,   132,   159,    98,    99,    42,
      43,   959,   960,     3,   105,   106,   107,     3,   132,     4,
     132,  1058,    62,     4,     6,   225,   168,    73,    74,   182,
       3,   184,     6,   186,     4,     3,   127,    83,     4,    85,
       4,    55,   172,   197,    90,     3,     3,     4,     5,     6,
       3,     3,    53,    74,   135,    68,   102,   103,   104,     3,
      62,   214,   108,     4,   216,   225,   219,   220,   221,    78,
     200,    78,   225,     4,   225,   225,     4,   230,     4,   232,
    1141,   211,     4,     6,   225,   225,     3,     6,   225,    78,
     225,   137,   138,   139,   140,   225,   142,   228,   144,   225,
     146,   225,   148,   225,   150,   225,   225,   225,    46,    46,
      77,     4,     6,   159,    77,     4,    73,    74,   226,   226,
      69,     4,   133,    55,   225,     3,    83,     3,    85,   225,
     225,   225,   225,    90,    73,    78,   182,     4,   184,   225,
     186,   152,   153,   154,   155,   102,   103,   104,   225,   160,
     161,   108,   225,     3,     4,     5,     6,   225,     4,     4,
     226,   233,    91,    92,    93,    94,    95,    77,   214,    98,
      99,     3,   225,   219,   220,   221,   105,   106,   107,   225,
     137,   138,   139,   140,   230,   142,   232,   144,   230,   146,
       6,   148,     6,   150,   133,     6,     5,   225,   127,    42,
     225,   225,   159,     3,   226,   216,   217,   218,   219,   220,
     221,   222,   223,   152,   153,   154,   155,     6,   157,     4,
     168,   160,   168,    73,    76,   182,   130,   184,   227,   186,
     225,     3,   135,    83,   227,    85,   160,    76,   226,     4,
      90,   157,     3,     4,     5,     6,   225,   227,   132,   233,
     233,     6,   102,   103,   104,     6,     3,   214,   108,     3,
       6,     4,   219,   220,   221,     4,   227,   181,   225,     4,
     225,   225,    31,   230,   225,   232,   226,   216,   217,   218,
     219,   220,   221,   222,   223,   225,     4,   137,   138,   139,
     140,   225,   142,     6,   144,   225,   146,     6,   148,     4,
     150,   226,     6,     4,   225,   225,     6,   225,    78,   159,
     225,     5,    73,   225,   227,   226,    46,   226,   216,     3,
       6,   227,    83,     6,    85,   132,   130,   135,   134,    90,
     225,   168,   182,   168,   184,   226,   186,     4,   230,    23,
     227,   102,   103,   104,   226,   225,    30,   108,    32,    33,
       6,    35,   226,     6,    57,    57,    40,    41,     3,   227,
      52,   226,    73,   227,   214,   227,   227,   227,   227,   219,
     220,   221,   227,   227,   227,   225,   137,   138,   139,   140,
     230,   142,   232,   144,     4,   146,   227,   148,     3,   150,
     135,   227,     3,   227,    93,    93,   227,     7,   159,     9,
      10,    11,    12,    13,    14,    15,    16,    17,   227,   227,
     227,    93,   227,    23,   227,   227,    26,   227,    28,    29,
      30,   182,   133,   184,   227,   186,   227,    93,   227,   227,
     227,    91,    92,    93,    94,    95,   227,   227,    98,    99,
     227,   152,   153,   154,   155,   105,   106,   107,   227,   160,
     227,   227,   227,   214,   227,    93,   227,    93,   219,   220,
     221,    73,   135,   227,   225,   227,   226,   127,    78,   230,
     227,   232,    66,   227,   227,   227,     6,   227,   227,   227,
     164,   165,   166,   167,   225,   169,   170,   227,    53,   173,
     174,   175,    54,   227,   178,   227,   180,     6,   227,   227,
     184,   185,   227,   227,   227,   216,   217,   218,   219,   220,
     221,   222,   223,   197,   198,   226,     6,   201,   202,   203,
     227,   133,     6,   207,     6,     4,     6,   211,   212,   213,
     227,   227,   227,   133,   227,     6,   227,   227,   227,   227,
     152,   153,   154,   155,   133,     6,   227,   227,   160,   227,
     227,     6,   152,   153,   154,   155,     6,     6,   227,     6,
     160,   161,   172,   152,   153,   154,   155,     6,     6,     6,
       6,   160,     6,     6,     6,     6,     6,   227,   188,     6,
       6,     6,     6,     6,   227,     6,     6,     6,   227,     6,
     200,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     210,   211,     6,     6,   216,   217,   218,   219,   220,   221,
     222,   223,     6,     6,     6,   225,   216,   217,   218,   219,
     220,   221,   222,   223,     6,     6,     6,   216,   217,   218,
     219,   220,   221,   222,   223,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,     6,   108,   109,   110,   111,   112,   113,
       6,     6,   116,     6,   227,   119,   120,   121,   122,   123,
     124,   125,   126,    91,    92,    93,    94,    95,     6,     6,
      98,    99,   163,     6,     6,     6,     6,   105,   106,   107,
       6,     6,   173,     6,     6,   176,   177,   178,   227,   180,
       6,     4,     4,     4,     4,   226,    62,     6,   226,   127,
     226,   226,   226,   194,   195,   226,   197,   198,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     226,   226,   226,   226,   226,     6,     6,   226,   226,   226,
     226,   226,     6,   226,     6,     6,     6,     4,   226,   226,
     226,   226,   226,   226,   226,     4,   226,   226,   226,   226,
     226,   226,   226,   226,   226,     6,   226,   226,   226,   226,
     226,     6,     6,   226,   226,   226,     4,     6,   226,     6,
     226,   226,   226,   226,   226,   226,   226,    39,    47,    39,
       3,   226,    39,    39,    39,    39,     3,   226,   226,     3,
     140,   674,    65,   765,   407,    22,   631,   767,   369,   957,
     226,   538,   822,   467,   431,   962,   491,   493,   921,   797,
     226,   226,   797,    -1,   226,   226,   226,   226,   226,   226,
     226,    -1,   227,   571,    -1,   205,   227,   227,   226,   226,
     226,   226,   226,   226,   816,   234,    -1,   226,   226,   785,
     227,   227,   226,   226,   226,   226,   225,   225,   225,    -1,
     226,   226,   226,    -1,   797,    -1,    -1,    -1,    -1,    -1,
     245,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   281
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
     189,   193,   163,   173,   176,   177,   178,   180,   194,   195,
     197,   198,   197,     4,   195,    76,   197,   197,   197,   284,
     226,   226,   237,    21,   225,   264,   265,    65,   272,    67,
     266,    73,     3,   284,   284,   284,     3,    62,    62,   225,
     253,    74,     3,   284,   284,   284,     3,     3,     3,   256,
     257,    66,   277,     4,   344,   344,     3,     4,     5,     6,
      73,    74,    83,    85,    90,   102,   103,   104,   108,   137,
     138,   139,   140,   142,   144,   146,   148,   150,   159,   182,
     184,   186,   214,   219,   220,   221,   225,   230,   232,   299,
     301,   302,   303,   304,   305,   306,   307,   308,   309,   312,
     313,   314,   315,   316,   318,   319,   320,   321,   322,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   336,
     337,   338,   339,   340,   341,     3,     5,     6,    62,   171,
       3,     5,     6,    62,   171,     3,     5,     6,    62,   171,
     226,   132,   132,    39,    42,    43,    47,    48,     3,     3,
     344,     4,   132,   132,   284,    10,    24,    25,    62,   225,
     288,   344,     4,   168,     6,     3,     6,     4,     3,     4,
      55,     4,   197,     3,     3,     3,   264,   265,   299,    53,
      68,   270,    74,   135,    55,   225,   253,   284,    32,    33,
      51,     3,   250,    37,   262,    62,   216,   227,   277,   302,
      78,    78,   225,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,    73,    74,   303,   225,   225,
      88,   302,   317,     4,     4,     4,     4,     4,     6,   341,
     225,   225,   225,   225,   225,   225,   225,   122,   124,   125,
     128,   225,   225,   225,   303,   303,   262,   302,     5,     6,
     229,   322,   334,   335,   231,   321,   226,   227,    55,   157,
     158,    73,    75,   133,   152,   153,   154,   155,   156,   160,
     216,   217,   218,   219,   220,   221,   222,   223,   228,   227,
     229,   227,   229,   227,   229,   227,   231,   227,   229,   227,
     229,     3,     6,    78,   349,    46,    46,    77,   284,   263,
       4,    39,    48,     6,    77,   190,   191,     4,   226,   226,
      82,   273,   267,   268,   302,   302,    69,   271,     4,   260,
       3,   129,   131,   240,   242,   243,   249,    55,   225,   353,
       3,     3,   226,   227,   225,   300,   284,   302,   257,   225,
     225,    65,   226,   299,   225,    73,   262,   302,   302,   317,
      84,    86,    88,   302,   302,   302,   302,   302,   302,     4,
     225,   225,   225,   225,     4,     4,   302,   226,   226,   233,
      77,   301,     3,   302,   302,    75,   160,   225,    73,   132,
     303,   303,   303,   303,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   303,     3,   221,   322,     6,   334,     6,
     335,   321,     6,     5,    42,    44,    45,   225,   225,   225,
       3,   226,     6,    32,    41,     4,   168,   168,   299,    76,
     274,   227,    70,    71,   269,   302,   135,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   108,   109,   110,   111,   112,
     113,   116,   119,   120,   121,   122,   123,   124,   125,   126,
     245,   130,   225,   226,   227,   260,     3,   303,   312,   135,
       3,   299,   227,    68,    69,    79,    80,    81,   187,   342,
     343,   342,   299,   226,   262,   226,    55,    87,    84,    86,
     302,   302,   226,   226,   226,   226,   226,   226,    76,   302,
       3,   320,   302,     4,   227,   276,   226,   227,   226,     5,
       6,   344,   225,   303,   262,   299,   132,   157,   161,   233,
     233,     6,     6,     3,   351,   352,   250,   241,   243,     6,
       4,     4,   225,   281,   282,   283,   284,   289,   181,   275,
     268,     4,   225,   225,   225,   225,   225,   225,   225,   225,
     225,    73,   129,   131,   132,   246,   247,   349,   225,   250,
      31,   350,   242,   226,   226,     4,   226,   225,     6,     6,
       4,     3,    90,     6,   226,   227,   226,   226,   226,   245,
     302,   302,    84,    87,   303,   227,   227,   227,   227,     4,
      66,   226,     4,    78,   262,   299,   161,   226,   226,   303,
     303,    49,    50,    46,   216,   226,   227,   226,   226,   227,
       6,   260,   227,    56,    58,    59,    60,    61,    63,    64,
     290,     3,    55,   285,   304,   305,   306,   307,   308,   309,
     310,   311,   277,     6,   245,   244,   245,    91,    92,    93,
      94,    95,    98,    99,   105,   106,   107,   127,    91,    92,
      93,    94,    95,    98,    99,   105,   106,   107,   127,    91,
      92,    93,    94,    95,    98,    99,   105,   106,   107,   127,
      91,    92,    93,    94,    95,    98,    99,   105,   106,   107,
     127,    91,    92,    93,    94,    95,    98,    99,   105,   106,
     107,   127,    91,    92,    93,    94,    95,    98,    99,   105,
     106,   107,   127,   132,   130,   134,   247,   248,   248,   250,
     226,   225,   135,   168,   168,   299,   343,   226,    84,   302,
     226,   302,   322,   230,   323,   326,   331,   336,     4,   227,
     276,   302,   226,   225,   226,   226,   303,     6,     6,     3,
       4,     5,     6,   352,   243,    33,    35,   226,   282,    57,
      57,     3,   227,    52,   279,   226,   227,   226,   226,   227,
     227,   227,   227,   227,   227,   227,   227,   227,   227,    93,
     227,   227,   227,   227,   227,   227,   227,   227,   227,   227,
      93,   227,   227,   227,   227,   227,   227,   227,   227,   227,
     227,    93,   227,   227,   227,   227,   227,   227,   227,   227,
     227,   227,    93,   227,   227,   227,   227,   227,   227,   227,
     227,   227,   227,    93,   227,   227,   227,   227,   227,   227,
     227,   227,   227,   227,    93,   227,   321,   135,   135,   226,
     351,     4,     3,     3,   226,   227,   227,   227,   227,   276,
     226,   342,     6,   285,   283,   283,   225,   310,    53,    54,
     278,     6,   245,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   227,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   227,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   227,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   227,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   227,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   227,     6,     4,
       4,   226,   349,   349,     4,     4,     4,     4,   226,   226,
       6,    62,   250,   299,   302,   226,   226,   226,   226,   226,
     226,   226,   226,   226,   226,     6,   226,   226,   226,   226,
     226,   226,   226,   226,   226,   226,     6,   226,   226,   226,
     226,   226,   226,   226,   226,   226,   226,     6,   226,   226,
     226,   226,   226,   226,   226,   226,   226,   226,     6,   226,
     226,   226,   226,   226,   226,   226,   226,   226,   226,     6,
     226,   226,   226,   226,   226,   226,   226,   226,   226,   226,
       6,   226,   227,   227,   227,   276,   276,    39,    42,    43,
      47,    48,   302,   226,   226,   226,   226,   226,   226,   226,
       4,     4,     6,   226,   226,     6,     6,   227,   276,   227,
     276,   349,     6,    44,    45,     6,   226,     4,   226,    42,
      43,     6,   276,   349,   276,   136,   168,   349,     6,    47,
     226,   226,    39,    39,   136,   168,   349,   136,   168,   225,
      39,    39,    39,    39,     3,   225,   225,   226,     3,     3,
     349,   226,   226,   349
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
     296,   296,   296,   296,   297,   297,   297,   297,   298,   298,
     299,   299,   300,   300,   301,   301,   302,   302,   302,   302,
     302,   303,   303,   303,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   303,   304,   304,   304,   305,   305,   305,
     305,   306,   306,   306,   306,   307,   307,   308,   308,   309,
     309,   310,   310,   310,   310,   310,   310,   311,   311,   312,
     312,   312,   312,   312,   312,   312,   312,   312,   312,   312,
     312,   312,   312,   312,   312,   312,   312,   312,   312,   312,
     312,   312,   312,   312,   312,   312,   312,   312,   312,   312,
     313,   313,   314,   315,   315,   316,   316,   316,   316,   317,
     317,   318,   318,   319,   319,   319,   319,   320,   320,   320,
     320,   321,   321,   321,   321,   321,   321,   321,   321,   321,
     321,   321,   321,   321,   321,   322,   322,   322,   322,   323,
     323,   323,   324,   325,   325,   326,   326,   327,   328,   328,
     329,   330,   330,   331,   332,   332,   333,   333,   334,   335,
     336,   336,   337,   338,   338,   339,   340,   340,   341,   341,
     341,   341,   341,   341,   341,   341,   341,   341,   341,   341,
     342,   342,   343,   343,   343,   343,   343,   343,   343,   344,
     345,   345,   346,   346,   347,   347,   348,   348,   349,   349,
     350,   350,   351,   351,   352,   352,   352,   352,   352,   353,
     353,   353,   353
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
       3,     3,     3,     3,     3,     6,     3,     5,     4,     6,
       3,     3,     5,     5,     6,     4,     6,     3,     5,     4,
       5,     6,     4,     4,     5,     5,     6,     1,     3,     1,
       3,     1,     1,     1,     1,     1,     2,     2,     2,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     3,     1,     1,     2,     2,     3,
       2,     2,     3,     2,     2,     2,     2,     3,     3,     3,
       1,     1,     2,     2,     3,     2,     2,     3,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       1,     3,     2,     2,     2,     1,     2,     2,     2,     1,
       2,     0,     3,     0,     1,     0,     2,     0,     4,     0,
       4,     0,     1,     3,     1,     3,     3,     3,     3,     6,
       3,     3,     6
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
#line 2535 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 330 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2543 "parser.cpp"
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
#line 2557 "parser.cpp"
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
#line 2571 "parser.cpp"
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
#line 2585 "parser.cpp"
        break;

    case YYSYMBOL_column_type_array: /* column_type_array  */
#line 191 "parser.y"
            {
    fprintf(stderr, "destroy column_type_array\n");
    delete (((*yyvaluep).column_type_array_t));
}
#line 2594 "parser.cpp"
        break;

    case YYSYMBOL_column_type: /* column_type  */
#line 186 "parser.y"
            {
    fprintf(stderr, "destroy column_type\n");
    delete (((*yyvaluep).column_type_t));
}
#line 2603 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 323 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2614 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2622 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2631 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2640 "parser.cpp"
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
#line 2654 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 287 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2665 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2675 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2685 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2695 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2705 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2715 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2725 "parser.cpp"
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
#line 2739 "parser.cpp"
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
#line 2753 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 357 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2763 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2771 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2779 "parser.cpp"
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
#line 2793 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2802 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2810 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2818 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2826 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2834 "parser.cpp"
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
#line 2848 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2857 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2866 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2875 "parser.cpp"
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
#line 2888 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 347 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2897 "parser.cpp"
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
#line 2911 "parser.cpp"
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
#line 2925 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 363 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2935 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2944 "parser.cpp"
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
#line 2958 "parser.cpp"
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
#line 2972 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2980 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2988 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2996 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3004 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3012 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3020 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3028 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3036 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3044 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3052 "parser.cpp"
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
#line 3066 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3074 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3082 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3090 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3098 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3106 "parser.cpp"
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
#line 3119 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3127 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3135 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3143 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3151 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3159 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3167 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3175 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3183 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3191 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3199 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3207 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3215 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3223 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3231 "parser.cpp"
        break;

    case YYSYMBOL_curly_brackets_expr: /* curly_brackets_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3239 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_curly_brackets_expr: /* unclosed_curly_brackets_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3247 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 384 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3255 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 388 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3263 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3271 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3279 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3287 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3295 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3303 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3311 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 330 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3319 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 220 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3330 "parser.cpp"
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
#line 3344 "parser.cpp"
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
#line 3358 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 196 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3369 "parser.cpp"
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

#line 3477 "parser.cpp"

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
#line 3692 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 522 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3703 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 528 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3714 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 535 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3720 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 536 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3726 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 537 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3732 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 538 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3738 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 539 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3744 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 540 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3750 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 541 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3756 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 542 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3762 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 543 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3768 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 544 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3774 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 545 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3780 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 546 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3786 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 547 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3792 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 548 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3798 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 549 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3804 "parser.cpp"
    break;

  case 20: /* statement: check_statement  */
#line 550 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].check_stmt); }
#line 3810 "parser.cpp"
    break;

  case 21: /* explainable_statement: create_statement  */
#line 552 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3816 "parser.cpp"
    break;

  case 22: /* explainable_statement: drop_statement  */
#line 553 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3822 "parser.cpp"
    break;

  case 23: /* explainable_statement: copy_statement  */
#line 554 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3828 "parser.cpp"
    break;

  case 24: /* explainable_statement: show_statement  */
#line 555 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3834 "parser.cpp"
    break;

  case 25: /* explainable_statement: select_statement  */
#line 556 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3840 "parser.cpp"
    break;

  case 26: /* explainable_statement: delete_statement  */
#line 557 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3846 "parser.cpp"
    break;

  case 27: /* explainable_statement: update_statement  */
#line 558 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3852 "parser.cpp"
    break;

  case 28: /* explainable_statement: insert_statement  */
#line 559 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3858 "parser.cpp"
    break;

  case 29: /* explainable_statement: flush_statement  */
#line 560 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3864 "parser.cpp"
    break;

  case 30: /* explainable_statement: optimize_statement  */
#line 561 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3870 "parser.cpp"
    break;

  case 31: /* explainable_statement: command_statement  */
#line 562 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3876 "parser.cpp"
    break;

  case 32: /* explainable_statement: compact_statement  */
#line 563 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3882 "parser.cpp"
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
#line 3904 "parser.cpp"
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
#line 3924 "parser.cpp"
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
#line 3942 "parser.cpp"
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
#line 3975 "parser.cpp"
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
#line 3995 "parser.cpp"
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
#line 4031 "parser.cpp"
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
#line 4053 "parser.cpp"
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
#line 4074 "parser.cpp"
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
#line 4107 "parser.cpp"
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
#line 4142 "parser.cpp"
    break;

  case 43: /* table_element_array: table_element  */
#line 791 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 4151 "parser.cpp"
    break;

  case 44: /* table_element_array: table_element_array ',' table_element  */
#line 795 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 4160 "parser.cpp"
    break;

  case 45: /* column_def_array: table_column  */
#line 800 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 4169 "parser.cpp"
    break;

  case 46: /* column_def_array: column_def_array ',' table_column  */
#line 804 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 4178 "parser.cpp"
    break;

  case 47: /* table_element: table_column  */
#line 810 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 4186 "parser.cpp"
    break;

  case 48: /* table_element: table_constraint  */
#line 813 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 4194 "parser.cpp"
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
#line 4219 "parser.cpp"
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
#line 4246 "parser.cpp"
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
#line 4275 "parser.cpp"
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
#line 4305 "parser.cpp"
    break;

  case 53: /* column_type_array: column_type  */
#line 912 "parser.y"
                                {
    (yyval.column_type_array_t) = new std::vector<std::unique_ptr<infinity::ColumnType>>();
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4314 "parser.cpp"
    break;

  case 54: /* column_type_array: column_type_array ',' column_type  */
#line 916 "parser.y"
                                    {
    (yyval.column_type_array_t) = (yyvsp[-2].column_type_array_t);
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4323 "parser.cpp"
    break;

  case 55: /* column_type: BOOLEAN  */
#line 922 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4329 "parser.cpp"
    break;

  case 56: /* column_type: JSON  */
#line 923 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kJson, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4335 "parser.cpp"
    break;

  case 57: /* column_type: TINYINT  */
#line 924 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4341 "parser.cpp"
    break;

  case 58: /* column_type: SMALLINT  */
#line 925 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4347 "parser.cpp"
    break;

  case 59: /* column_type: INTEGER  */
#line 926 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4353 "parser.cpp"
    break;

  case 60: /* column_type: INT  */
#line 927 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4359 "parser.cpp"
    break;

  case 61: /* column_type: BIGINT  */
#line 928 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4365 "parser.cpp"
    break;

  case 62: /* column_type: HUGEINT  */
#line 929 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4371 "parser.cpp"
    break;

  case 63: /* column_type: FLOAT  */
#line 930 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4377 "parser.cpp"
    break;

  case 64: /* column_type: REAL  */
#line 931 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4383 "parser.cpp"
    break;

  case 65: /* column_type: DOUBLE  */
#line 932 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4389 "parser.cpp"
    break;

  case 66: /* column_type: FLOAT16  */
#line 933 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4395 "parser.cpp"
    break;

  case 67: /* column_type: BFLOAT16  */
#line 934 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4401 "parser.cpp"
    break;

  case 68: /* column_type: DATE  */
#line 935 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4407 "parser.cpp"
    break;

  case 69: /* column_type: TIME  */
#line 936 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4413 "parser.cpp"
    break;

  case 70: /* column_type: DATETIME  */
#line 937 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4419 "parser.cpp"
    break;

  case 71: /* column_type: TIMESTAMP  */
#line 938 "parser.y"
            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4425 "parser.cpp"
    break;

  case 72: /* column_type: UUID  */
#line 939 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4431 "parser.cpp"
    break;

  case 73: /* column_type: POINT  */
#line 940 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4437 "parser.cpp"
    break;

  case 74: /* column_type: LINE  */
#line 941 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4443 "parser.cpp"
    break;

  case 75: /* column_type: LSEG  */
#line 942 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4449 "parser.cpp"
    break;

  case 76: /* column_type: BOX  */
#line 943 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4455 "parser.cpp"
    break;

  case 77: /* column_type: CIRCLE  */
#line 946 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4461 "parser.cpp"
    break;

  case 78: /* column_type: VARCHAR  */
#line 948 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4467 "parser.cpp"
    break;

  case 79: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 949 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4473 "parser.cpp"
    break;

  case 80: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 950 "parser.y"
                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4479 "parser.cpp"
    break;

  case 81: /* column_type: DECIMAL  */
#line 951 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4485 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 954 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4491 "parser.cpp"
    break;

  case 83: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 955 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4497 "parser.cpp"
    break;

  case 84: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 956 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4503 "parser.cpp"
    break;

  case 85: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 957 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4509 "parser.cpp"
    break;

  case 86: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 958 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4515 "parser.cpp"
    break;

  case 87: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 959 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4521 "parser.cpp"
    break;

  case 88: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 960 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4527 "parser.cpp"
    break;

  case 89: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 961 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4533 "parser.cpp"
    break;

  case 90: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 962 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4539 "parser.cpp"
    break;

  case 91: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 963 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4545 "parser.cpp"
    break;

  case 92: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 964 "parser.y"
                                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4551 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 965 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4557 "parser.cpp"
    break;

  case 94: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 966 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4563 "parser.cpp"
    break;

  case 95: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 967 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4569 "parser.cpp"
    break;

  case 96: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 968 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4575 "parser.cpp"
    break;

  case 97: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 969 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4581 "parser.cpp"
    break;

  case 98: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 970 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4587 "parser.cpp"
    break;

  case 99: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 971 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4593 "parser.cpp"
    break;

  case 100: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 972 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4599 "parser.cpp"
    break;

  case 101: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 973 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4605 "parser.cpp"
    break;

  case 102: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 974 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4611 "parser.cpp"
    break;

  case 103: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 975 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4617 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 976 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4623 "parser.cpp"
    break;

  case 105: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 977 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4629 "parser.cpp"
    break;

  case 106: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 978 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4635 "parser.cpp"
    break;

  case 107: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 979 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4641 "parser.cpp"
    break;

  case 108: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 980 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4647 "parser.cpp"
    break;

  case 109: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 981 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4653 "parser.cpp"
    break;

  case 110: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 982 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4659 "parser.cpp"
    break;

  case 111: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 983 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4665 "parser.cpp"
    break;

  case 112: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 984 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4671 "parser.cpp"
    break;

  case 113: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 985 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4677 "parser.cpp"
    break;

  case 114: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 986 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4683 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 987 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4689 "parser.cpp"
    break;

  case 116: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 988 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4695 "parser.cpp"
    break;

  case 117: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 989 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4701 "parser.cpp"
    break;

  case 118: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 990 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4707 "parser.cpp"
    break;

  case 119: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 991 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4713 "parser.cpp"
    break;

  case 120: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 992 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4719 "parser.cpp"
    break;

  case 121: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 993 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4725 "parser.cpp"
    break;

  case 122: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 994 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4731 "parser.cpp"
    break;

  case 123: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 995 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4737 "parser.cpp"
    break;

  case 124: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 996 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4743 "parser.cpp"
    break;

  case 125: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 997 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4749 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 998 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4755 "parser.cpp"
    break;

  case 127: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 999 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4761 "parser.cpp"
    break;

  case 128: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1000 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4767 "parser.cpp"
    break;

  case 129: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1001 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4773 "parser.cpp"
    break;

  case 130: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1002 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4779 "parser.cpp"
    break;

  case 131: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1003 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4785 "parser.cpp"
    break;

  case 132: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1004 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4791 "parser.cpp"
    break;

  case 133: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1005 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4797 "parser.cpp"
    break;

  case 134: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1006 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4803 "parser.cpp"
    break;

  case 135: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1007 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4809 "parser.cpp"
    break;

  case 136: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1008 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4815 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1009 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4821 "parser.cpp"
    break;

  case 138: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4827 "parser.cpp"
    break;

  case 139: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1011 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4833 "parser.cpp"
    break;

  case 140: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1012 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4839 "parser.cpp"
    break;

  case 141: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1013 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4845 "parser.cpp"
    break;

  case 142: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1014 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4851 "parser.cpp"
    break;

  case 143: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4857 "parser.cpp"
    break;

  case 144: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4863 "parser.cpp"
    break;

  case 145: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1017 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4869 "parser.cpp"
    break;

  case 146: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1018 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4875 "parser.cpp"
    break;

  case 147: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1019 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4881 "parser.cpp"
    break;

  case 148: /* column_type: ARRAY '(' column_type ')'  */
#line 1020 "parser.y"
                            {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kArray, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_.emplace_back((yyvsp[-1].column_type_t));
}
#line 4890 "parser.cpp"
    break;

  case 149: /* column_type: TUPLE '(' column_type_array ')'  */
#line 1024 "parser.y"
                                  {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTuple, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_ = std::move(*((yyvsp[-1].column_type_array_t)));
  delete (yyvsp[-1].column_type_array_t);
}
#line 4900 "parser.cpp"
    break;

  case 150: /* column_constraints: column_constraint  */
#line 1047 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4909 "parser.cpp"
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
#line 4923 "parser.cpp"
    break;

  case 152: /* column_constraint: PRIMARY KEY  */
#line 1061 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4931 "parser.cpp"
    break;

  case 153: /* column_constraint: UNIQUE  */
#line 1064 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4939 "parser.cpp"
    break;

  case 154: /* column_constraint: NULLABLE  */
#line 1067 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4947 "parser.cpp"
    break;

  case 155: /* column_constraint: NOT NULLABLE  */
#line 1070 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4955 "parser.cpp"
    break;

  case 156: /* default_expr: DEFAULT constant_expr  */
#line 1074 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4963 "parser.cpp"
    break;

  case 157: /* default_expr: %empty  */
#line 1077 "parser.y"
                                   {
    (yyval.const_expr_t) = nullptr;
}
#line 4971 "parser.cpp"
    break;

  case 158: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1082 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4981 "parser.cpp"
    break;

  case 159: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1087 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4991 "parser.cpp"
    break;

  case 160: /* identifier_array: IDENTIFIER  */
#line 1094 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5002 "parser.cpp"
    break;

  case 161: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1100 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 5013 "parser.cpp"
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
#line 5030 "parser.cpp"
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
#line 5073 "parser.cpp"
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
#line 5093 "parser.cpp"
    break;

  case 165: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1180 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 5101 "parser.cpp"
    break;

  case 166: /* optional_identifier_array: %empty  */
#line 1183 "parser.y"
         {
    (yyval.identifier_array_t) = nullptr;
}
#line 5109 "parser.cpp"
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
#line 5132 "parser.cpp"
    break;

  case 168: /* explain_statement: EXPLAIN explainable_statement  */
#line 1207 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 5142 "parser.cpp"
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
#line 5159 "parser.cpp"
    break;

  case 170: /* update_expr_array: update_expr  */
#line 1229 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 5168 "parser.cpp"
    break;

  case 171: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1233 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 5177 "parser.cpp"
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
#line 5189 "parser.cpp"
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
#line 5205 "parser.cpp"
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
#line 5223 "parser.cpp"
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
#line 5241 "parser.cpp"
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
#line 5259 "parser.cpp"
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
#line 5282 "parser.cpp"
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
#line 5340 "parser.cpp"
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
#line 5400 "parser.cpp"
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
#line 5452 "parser.cpp"
    break;

  case 181: /* select_statement: select_without_paren  */
#line 1491 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5460 "parser.cpp"
    break;

  case 182: /* select_statement: select_with_paren  */
#line 1494 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5468 "parser.cpp"
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
#line 5482 "parser.cpp"
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
#line 5496 "parser.cpp"
    break;

  case 185: /* select_with_paren: '(' select_without_paren ')'  */
#line 1516 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5504 "parser.cpp"
    break;

  case 186: /* select_with_paren: '(' select_with_paren ')'  */
#line 1519 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5512 "parser.cpp"
    break;

  case 187: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1523 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5521 "parser.cpp"
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
#line 5552 "parser.cpp"
    break;

  case 189: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1555 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5560 "parser.cpp"
    break;

  case 190: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1558 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5568 "parser.cpp"
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
#line 5589 "parser.cpp"
    break;

  case 192: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1580 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5597 "parser.cpp"
    break;

  case 193: /* order_by_clause: %empty  */
#line 1583 "parser.y"
                              {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5605 "parser.cpp"
    break;

  case 194: /* order_by_expr_list: order_by_expr  */
#line 1587 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5614 "parser.cpp"
    break;

  case 195: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1591 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5623 "parser.cpp"
    break;

  case 196: /* order_by_expr: expr order_by_type  */
#line 1596 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5633 "parser.cpp"
    break;

  case 197: /* order_by_type: ASC  */
#line 1602 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5641 "parser.cpp"
    break;

  case 198: /* order_by_type: DESC  */
#line 1605 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5649 "parser.cpp"
    break;

  case 199: /* order_by_type: %empty  */
#line 1608 "parser.y"
         {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5657 "parser.cpp"
    break;

  case 200: /* limit_expr: LIMIT expr  */
#line 1612 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5665 "parser.cpp"
    break;

  case 201: /* limit_expr: %empty  */
#line 1616 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5671 "parser.cpp"
    break;

  case 202: /* offset_expr: OFFSET expr  */
#line 1618 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5679 "parser.cpp"
    break;

  case 203: /* offset_expr: %empty  */
#line 1622 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5685 "parser.cpp"
    break;

  case 204: /* distinct: DISTINCT  */
#line 1624 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5693 "parser.cpp"
    break;

  case 205: /* distinct: %empty  */
#line 1627 "parser.y"
         {
    (yyval.bool_value) = false;
}
#line 5701 "parser.cpp"
    break;

  case 206: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1631 "parser.y"
                                       {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5709 "parser.cpp"
    break;

  case 207: /* highlight_clause: %empty  */
#line 1634 "parser.y"
         {
    (yyval.expr_array_t) = nullptr;
}
#line 5717 "parser.cpp"
    break;

  case 208: /* from_clause: FROM table_reference  */
#line 1638 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5725 "parser.cpp"
    break;

  case 209: /* from_clause: %empty  */
#line 1641 "parser.y"
                              {
    (yyval.table_reference_t) = nullptr;
}
#line 5733 "parser.cpp"
    break;

  case 210: /* search_clause: SEARCH sub_search_array  */
#line 1645 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5743 "parser.cpp"
    break;

  case 211: /* search_clause: %empty  */
#line 1650 "parser.y"
                                {
    (yyval.expr_t) = nullptr;
}
#line 5751 "parser.cpp"
    break;

  case 212: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1654 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5759 "parser.cpp"
    break;

  case 213: /* optional_search_filter_expr: %empty  */
#line 1657 "parser.y"
                               {
    (yyval.expr_t) = nullptr;
}
#line 5767 "parser.cpp"
    break;

  case 214: /* where_clause: WHERE expr  */
#line 1661 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5775 "parser.cpp"
    break;

  case 215: /* where_clause: %empty  */
#line 1664 "parser.y"
                               {
    (yyval.expr_t) = nullptr;
}
#line 5783 "parser.cpp"
    break;

  case 216: /* having_clause: HAVING expr  */
#line 1668 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5791 "parser.cpp"
    break;

  case 217: /* having_clause: %empty  */
#line 1671 "parser.y"
                               {
    (yyval.expr_t) = nullptr;
}
#line 5799 "parser.cpp"
    break;

  case 218: /* group_by_clause: GROUP BY expr_array  */
#line 1675 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5807 "parser.cpp"
    break;

  case 219: /* group_by_clause: %empty  */
#line 1678 "parser.y"
         {
    (yyval.expr_array_t) = nullptr;
}
#line 5815 "parser.cpp"
    break;

  case 220: /* set_operator: UNION  */
#line 1682 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5823 "parser.cpp"
    break;

  case 221: /* set_operator: UNION ALL  */
#line 1685 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5831 "parser.cpp"
    break;

  case 222: /* set_operator: INTERSECT  */
#line 1688 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5839 "parser.cpp"
    break;

  case 223: /* set_operator: EXCEPT  */
#line 1691 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5847 "parser.cpp"
    break;

  case 224: /* table_reference: table_reference_unit  */
#line 1699 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5855 "parser.cpp"
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
#line 5873 "parser.cpp"
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
#line 5891 "parser.cpp"
    break;

  case 229: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1733 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5902 "parser.cpp"
    break;

  case 230: /* table_name: IDENTIFIER  */
#line 1742 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5912 "parser.cpp"
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
#line 5924 "parser.cpp"
    break;

  case 232: /* table_alias: AS IDENTIFIER  */
#line 1756 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5934 "parser.cpp"
    break;

  case 233: /* table_alias: IDENTIFIER  */
#line 1761 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5944 "parser.cpp"
    break;

  case 234: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1766 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5955 "parser.cpp"
    break;

  case 235: /* table_alias: %empty  */
#line 1772 "parser.y"
         {
    (yyval.table_alias_t) = nullptr;
}
#line 5963 "parser.cpp"
    break;

  case 236: /* with_clause: WITH with_expr_list  */
#line 1779 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5971 "parser.cpp"
    break;

  case 237: /* with_clause: %empty  */
#line 1782 "parser.y"
                                 {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5979 "parser.cpp"
    break;

  case 238: /* with_expr_list: with_expr  */
#line 1786 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5988 "parser.cpp"
    break;

  case 239: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1789 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5997 "parser.cpp"
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
#line 6009 "parser.cpp"
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
#line 6021 "parser.cpp"
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
#line 6034 "parser.cpp"
    break;

  case 243: /* join_type: INNER  */
#line 1827 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 6042 "parser.cpp"
    break;

  case 244: /* join_type: LEFT  */
#line 1830 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 6050 "parser.cpp"
    break;

  case 245: /* join_type: RIGHT  */
#line 1833 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 6058 "parser.cpp"
    break;

  case 246: /* join_type: OUTER  */
#line 1836 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6066 "parser.cpp"
    break;

  case 247: /* join_type: FULL  */
#line 1839 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6074 "parser.cpp"
    break;

  case 248: /* join_type: CROSS  */
#line 1842 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 6082 "parser.cpp"
    break;

  case 249: /* join_type: %empty  */
#line 1845 "parser.y"
                       {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 6090 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW DATABASES  */
#line 1852 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 6099 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW TABLES  */
#line 1856 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 6108 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW TASKS  */
#line 1860 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTasks;
}
#line 6117 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW CONFIGS  */
#line 1864 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 6126 "parser.cpp"
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
#line 6138 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW PROFILES  */
#line 1875 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 6147 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW BUFFER  */
#line 1879 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 6156 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW MEMINDEX  */
#line 1883 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 6165 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW QUERIES  */
#line 1887 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 6174 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1891 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 6184 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW TRANSACTIONS  */
#line 1896 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 6193 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1900 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6203 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW TRANSACTION HISTORY  */
#line 1905 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactionHistory;
}
#line 6212 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW SESSION VARIABLES  */
#line 1909 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 6221 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1913 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 6230 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1917 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6241 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1923 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6252 "parser.cpp"
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
#line 6264 "parser.cpp"
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
#line 6280 "parser.cpp"
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
#line 6296 "parser.cpp"
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
#line 6312 "parser.cpp"
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
#line 6329 "parser.cpp"
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
#line 6346 "parser.cpp"
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
#line 6364 "parser.cpp"
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
#line 6383 "parser.cpp"
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
#line 6399 "parser.cpp"
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
#line 6419 "parser.cpp"
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
#line 6441 "parser.cpp"
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
#line 6463 "parser.cpp"
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
#line 6486 "parser.cpp"
    break;

  case 280: /* show_statement: SHOW LOGS  */
#line 2098 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6495 "parser.cpp"
    break;

  case 281: /* show_statement: SHOW CATALOG  */
#line 2102 "parser.y"
               {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalog;
}
#line 6504 "parser.cpp"
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
#line 6516 "parser.cpp"
    break;

  case 283: /* show_statement: SHOW CATALOG TO file_path  */
#line 2113 "parser.y"
                            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogToFile;
      (yyval.show_stmt)->file_path_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6527 "parser.cpp"
    break;

  case 284: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2119 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6536 "parser.cpp"
    break;

  case 285: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2123 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6545 "parser.cpp"
    break;

  case 286: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2127 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6556 "parser.cpp"
    break;

  case 287: /* show_statement: SHOW MEMORY  */
#line 2133 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6565 "parser.cpp"
    break;

  case 288: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2137 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6574 "parser.cpp"
    break;

  case 289: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2141 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6583 "parser.cpp"
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
#line 6595 "parser.cpp"
    break;

  case 291: /* show_statement: SHOW SNAPSHOTS  */
#line 2152 "parser.y"
                 {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListSnapshots;
}
#line 6604 "parser.cpp"
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
#line 6616 "parser.cpp"
    break;

  case 293: /* show_statement: SHOW CACHES  */
#line 2163 "parser.y"
              {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCaches;
}
#line 6625 "parser.cpp"
    break;

  case 294: /* show_statement: SHOW CACHE  */
#line 2167 "parser.y"
             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowCache;
}
#line 6634 "parser.cpp"
    break;

  case 295: /* show_statement: SHOW COMPACT  */
#line 2171 "parser.y"
               {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCompact;
}
#line 6643 "parser.cpp"
    break;

  case 296: /* show_statement: SHOW COMPACT NOT NULLABLE  */
#line 2175 "parser.y"
                            {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCompact;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6653 "parser.cpp"
    break;

  case 297: /* show_statement: SHOW CHECKPOINT  */
#line 2180 "parser.y"
                  {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCheckpoint;
}
#line 6662 "parser.cpp"
    break;

  case 298: /* show_statement: SHOW CHECKPOINT NOT NULLABLE  */
#line 2184 "parser.y"
                               {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCheckpoint;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6672 "parser.cpp"
    break;

  case 299: /* show_statement: SHOW CHECKPOINT LONG_VALUE  */
#line 2189 "parser.y"
                             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowCheckpoint;
     (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6682 "parser.cpp"
    break;

  case 300: /* show_statement: SHOW OPTIMIZE  */
#line 2194 "parser.y"
                {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListOptimize;
}
#line 6691 "parser.cpp"
    break;

  case 301: /* show_statement: SHOW OPTIMIZE NOT NULLABLE  */
#line 2198 "parser.y"
                             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListOptimize;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6701 "parser.cpp"
    break;

  case 302: /* show_statement: SHOW IMPORT  */
#line 2203 "parser.y"
              {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListImport;
}
#line 6710 "parser.cpp"
    break;

  case 303: /* show_statement: SHOW CLEAN  */
#line 2207 "parser.y"
             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListClean;
}
#line 6719 "parser.cpp"
    break;

  case 304: /* show_statement: SHOW CLEAN NOT NULLABLE  */
#line 2211 "parser.y"
                          {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListClean;
     (yyval.show_stmt)->show_nullable_ = false;
}
#line 6729 "parser.cpp"
    break;

  case 305: /* show_statement: SHOW CLEAN LONG_VALUE  */
#line 2216 "parser.y"
                        {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowClean;
     (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6739 "parser.cpp"
    break;

  case 306: /* flush_statement: FLUSH DATA  */
#line 2225 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6748 "parser.cpp"
    break;

  case 307: /* flush_statement: FLUSH LOG  */
#line 2229 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6757 "parser.cpp"
    break;

  case 308: /* flush_statement: FLUSH BUFFER  */
#line 2233 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6766 "parser.cpp"
    break;

  case 309: /* flush_statement: FLUSH CATALOG  */
#line 2237 "parser.y"
                {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kCatalog;
}
#line 6775 "parser.cpp"
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
#line 6790 "parser.cpp"
    break;

  case 311: /* command_statement: USE IDENTIFIER  */
#line 2259 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6801 "parser.cpp"
    break;

  case 312: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2265 "parser.y"
                                       {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6811 "parser.cpp"
    break;

  case 313: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2270 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6822 "parser.cpp"
    break;

  case 314: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2276 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6833 "parser.cpp"
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
#line 6846 "parser.cpp"
    break;

  case 316: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2290 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6857 "parser.cpp"
    break;

  case 317: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2296 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6868 "parser.cpp"
    break;

  case 318: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2302 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6879 "parser.cpp"
    break;

  case 319: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2308 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6890 "parser.cpp"
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
#line 6903 "parser.cpp"
    break;

  case 321: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2322 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6914 "parser.cpp"
    break;

  case 322: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2328 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6925 "parser.cpp"
    break;

  case 323: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2334 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6936 "parser.cpp"
    break;

  case 324: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2340 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6947 "parser.cpp"
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
#line 6960 "parser.cpp"
    break;

  case 326: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2354 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6971 "parser.cpp"
    break;

  case 327: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2360 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6982 "parser.cpp"
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
#line 6995 "parser.cpp"
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
#line 7008 "parser.cpp"
    break;

  case 330: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON SYSTEM  */
#line 2382 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-2].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kSystem);
    free((yyvsp[-2].str_value));
}
#line 7019 "parser.cpp"
    break;

  case 331: /* command_statement: DROP SNAPSHOT IDENTIFIER  */
#line 2388 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kDrop, infinity::SnapshotScope::kIgnore);
    free((yyvsp[0].str_value));
}
#line 7030 "parser.cpp"
    break;

  case 332: /* command_statement: RESTORE SYSTEM SNAPSHOT IDENTIFIER  */
#line 2394 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kSystem);
    free((yyvsp[0].str_value));
}
#line 7041 "parser.cpp"
    break;

  case 333: /* command_statement: RESTORE DATABASE SNAPSHOT IDENTIFIER  */
#line 2400 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kDatabase);
    free((yyvsp[0].str_value));
}
#line 7052 "parser.cpp"
    break;

  case 334: /* command_statement: RESTORE TABLE SNAPSHOT IDENTIFIER  */
#line 2406 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kTable);
    free((yyvsp[0].str_value));
}
#line 7063 "parser.cpp"
    break;

  case 335: /* command_statement: CLEAN DATA  */
#line 2412 "parser.y"
             {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::CleanupCmd>();
}
#line 7072 "parser.cpp"
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
#line 7088 "parser.cpp"
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
#line 7105 "parser.cpp"
    break;

  case 338: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2441 "parser.y"
                                                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 7116 "parser.cpp"
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
#line 7128 "parser.cpp"
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
#line 7141 "parser.cpp"
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
#line 7155 "parser.cpp"
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
#line 7170 "parser.cpp"
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
#line 7185 "parser.cpp"
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
#line 7201 "parser.cpp"
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
#line 7217 "parser.cpp"
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
#line 7234 "parser.cpp"
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
#line 7251 "parser.cpp"
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
#line 7266 "parser.cpp"
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
#line 7282 "parser.cpp"
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
#line 7299 "parser.cpp"
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
#line 7317 "parser.cpp"
    break;

  case 352: /* admin_statement: ADMIN SHOW LOGS  */
#line 2583 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 7326 "parser.cpp"
    break;

  case 353: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2587 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 7336 "parser.cpp"
    break;

  case 354: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2592 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 7346 "parser.cpp"
    break;

  case 355: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2597 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 7357 "parser.cpp"
    break;

  case 356: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2603 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 7366 "parser.cpp"
    break;

  case 357: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2607 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 7375 "parser.cpp"
    break;

  case 358: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2611 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     ParserHelper::ToLower((yyvsp[0].str_value));
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7387 "parser.cpp"
    break;

  case 359: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2618 "parser.y"
                        {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
}
#line 7396 "parser.cpp"
    break;

  case 360: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2622 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
}
#line 7405 "parser.cpp"
    break;

  case 361: /* admin_statement: ADMIN SHOW SNAPSHOT IDENTIFIER  */
#line 2626 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSnapshot;
     ParserHelper::ToLower((yyvsp[0].str_value));
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7417 "parser.cpp"
    break;

  case 362: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2633 "parser.y"
                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7428 "parser.cpp"
    break;

  case 363: /* admin_statement: ADMIN EXPORT SNAPSHOT STRING TO STRING  */
#line 2639 "parser.y"
                                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kExportSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[-2].str_value);
     (yyval.admin_stmt)->export_path_ = (yyvsp[0].str_value);
     free((yyvsp[-2].str_value));
     free((yyvsp[0].str_value));
}
#line 7441 "parser.cpp"
    break;

  case 364: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2647 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7452 "parser.cpp"
    break;

  case 365: /* admin_statement: ADMIN SHOW NODES  */
#line 2653 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 7461 "parser.cpp"
    break;

  case 366: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2657 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7472 "parser.cpp"
    break;

  case 367: /* admin_statement: ADMIN SHOW NODE  */
#line 2663 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 7481 "parser.cpp"
    break;

  case 368: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2667 "parser.y"
                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7492 "parser.cpp"
    break;

  case 369: /* admin_statement: ADMIN SET ADMIN  */
#line 2673 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
}
#line 7502 "parser.cpp"
    break;

  case 370: /* admin_statement: ADMIN SET STANDALONE  */
#line 2678 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
}
#line 7512 "parser.cpp"
    break;

  case 371: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2683 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7524 "parser.cpp"
    break;

  case 372: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2690 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7538 "parser.cpp"
    break;

  case 373: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2699 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7552 "parser.cpp"
    break;

  case 374: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2709 "parser.y"
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
#line 7567 "parser.cpp"
    break;

  case 375: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2719 "parser.y"
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
#line 7584 "parser.cpp"
    break;

  case 376: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2731 "parser.y"
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
#line 7600 "parser.cpp"
    break;

  case 377: /* alter_statement: ALTER IDENTIFIER ON table_name with_index_param_list  */
#line 2742 "parser.y"
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
#line 7623 "parser.cpp"
    break;

  case 378: /* check_statement: CHECK SYSTEM  */
#line 2761 "parser.y"
                               {
    (yyval.check_stmt) = new infinity::CheckStatement();
    (yyval.check_stmt)->check_type_ = infinity::CheckStmtType::kSystem;
}
#line 7632 "parser.cpp"
    break;

  case 379: /* check_statement: CHECK TABLE table_name  */
#line 2765 "parser.y"
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
#line 7648 "parser.cpp"
    break;

  case 380: /* expr_array: expr_alias  */
#line 2781 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7657 "parser.cpp"
    break;

  case 381: /* expr_array: expr_array ',' expr_alias  */
#line 2785 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7666 "parser.cpp"
    break;

  case 382: /* insert_row_list: '(' expr_array ')'  */
#line 2790 "parser.y"
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
#line 7681 "parser.cpp"
    break;

  case 383: /* insert_row_list: insert_row_list ',' '(' expr_array ')'  */
#line 2800 "parser.y"
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
#line 7696 "parser.cpp"
    break;

  case 384: /* expr_alias: expr AS IDENTIFIER  */
#line 2822 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7707 "parser.cpp"
    break;

  case 385: /* expr_alias: expr  */
#line 2828 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7715 "parser.cpp"
    break;

  case 391: /* operand: '(' expr ')'  */
#line 2838 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 7723 "parser.cpp"
    break;

  case 392: /* operand: '(' select_without_paren ')'  */
#line 2841 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7734 "parser.cpp"
    break;

  case 393: /* operand: constant_expr  */
#line 2847 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7742 "parser.cpp"
    break;

  case 404: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2863 "parser.y"
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
#line 7762 "parser.cpp"
    break;

  case 405: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')'  */
#line 2879 "parser.y"
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
#line 7784 "parser.cpp"
    break;

  case 406: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' IGNORE INDEX  */
#line 2897 "parser.y"
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
#line 7805 "parser.cpp"
    break;

  case 407: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2915 "parser.y"
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
#line 7867 "parser.cpp"
    break;

  case 408: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2973 "parser.y"
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
#line 7921 "parser.cpp"
    break;

  case 409: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 3023 "parser.y"
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
#line 7979 "parser.cpp"
    break;

  case 410: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3077 "parser.y"
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
#line 8038 "parser.cpp"
    break;

  case 411: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 3135 "parser.y"
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
#line 8067 "parser.cpp"
    break;

  case 412: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 3160 "parser.y"
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
#line 8094 "parser.cpp"
    break;

  case 413: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 3183 "parser.y"
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
#line 8119 "parser.cpp"
    break;

  case 414: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3204 "parser.y"
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
#line 8144 "parser.cpp"
    break;

  case 415: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3225 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8158 "parser.cpp"
    break;

  case 416: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3234 "parser.y"
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
#line 8174 "parser.cpp"
    break;

  case 417: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 3246 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8186 "parser.cpp"
    break;

  case 418: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3253 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8200 "parser.cpp"
    break;

  case 419: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3263 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 8211 "parser.cpp"
    break;

  case 420: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 3269 "parser.y"
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
#line 8227 "parser.cpp"
    break;

  case 421: /* sub_search: match_vector_expr  */
#line 3281 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8235 "parser.cpp"
    break;

  case 422: /* sub_search: match_text_expr  */
#line 3284 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8243 "parser.cpp"
    break;

  case 423: /* sub_search: match_tensor_expr  */
#line 3287 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8251 "parser.cpp"
    break;

  case 424: /* sub_search: match_sparse_expr  */
#line 3290 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8259 "parser.cpp"
    break;

  case 425: /* sub_search: query_expr  */
#line 3293 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8267 "parser.cpp"
    break;

  case 426: /* sub_search: fusion_expr  */
#line 3296 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8275 "parser.cpp"
    break;

  case 427: /* sub_search_array: sub_search  */
#line 3300 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 8284 "parser.cpp"
    break;

  case 428: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3304 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 8293 "parser.cpp"
    break;

  case 429: /* function_expr: IDENTIFIER '(' ')'  */
#line 3309 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 8306 "parser.cpp"
    break;

  case 430: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3317 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 8319 "parser.cpp"
    break;

  case 431: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 3325 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 8333 "parser.cpp"
    break;

  case 432: /* function_expr: YEAR '(' expr ')'  */
#line 3334 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "year";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8345 "parser.cpp"
    break;

  case 433: /* function_expr: MONTH '(' expr ')'  */
#line 3341 "parser.y"
                     {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "month";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8357 "parser.cpp"
    break;

  case 434: /* function_expr: DAY '(' expr ')'  */
#line 3348 "parser.y"
                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "day";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8369 "parser.cpp"
    break;

  case 435: /* function_expr: HOUR '(' expr ')'  */
#line 3355 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "hour";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8381 "parser.cpp"
    break;

  case 436: /* function_expr: MINUTE '(' expr ')'  */
#line 3362 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "minute";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8393 "parser.cpp"
    break;

  case 437: /* function_expr: SECOND '(' expr ')'  */
#line 3369 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "second";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8405 "parser.cpp"
    break;

  case 438: /* function_expr: operand IS NOT NULLABLE  */
#line 3376 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8417 "parser.cpp"
    break;

  case 439: /* function_expr: operand IS NULLABLE  */
#line 3383 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8429 "parser.cpp"
    break;

  case 440: /* function_expr: NOT operand  */
#line 3390 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8441 "parser.cpp"
    break;

  case 441: /* function_expr: '-' operand  */
#line 3397 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8453 "parser.cpp"
    break;

  case 442: /* function_expr: '+' operand  */
#line 3404 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8465 "parser.cpp"
    break;

  case 443: /* function_expr: operand '-' operand  */
#line 3411 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8478 "parser.cpp"
    break;

  case 444: /* function_expr: operand '+' operand  */
#line 3419 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8491 "parser.cpp"
    break;

  case 445: /* function_expr: operand '*' operand  */
#line 3427 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8504 "parser.cpp"
    break;

  case 446: /* function_expr: operand '/' operand  */
#line 3435 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8517 "parser.cpp"
    break;

  case 447: /* function_expr: operand '%' operand  */
#line 3443 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8530 "parser.cpp"
    break;

  case 448: /* function_expr: operand '=' operand  */
#line 3451 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8543 "parser.cpp"
    break;

  case 449: /* function_expr: operand EQUAL operand  */
#line 3459 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8556 "parser.cpp"
    break;

  case 450: /* function_expr: operand NOT_EQ operand  */
#line 3467 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8569 "parser.cpp"
    break;

  case 451: /* function_expr: operand '<' operand  */
#line 3475 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8582 "parser.cpp"
    break;

  case 452: /* function_expr: operand '>' operand  */
#line 3483 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8595 "parser.cpp"
    break;

  case 453: /* function_expr: operand LESS_EQ operand  */
#line 3491 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8608 "parser.cpp"
    break;

  case 454: /* function_expr: operand GREATER_EQ operand  */
#line 3499 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8621 "parser.cpp"
    break;

  case 455: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3507 "parser.y"
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
#line 8656 "parser.cpp"
    break;

  case 456: /* function_expr: operand LIKE operand  */
#line 3537 "parser.y"
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
#line 8673 "parser.cpp"
    break;

  case 457: /* function_expr: operand LIKE operand ESCAPE operand  */
#line 3549 "parser.y"
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
#line 8696 "parser.cpp"
    break;

  case 458: /* function_expr: operand NOT LIKE operand  */
#line 3567 "parser.y"
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
#line 8713 "parser.cpp"
    break;

  case 459: /* function_expr: operand NOT LIKE operand ESCAPE operand  */
#line 3579 "parser.y"
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
#line 8736 "parser.cpp"
    break;

  case 460: /* conjunction_expr: expr AND expr  */
#line 3598 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8749 "parser.cpp"
    break;

  case 461: /* conjunction_expr: expr OR expr  */
#line 3606 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8762 "parser.cpp"
    break;

  case 462: /* between_expr: operand BETWEEN operand AND operand  */
#line 3615 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 8774 "parser.cpp"
    break;

  case 463: /* in_expr: operand IN '(' expr_array ')'  */
#line 3623 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8785 "parser.cpp"
    break;

  case 464: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3629 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8796 "parser.cpp"
    break;

  case 465: /* case_expr: CASE expr case_check_array END  */
#line 3636 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8807 "parser.cpp"
    break;

  case 466: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3642 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8819 "parser.cpp"
    break;

  case 467: /* case_expr: CASE case_check_array END  */
#line 3649 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8829 "parser.cpp"
    break;

  case 468: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3654 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8840 "parser.cpp"
    break;

  case 469: /* case_check_array: WHEN expr THEN expr  */
#line 3661 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 8852 "parser.cpp"
    break;

  case 470: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3668 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 8864 "parser.cpp"
    break;

  case 471: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3676 "parser.y"
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
#line 8881 "parser.cpp"
    break;

  case 472: /* cast_expr: PARSE_JSON '(' expr ')'  */
#line 3688 "parser.y"
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
#line 8899 "parser.cpp"
    break;

  case 473: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3702 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8910 "parser.cpp"
    break;

  case 474: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3708 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8921 "parser.cpp"
    break;

  case 475: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3714 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8933 "parser.cpp"
    break;

  case 476: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3721 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8945 "parser.cpp"
    break;

  case 477: /* column_expr: IDENTIFIER  */
#line 3729 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8957 "parser.cpp"
    break;

  case 478: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3736 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8969 "parser.cpp"
    break;

  case 479: /* column_expr: '*'  */
#line 3743 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8979 "parser.cpp"
    break;

  case 480: /* column_expr: column_expr '.' '*'  */
#line 3748 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8993 "parser.cpp"
    break;

  case 481: /* constant_expr: STRING  */
#line 3758 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9003 "parser.cpp"
    break;

  case 482: /* constant_expr: TRUE  */
#line 3763 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 9013 "parser.cpp"
    break;

  case 483: /* constant_expr: FALSE  */
#line 3768 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 9023 "parser.cpp"
    break;

  case 484: /* constant_expr: DOUBLE_VALUE  */
#line 3773 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9033 "parser.cpp"
    break;

  case 485: /* constant_expr: LONG_VALUE  */
#line 3778 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9043 "parser.cpp"
    break;

  case 486: /* constant_expr: JSON STRING  */
#line 3783 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kJson);
    const_expr->json_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9053 "parser.cpp"
    break;

  case 487: /* constant_expr: DATE STRING  */
#line 3788 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9063 "parser.cpp"
    break;

  case 488: /* constant_expr: TIME STRING  */
#line 3793 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9073 "parser.cpp"
    break;

  case 489: /* constant_expr: DATETIME STRING  */
#line 3798 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9083 "parser.cpp"
    break;

  case 490: /* constant_expr: TIMESTAMP STRING  */
#line 3803 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9093 "parser.cpp"
    break;

  case 491: /* constant_expr: INTERVAL interval_expr  */
#line 3808 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9101 "parser.cpp"
    break;

  case 492: /* constant_expr: interval_expr  */
#line 3811 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9109 "parser.cpp"
    break;

  case 493: /* constant_expr: common_array_expr  */
#line 3814 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9117 "parser.cpp"
    break;

  case 494: /* constant_expr: curly_brackets_expr  */
#line 3817 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9125 "parser.cpp"
    break;

  case 495: /* common_array_expr: array_expr  */
#line 3821 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9133 "parser.cpp"
    break;

  case 496: /* common_array_expr: subarray_array_expr  */
#line 3824 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9141 "parser.cpp"
    break;

  case 497: /* common_array_expr: sparse_array_expr  */
#line 3827 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9149 "parser.cpp"
    break;

  case 498: /* common_array_expr: empty_array_expr  */
#line 3830 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9157 "parser.cpp"
    break;

  case 499: /* common_sparse_array_expr: sparse_array_expr  */
#line 3834 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9165 "parser.cpp"
    break;

  case 500: /* common_sparse_array_expr: array_expr  */
#line 3837 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9173 "parser.cpp"
    break;

  case 501: /* common_sparse_array_expr: empty_array_expr  */
#line 3840 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9181 "parser.cpp"
    break;

  case 502: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3844 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9189 "parser.cpp"
    break;

  case 503: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3848 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 9199 "parser.cpp"
    break;

  case 504: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3853 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9208 "parser.cpp"
    break;

  case 505: /* sparse_array_expr: long_sparse_array_expr  */
#line 3858 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9216 "parser.cpp"
    break;

  case 506: /* sparse_array_expr: double_sparse_array_expr  */
#line 3861 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9224 "parser.cpp"
    break;

  case 507: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3865 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9232 "parser.cpp"
    break;

  case 508: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3869 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 9244 "parser.cpp"
    break;

  case 509: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3876 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9255 "parser.cpp"
    break;

  case 510: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3883 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9263 "parser.cpp"
    break;

  case 511: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3887 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 9275 "parser.cpp"
    break;

  case 512: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3894 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9286 "parser.cpp"
    break;

  case 513: /* empty_array_expr: '[' ']'  */
#line 3901 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 9294 "parser.cpp"
    break;

  case 514: /* curly_brackets_expr: unclosed_curly_brackets_expr '}'  */
#line 3905 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9302 "parser.cpp"
    break;

  case 515: /* curly_brackets_expr: '{' '}'  */
#line 3908 "parser.y"
          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
}
#line 9310 "parser.cpp"
    break;

  case 516: /* unclosed_curly_brackets_expr: '{' constant_expr  */
#line 3912 "parser.y"
                                                {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
    (yyval.const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
}
#line 9319 "parser.cpp"
    break;

  case 517: /* unclosed_curly_brackets_expr: unclosed_curly_brackets_expr ',' constant_expr  */
#line 3916 "parser.y"
                                                 {
    (yyvsp[-2].const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9328 "parser.cpp"
    break;

  case 518: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3921 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 9336 "parser.cpp"
    break;

  case 519: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3925 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 9344 "parser.cpp"
    break;

  case 520: /* array_expr: long_array_expr  */
#line 3929 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9352 "parser.cpp"
    break;

  case 521: /* array_expr: double_array_expr  */
#line 3932 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9360 "parser.cpp"
    break;

  case 522: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3936 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9368 "parser.cpp"
    break;

  case 523: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3940 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9378 "parser.cpp"
    break;

  case 524: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3945 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9387 "parser.cpp"
    break;

  case 525: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3950 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9395 "parser.cpp"
    break;

  case 526: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3954 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9405 "parser.cpp"
    break;

  case 527: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3959 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9414 "parser.cpp"
    break;

  case 528: /* interval_expr: LONG_VALUE SECONDS  */
#line 3964 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9425 "parser.cpp"
    break;

  case 529: /* interval_expr: LONG_VALUE SECOND  */
#line 3970 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9436 "parser.cpp"
    break;

  case 530: /* interval_expr: LONG_VALUE MINUTES  */
#line 3976 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9447 "parser.cpp"
    break;

  case 531: /* interval_expr: LONG_VALUE MINUTE  */
#line 3982 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9458 "parser.cpp"
    break;

  case 532: /* interval_expr: LONG_VALUE HOURS  */
#line 3988 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9469 "parser.cpp"
    break;

  case 533: /* interval_expr: LONG_VALUE HOUR  */
#line 3994 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9480 "parser.cpp"
    break;

  case 534: /* interval_expr: LONG_VALUE DAYS  */
#line 4000 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9491 "parser.cpp"
    break;

  case 535: /* interval_expr: LONG_VALUE DAY  */
#line 4006 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9502 "parser.cpp"
    break;

  case 536: /* interval_expr: LONG_VALUE MONTHS  */
#line 4012 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9513 "parser.cpp"
    break;

  case 537: /* interval_expr: LONG_VALUE MONTH  */
#line 4018 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9524 "parser.cpp"
    break;

  case 538: /* interval_expr: LONG_VALUE YEARS  */
#line 4024 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9535 "parser.cpp"
    break;

  case 539: /* interval_expr: LONG_VALUE YEAR  */
#line 4030 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9546 "parser.cpp"
    break;

  case 540: /* copy_option_list: copy_option  */
#line 4041 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 9555 "parser.cpp"
    break;

  case 541: /* copy_option_list: copy_option_list ',' copy_option  */
#line 4045 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 9564 "parser.cpp"
    break;

  case 542: /* copy_option: FORMAT JSON  */
#line 4050 "parser.y"
                          {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kFormat;
    (yyval.copy_option_t)->file_type_ = infinity::CopyFileType::kJSON;
}
#line 9574 "parser.cpp"
    break;

  case 543: /* copy_option: FORMAT IDENTIFIER  */
#line 4055 "parser.y"
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
#line 9608 "parser.cpp"
    break;

  case 544: /* copy_option: DELIMITER STRING  */
#line 4084 "parser.y"
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
#line 9623 "parser.cpp"
    break;

  case 545: /* copy_option: HEADER  */
#line 4094 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 9633 "parser.cpp"
    break;

  case 546: /* copy_option: OFFSET LONG_VALUE  */
#line 4099 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 9643 "parser.cpp"
    break;

  case 547: /* copy_option: LIMIT LONG_VALUE  */
#line 4104 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 9653 "parser.cpp"
    break;

  case 548: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 4109 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 9663 "parser.cpp"
    break;

  case 549: /* file_path: STRING  */
#line 4115 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 9671 "parser.cpp"
    break;

  case 550: /* if_exists: IF EXISTS  */
#line 4119 "parser.y"
                     { (yyval.bool_value) = true; }
#line 9677 "parser.cpp"
    break;

  case 551: /* if_exists: %empty  */
#line 4120 "parser.y"
         { (yyval.bool_value) = false; }
#line 9683 "parser.cpp"
    break;

  case 552: /* if_not_exists: IF NOT EXISTS  */
#line 4122 "parser.y"
                              { (yyval.bool_value) = true; }
#line 9689 "parser.cpp"
    break;

  case 553: /* if_not_exists: %empty  */
#line 4123 "parser.y"
         { (yyval.bool_value) = false; }
#line 9695 "parser.cpp"
    break;

  case 556: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 4138 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9708 "parser.cpp"
    break;

  case 557: /* if_not_exists_info: %empty  */
#line 4146 "parser.y"
         {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 9716 "parser.cpp"
    break;

  case 558: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 4150 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9724 "parser.cpp"
    break;

  case 559: /* with_index_param_list: %empty  */
#line 4153 "parser.y"
         {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 9732 "parser.cpp"
    break;

  case 560: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 4157 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9740 "parser.cpp"
    break;

  case 561: /* optional_table_properties_list: %empty  */
#line 4160 "parser.y"
         {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 9748 "parser.cpp"
    break;

  case 562: /* index_param_list: index_param  */
#line 4164 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 9757 "parser.cpp"
    break;

  case 563: /* index_param_list: index_param_list ',' index_param  */
#line 4168 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 9766 "parser.cpp"
    break;

  case 564: /* index_param: IDENTIFIER  */
#line 4173 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9777 "parser.cpp"
    break;

  case 565: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 4179 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9792 "parser.cpp"
    break;

  case 566: /* index_param: IDENTIFIER '=' STRING  */
#line 4189 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9807 "parser.cpp"
    break;

  case 567: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 4199 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 9820 "parser.cpp"
    break;

  case 568: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 4207 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 9833 "parser.cpp"
    break;

  case 569: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 4218 "parser.y"
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
    } else if(strcmp((yyvsp[-1].str_value), "plaid") == 0){
        index_type = infinity::IndexType::kPLAID;
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
#line 9893 "parser.cpp"
    break;

  case 570: /* index_info: '(' IDENTIFIER ')'  */
#line 4273 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 9905 "parser.cpp"
    break;

  case 571: /* index_info: '(' function_expr ')'  */
#line 4280 "parser.y"
                        {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondaryFunctional;
    (yyval.index_info_t)->function_expr_ = (yyvsp[-1].expr_t);
}
#line 9915 "parser.cpp"
    break;

  case 572: /* index_info: '(' function_expr ')' USING IDENTIFIER with_index_param_list  */
#line 4286 "parser.y"
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
#line 9957 "parser.cpp"
    break;


#line 9961 "parser.cpp"

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

#line 4325 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

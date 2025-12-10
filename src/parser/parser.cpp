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
#define YYSTYPE SQLSTYPE
#define YYLTYPE SQLLTYPE
/* Substitute the variable and function names.  */
#define yyparse sqlparse
#define yylex sqllex
#define yyerror sqlerror
#define yydebug sqldebug
#define yynerrs sqlnerrs

/* First part of user prologue.  */
#line 2 "parser.y"

#include "parser.h"
#include "lexer.h"

void yyerror(YYLTYPE *llocp, void *lexer, infinity::ParserResult *result, const char *msg);

#line 85 "parser.cpp"

#ifndef YY_CAST
#ifdef __cplusplus
#define YY_CAST(Type, Val) static_cast<Type>(Val)
#define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type>(Val)
#else
#define YY_CAST(Type, Val) ((Type)(Val))
#define YY_REINTERPRET_CAST(Type, Val) ((Type)(Val))
#endif
#endif
#ifndef YY_NULLPTR
#if defined __cplusplus
#if 201103L <= __cplusplus
#define YY_NULLPTR nullptr
#else
#define YY_NULLPTR 0
#endif
#else
#define YY_NULLPTR ((void *)0)
#endif
#endif

#include "parser.h"
/* Symbol kind.  */
enum yysymbol_kind_t {
    YYSYMBOL_YYEMPTY = -2,
    YYSYMBOL_YYEOF = 0,                                  /* "end of file"  */
    YYSYMBOL_YYerror = 1,                                /* error  */
    YYSYMBOL_YYUNDEF = 2,                                /* "invalid token"  */
    YYSYMBOL_IDENTIFIER = 3,                             /* IDENTIFIER  */
    YYSYMBOL_STRING = 4,                                 /* STRING  */
    YYSYMBOL_DOUBLE_VALUE = 5,                           /* DOUBLE_VALUE  */
    YYSYMBOL_LONG_VALUE = 6,                             /* LONG_VALUE  */
    YYSYMBOL_CREATE = 7,                                 /* CREATE  */
    YYSYMBOL_SELECT = 8,                                 /* SELECT  */
    YYSYMBOL_INSERT = 9,                                 /* INSERT  */
    YYSYMBOL_DROP = 10,                                  /* DROP  */
    YYSYMBOL_UPDATE = 11,                                /* UPDATE  */
    YYSYMBOL_DELETE = 12,                                /* DELETE  */
    YYSYMBOL_COPY = 13,                                  /* COPY  */
    YYSYMBOL_SET = 14,                                   /* SET  */
    YYSYMBOL_EXPLAIN = 15,                               /* EXPLAIN  */
    YYSYMBOL_SHOW = 16,                                  /* SHOW  */
    YYSYMBOL_ALTER = 17,                                 /* ALTER  */
    YYSYMBOL_EXECUTE = 18,                               /* EXECUTE  */
    YYSYMBOL_PREPARE = 19,                               /* PREPARE  */
    YYSYMBOL_UNION = 20,                                 /* UNION  */
    YYSYMBOL_ALL = 21,                                   /* ALL  */
    YYSYMBOL_INTERSECT = 22,                             /* INTERSECT  */
    YYSYMBOL_COMPACT = 23,                               /* COMPACT  */
    YYSYMBOL_ADD = 24,                                   /* ADD  */
    YYSYMBOL_RENAME = 25,                                /* RENAME  */
    YYSYMBOL_DUMP = 26,                                  /* DUMP  */
    YYSYMBOL_EXCEPT = 27,                                /* EXCEPT  */
    YYSYMBOL_FLUSH = 28,                                 /* FLUSH  */
    YYSYMBOL_USE = 29,                                   /* USE  */
    YYSYMBOL_OPTIMIZE = 30,                              /* OPTIMIZE  */
    YYSYMBOL_PROPERTIES = 31,                            /* PROPERTIES  */
    YYSYMBOL_DATABASE = 32,                              /* DATABASE  */
    YYSYMBOL_TABLE = 33,                                 /* TABLE  */
    YYSYMBOL_COLLECTION = 34,                            /* COLLECTION  */
    YYSYMBOL_TABLES = 35,                                /* TABLES  */
    YYSYMBOL_INTO = 36,                                  /* INTO  */
    YYSYMBOL_VALUES = 37,                                /* VALUES  */
    YYSYMBOL_VIEW = 38,                                  /* VIEW  */
    YYSYMBOL_INDEX = 39,                                 /* INDEX  */
    YYSYMBOL_TASKS = 40,                                 /* TASKS  */
    YYSYMBOL_DATABASES = 41,                             /* DATABASES  */
    YYSYMBOL_SEGMENT = 42,                               /* SEGMENT  */
    YYSYMBOL_SEGMENTS = 43,                              /* SEGMENTS  */
    YYSYMBOL_BLOCK = 44,                                 /* BLOCK  */
    YYSYMBOL_BLOCKS = 45,                                /* BLOCKS  */
    YYSYMBOL_COLUMN = 46,                                /* COLUMN  */
    YYSYMBOL_COLUMNS = 47,                               /* COLUMNS  */
    YYSYMBOL_INDEXES = 48,                               /* INDEXES  */
    YYSYMBOL_CHUNK = 49,                                 /* CHUNK  */
    YYSYMBOL_CHUNKS = 50,                                /* CHUNKS  */
    YYSYMBOL_SYSTEM = 51,                                /* SYSTEM  */
    YYSYMBOL_GROUP = 52,                                 /* GROUP  */
    YYSYMBOL_BY = 53,                                    /* BY  */
    YYSYMBOL_HAVING = 54,                                /* HAVING  */
    YYSYMBOL_AS = 55,                                    /* AS  */
    YYSYMBOL_NATURAL = 56,                               /* NATURAL  */
    YYSYMBOL_JOIN = 57,                                  /* JOIN  */
    YYSYMBOL_LEFT = 58,                                  /* LEFT  */
    YYSYMBOL_RIGHT = 59,                                 /* RIGHT  */
    YYSYMBOL_OUTER = 60,                                 /* OUTER  */
    YYSYMBOL_FULL = 61,                                  /* FULL  */
    YYSYMBOL_ON = 62,                                    /* ON  */
    YYSYMBOL_INNER = 63,                                 /* INNER  */
    YYSYMBOL_CROSS = 64,                                 /* CROSS  */
    YYSYMBOL_DISTINCT = 65,                              /* DISTINCT  */
    YYSYMBOL_WHERE = 66,                                 /* WHERE  */
    YYSYMBOL_ORDER = 67,                                 /* ORDER  */
    YYSYMBOL_LIMIT = 68,                                 /* LIMIT  */
    YYSYMBOL_OFFSET = 69,                                /* OFFSET  */
    YYSYMBOL_ASC = 70,                                   /* ASC  */
    YYSYMBOL_DESC = 71,                                  /* DESC  */
    YYSYMBOL_IF = 72,                                    /* IF  */
    YYSYMBOL_NOT = 73,                                   /* NOT  */
    YYSYMBOL_EXISTS = 74,                                /* EXISTS  */
    YYSYMBOL_IN = 75,                                    /* IN  */
    YYSYMBOL_FROM = 76,                                  /* FROM  */
    YYSYMBOL_TO = 77,                                    /* TO  */
    YYSYMBOL_WITH = 78,                                  /* WITH  */
    YYSYMBOL_DELIMITER = 79,                             /* DELIMITER  */
    YYSYMBOL_FORMAT = 80,                                /* FORMAT  */
    YYSYMBOL_HEADER = 81,                                /* HEADER  */
    YYSYMBOL_HIGHLIGHT = 82,                             /* HIGHLIGHT  */
    YYSYMBOL_CAST = 83,                                  /* CAST  */
    YYSYMBOL_END = 84,                                   /* END  */
    YYSYMBOL_CASE = 85,                                  /* CASE  */
    YYSYMBOL_ELSE = 86,                                  /* ELSE  */
    YYSYMBOL_THEN = 87,                                  /* THEN  */
    YYSYMBOL_WHEN = 88,                                  /* WHEN  */
    YYSYMBOL_BOOLEAN = 89,                               /* BOOLEAN  */
    YYSYMBOL_JSON = 90,                                  /* JSON  */
    YYSYMBOL_INTEGER = 91,                               /* INTEGER  */
    YYSYMBOL_INT = 92,                                   /* INT  */
    YYSYMBOL_TINYINT = 93,                               /* TINYINT  */
    YYSYMBOL_SMALLINT = 94,                              /* SMALLINT  */
    YYSYMBOL_BIGINT = 95,                                /* BIGINT  */
    YYSYMBOL_HUGEINT = 96,                               /* HUGEINT  */
    YYSYMBOL_VARCHAR = 97,                               /* VARCHAR  */
    YYSYMBOL_FLOAT = 98,                                 /* FLOAT  */
    YYSYMBOL_DOUBLE = 99,                                /* DOUBLE  */
    YYSYMBOL_REAL = 100,                                 /* REAL  */
    YYSYMBOL_DECIMAL = 101,                              /* DECIMAL  */
    YYSYMBOL_DATE = 102,                                 /* DATE  */
    YYSYMBOL_TIME = 103,                                 /* TIME  */
    YYSYMBOL_DATETIME = 104,                             /* DATETIME  */
    YYSYMBOL_FLOAT16 = 105,                              /* FLOAT16  */
    YYSYMBOL_BFLOAT16 = 106,                             /* BFLOAT16  */
    YYSYMBOL_UNSIGNED = 107,                             /* UNSIGNED  */
    YYSYMBOL_TIMESTAMP = 108,                            /* TIMESTAMP  */
    YYSYMBOL_UUID = 109,                                 /* UUID  */
    YYSYMBOL_POINT = 110,                                /* POINT  */
    YYSYMBOL_LINE = 111,                                 /* LINE  */
    YYSYMBOL_LSEG = 112,                                 /* LSEG  */
    YYSYMBOL_BOX = 113,                                  /* BOX  */
    YYSYMBOL_PATH = 114,                                 /* PATH  */
    YYSYMBOL_POLYGON = 115,                              /* POLYGON  */
    YYSYMBOL_CIRCLE = 116,                               /* CIRCLE  */
    YYSYMBOL_BLOB = 117,                                 /* BLOB  */
    YYSYMBOL_BITMAP = 118,                               /* BITMAP  */
    YYSYMBOL_ARRAY = 119,                                /* ARRAY  */
    YYSYMBOL_TUPLE = 120,                                /* TUPLE  */
    YYSYMBOL_EMBEDDING = 121,                            /* EMBEDDING  */
    YYSYMBOL_VECTOR = 122,                               /* VECTOR  */
    YYSYMBOL_MULTIVECTOR = 123,                          /* MULTIVECTOR  */
    YYSYMBOL_TENSOR = 124,                               /* TENSOR  */
    YYSYMBOL_SPARSE = 125,                               /* SPARSE  */
    YYSYMBOL_TENSORARRAY = 126,                          /* TENSORARRAY  */
    YYSYMBOL_BIT = 127,                                  /* BIT  */
    YYSYMBOL_TEXT = 128,                                 /* TEXT  */
    YYSYMBOL_PRIMARY = 129,                              /* PRIMARY  */
    YYSYMBOL_KEY = 130,                                  /* KEY  */
    YYSYMBOL_UNIQUE = 131,                               /* UNIQUE  */
    YYSYMBOL_NULLABLE = 132,                             /* NULLABLE  */
    YYSYMBOL_IS = 133,                                   /* IS  */
    YYSYMBOL_DEFAULT = 134,                              /* DEFAULT  */
    YYSYMBOL_COMMENT = 135,                              /* COMMENT  */
    YYSYMBOL_IGNORE = 136,                               /* IGNORE  */
    YYSYMBOL_TRUE = 137,                                 /* TRUE  */
    YYSYMBOL_FALSE = 138,                                /* FALSE  */
    YYSYMBOL_INTERVAL = 139,                             /* INTERVAL  */
    YYSYMBOL_SECOND = 140,                               /* SECOND  */
    YYSYMBOL_SECONDS = 141,                              /* SECONDS  */
    YYSYMBOL_MINUTE = 142,                               /* MINUTE  */
    YYSYMBOL_MINUTES = 143,                              /* MINUTES  */
    YYSYMBOL_HOUR = 144,                                 /* HOUR  */
    YYSYMBOL_HOURS = 145,                                /* HOURS  */
    YYSYMBOL_DAY = 146,                                  /* DAY  */
    YYSYMBOL_DAYS = 147,                                 /* DAYS  */
    YYSYMBOL_MONTH = 148,                                /* MONTH  */
    YYSYMBOL_MONTHS = 149,                               /* MONTHS  */
    YYSYMBOL_YEAR = 150,                                 /* YEAR  */
    YYSYMBOL_YEARS = 151,                                /* YEARS  */
    YYSYMBOL_EQUAL = 152,                                /* EQUAL  */
    YYSYMBOL_NOT_EQ = 153,                               /* NOT_EQ  */
    YYSYMBOL_LESS_EQ = 154,                              /* LESS_EQ  */
    YYSYMBOL_GREATER_EQ = 155,                           /* GREATER_EQ  */
    YYSYMBOL_BETWEEN = 156,                              /* BETWEEN  */
    YYSYMBOL_AND = 157,                                  /* AND  */
    YYSYMBOL_OR = 158,                                   /* OR  */
    YYSYMBOL_EXTRACT = 159,                              /* EXTRACT  */
    YYSYMBOL_LIKE = 160,                                 /* LIKE  */
    YYSYMBOL_DATA = 161,                                 /* DATA  */
    YYSYMBOL_LOG = 162,                                  /* LOG  */
    YYSYMBOL_BUFFER = 163,                               /* BUFFER  */
    YYSYMBOL_TRANSACTIONS = 164,                         /* TRANSACTIONS  */
    YYSYMBOL_TRANSACTION = 165,                          /* TRANSACTION  */
    YYSYMBOL_MEMINDEX = 166,                             /* MEMINDEX  */
    YYSYMBOL_USING = 167,                                /* USING  */
    YYSYMBOL_SESSION = 168,                              /* SESSION  */
    YYSYMBOL_GLOBAL = 169,                               /* GLOBAL  */
    YYSYMBOL_OFF = 170,                                  /* OFF  */
    YYSYMBOL_EXPORT = 171,                               /* EXPORT  */
    YYSYMBOL_CONFIGS = 172,                              /* CONFIGS  */
    YYSYMBOL_CONFIG = 173,                               /* CONFIG  */
    YYSYMBOL_PROFILES = 174,                             /* PROFILES  */
    YYSYMBOL_VARIABLES = 175,                            /* VARIABLES  */
    YYSYMBOL_VARIABLE = 176,                             /* VARIABLE  */
    YYSYMBOL_LOGS = 177,                                 /* LOGS  */
    YYSYMBOL_CATALOGS = 178,                             /* CATALOGS  */
    YYSYMBOL_CATALOG = 179,                              /* CATALOG  */
    YYSYMBOL_SEARCH = 180,                               /* SEARCH  */
    YYSYMBOL_MATCH = 181,                                /* MATCH  */
    YYSYMBOL_MAXSIM = 182,                               /* MAXSIM  */
    YYSYMBOL_QUERY = 183,                                /* QUERY  */
    YYSYMBOL_QUERIES = 184,                              /* QUERIES  */
    YYSYMBOL_FUSION = 185,                               /* FUSION  */
    YYSYMBOL_ROWLIMIT = 186,                             /* ROWLIMIT  */
    YYSYMBOL_ADMIN = 187,                                /* ADMIN  */
    YYSYMBOL_LEADER = 188,                               /* LEADER  */
    YYSYMBOL_FOLLOWER = 189,                             /* FOLLOWER  */
    YYSYMBOL_LEARNER = 190,                              /* LEARNER  */
    YYSYMBOL_CONNECT = 191,                              /* CONNECT  */
    YYSYMBOL_STANDALONE = 192,                           /* STANDALONE  */
    YYSYMBOL_NODES = 193,                                /* NODES  */
    YYSYMBOL_NODE = 194,                                 /* NODE  */
    YYSYMBOL_REMOVE = 195,                               /* REMOVE  */
    YYSYMBOL_SNAPSHOT = 196,                             /* SNAPSHOT  */
    YYSYMBOL_SNAPSHOTS = 197,                            /* SNAPSHOTS  */
    YYSYMBOL_RECOVER = 198,                              /* RECOVER  */
    YYSYMBOL_RESTORE = 199,                              /* RESTORE  */
    YYSYMBOL_CACHES = 200,                               /* CACHES  */
    YYSYMBOL_CACHE = 201,                                /* CACHE  */
    YYSYMBOL_PERSISTENCE = 202,                          /* PERSISTENCE  */
    YYSYMBOL_OBJECT = 203,                               /* OBJECT  */
    YYSYMBOL_OBJECTS = 204,                              /* OBJECTS  */
    YYSYMBOL_FILES = 205,                                /* FILES  */
    YYSYMBOL_MEMORY = 206,                               /* MEMORY  */
    YYSYMBOL_ALLOCATION = 207,                           /* ALLOCATION  */
    YYSYMBOL_HISTORY = 208,                              /* HISTORY  */
    YYSYMBOL_CHECK = 209,                                /* CHECK  */
    YYSYMBOL_CLEAN = 210,                                /* CLEAN  */
    YYSYMBOL_CHECKPOINT = 211,                           /* CHECKPOINT  */
    YYSYMBOL_IMPORT = 212,                               /* IMPORT  */
    YYSYMBOL_NUMBER = 213,                               /* NUMBER  */
    YYSYMBOL_214_ = 214,                                 /* '='  */
    YYSYMBOL_215_ = 215,                                 /* '<'  */
    YYSYMBOL_216_ = 216,                                 /* '>'  */
    YYSYMBOL_217_ = 217,                                 /* '+'  */
    YYSYMBOL_218_ = 218,                                 /* '-'  */
    YYSYMBOL_219_ = 219,                                 /* '*'  */
    YYSYMBOL_220_ = 220,                                 /* '/'  */
    YYSYMBOL_221_ = 221,                                 /* '%'  */
    YYSYMBOL_222_ = 222,                                 /* ';'  */
    YYSYMBOL_223_ = 223,                                 /* '('  */
    YYSYMBOL_224_ = 224,                                 /* ')'  */
    YYSYMBOL_225_ = 225,                                 /* ','  */
    YYSYMBOL_226_ = 226,                                 /* '.'  */
    YYSYMBOL_227_ = 227,                                 /* ']'  */
    YYSYMBOL_228_ = 228,                                 /* '['  */
    YYSYMBOL_229_ = 229,                                 /* '}'  */
    YYSYMBOL_230_ = 230,                                 /* '{'  */
    YYSYMBOL_231_ = 231,                                 /* ':'  */
    YYSYMBOL_YYACCEPT = 232,                             /* $accept  */
    YYSYMBOL_input_pattern = 233,                        /* input_pattern  */
    YYSYMBOL_statement_list = 234,                       /* statement_list  */
    YYSYMBOL_statement = 235,                            /* statement  */
    YYSYMBOL_explainable_statement = 236,                /* explainable_statement  */
    YYSYMBOL_create_statement = 237,                     /* create_statement  */
    YYSYMBOL_table_element_array = 238,                  /* table_element_array  */
    YYSYMBOL_column_def_array = 239,                     /* column_def_array  */
    YYSYMBOL_table_element = 240,                        /* table_element  */
    YYSYMBOL_table_column = 241,                         /* table_column  */
    YYSYMBOL_column_type_array = 242,                    /* column_type_array  */
    YYSYMBOL_column_type = 243,                          /* column_type  */
    YYSYMBOL_column_constraints = 244,                   /* column_constraints  */
    YYSYMBOL_column_constraint = 245,                    /* column_constraint  */
    YYSYMBOL_default_expr = 246,                         /* default_expr  */
    YYSYMBOL_table_constraint = 247,                     /* table_constraint  */
    YYSYMBOL_identifier_array = 248,                     /* identifier_array  */
    YYSYMBOL_delete_statement = 249,                     /* delete_statement  */
    YYSYMBOL_insert_statement = 250,                     /* insert_statement  */
    YYSYMBOL_optional_identifier_array = 251,            /* optional_identifier_array  */
    YYSYMBOL_explain_statement = 252,                    /* explain_statement  */
    YYSYMBOL_update_statement = 253,                     /* update_statement  */
    YYSYMBOL_update_expr_array = 254,                    /* update_expr_array  */
    YYSYMBOL_update_expr = 255,                          /* update_expr  */
    YYSYMBOL_drop_statement = 256,                       /* drop_statement  */
    YYSYMBOL_copy_statement = 257,                       /* copy_statement  */
    YYSYMBOL_select_statement = 258,                     /* select_statement  */
    YYSYMBOL_select_with_paren = 259,                    /* select_with_paren  */
    YYSYMBOL_select_without_paren = 260,                 /* select_without_paren  */
    YYSYMBOL_select_clause_with_modifier = 261,          /* select_clause_with_modifier  */
    YYSYMBOL_select_clause_without_modifier_paren = 262, /* select_clause_without_modifier_paren  */
    YYSYMBOL_select_clause_without_modifier = 263,       /* select_clause_without_modifier  */
    YYSYMBOL_order_by_clause = 264,                      /* order_by_clause  */
    YYSYMBOL_order_by_expr_list = 265,                   /* order_by_expr_list  */
    YYSYMBOL_order_by_expr = 266,                        /* order_by_expr  */
    YYSYMBOL_order_by_type = 267,                        /* order_by_type  */
    YYSYMBOL_limit_expr = 268,                           /* limit_expr  */
    YYSYMBOL_offset_expr = 269,                          /* offset_expr  */
    YYSYMBOL_distinct = 270,                             /* distinct  */
    YYSYMBOL_highlight_clause = 271,                     /* highlight_clause  */
    YYSYMBOL_from_clause = 272,                          /* from_clause  */
    YYSYMBOL_search_clause = 273,                        /* search_clause  */
    YYSYMBOL_optional_search_filter_expr = 274,          /* optional_search_filter_expr  */
    YYSYMBOL_where_clause = 275,                         /* where_clause  */
    YYSYMBOL_having_clause = 276,                        /* having_clause  */
    YYSYMBOL_group_by_clause = 277,                      /* group_by_clause  */
    YYSYMBOL_set_operator = 278,                         /* set_operator  */
    YYSYMBOL_table_reference = 279,                      /* table_reference  */
    YYSYMBOL_table_reference_unit = 280,                 /* table_reference_unit  */
    YYSYMBOL_table_reference_name = 281,                 /* table_reference_name  */
    YYSYMBOL_table_name = 282,                           /* table_name  */
    YYSYMBOL_table_alias = 283,                          /* table_alias  */
    YYSYMBOL_with_clause = 284,                          /* with_clause  */
    YYSYMBOL_with_expr_list = 285,                       /* with_expr_list  */
    YYSYMBOL_with_expr = 286,                            /* with_expr  */
    YYSYMBOL_join_clause = 287,                          /* join_clause  */
    YYSYMBOL_join_type = 288,                            /* join_type  */
    YYSYMBOL_show_statement = 289,                       /* show_statement  */
    YYSYMBOL_flush_statement = 290,                      /* flush_statement  */
    YYSYMBOL_optimize_statement = 291,                   /* optimize_statement  */
    YYSYMBOL_command_statement = 292,                    /* command_statement  */
    YYSYMBOL_compact_statement = 293,                    /* compact_statement  */
    YYSYMBOL_admin_statement = 294,                      /* admin_statement  */
    YYSYMBOL_alter_statement = 295,                      /* alter_statement  */
    YYSYMBOL_check_statement = 296,                      /* check_statement  */
    YYSYMBOL_expr_array = 297,                           /* expr_array  */
    YYSYMBOL_insert_row_list = 298,                      /* insert_row_list  */
    YYSYMBOL_expr_alias = 299,                           /* expr_alias  */
    YYSYMBOL_expr = 300,                                 /* expr  */
    YYSYMBOL_operand = 301,                              /* operand  */
    YYSYMBOL_match_tensor_expr = 302,                    /* match_tensor_expr  */
    YYSYMBOL_match_vector_expr = 303,                    /* match_vector_expr  */
    YYSYMBOL_match_sparse_expr = 304,                    /* match_sparse_expr  */
    YYSYMBOL_match_text_expr = 305,                      /* match_text_expr  */
    YYSYMBOL_query_expr = 306,                           /* query_expr  */
    YYSYMBOL_fusion_expr = 307,                          /* fusion_expr  */
    YYSYMBOL_sub_search = 308,                           /* sub_search  */
    YYSYMBOL_sub_search_array = 309,                     /* sub_search_array  */
    YYSYMBOL_function_expr = 310,                        /* function_expr  */
    YYSYMBOL_conjunction_expr = 311,                     /* conjunction_expr  */
    YYSYMBOL_between_expr = 312,                         /* between_expr  */
    YYSYMBOL_in_expr = 313,                              /* in_expr  */
    YYSYMBOL_case_expr = 314,                            /* case_expr  */
    YYSYMBOL_case_check_array = 315,                     /* case_check_array  */
    YYSYMBOL_cast_expr = 316,                            /* cast_expr  */
    YYSYMBOL_subquery_expr = 317,                        /* subquery_expr  */
    YYSYMBOL_column_expr = 318,                          /* column_expr  */
    YYSYMBOL_constant_expr = 319,                        /* constant_expr  */
    YYSYMBOL_common_array_expr = 320,                    /* common_array_expr  */
    YYSYMBOL_common_sparse_array_expr = 321,             /* common_sparse_array_expr  */
    YYSYMBOL_subarray_array_expr = 322,                  /* subarray_array_expr  */
    YYSYMBOL_unclosed_subarray_array_expr = 323,         /* unclosed_subarray_array_expr  */
    YYSYMBOL_sparse_array_expr = 324,                    /* sparse_array_expr  */
    YYSYMBOL_long_sparse_array_expr = 325,               /* long_sparse_array_expr  */
    YYSYMBOL_unclosed_long_sparse_array_expr = 326,      /* unclosed_long_sparse_array_expr  */
    YYSYMBOL_double_sparse_array_expr = 327,             /* double_sparse_array_expr  */
    YYSYMBOL_unclosed_double_sparse_array_expr = 328,    /* unclosed_double_sparse_array_expr  */
    YYSYMBOL_empty_array_expr = 329,                     /* empty_array_expr  */
    YYSYMBOL_curly_brackets_expr = 330,                  /* curly_brackets_expr  */
    YYSYMBOL_unclosed_curly_brackets_expr = 331,         /* unclosed_curly_brackets_expr  */
    YYSYMBOL_int_sparse_ele = 332,                       /* int_sparse_ele  */
    YYSYMBOL_float_sparse_ele = 333,                     /* float_sparse_ele  */
    YYSYMBOL_array_expr = 334,                           /* array_expr  */
    YYSYMBOL_long_array_expr = 335,                      /* long_array_expr  */
    YYSYMBOL_unclosed_long_array_expr = 336,             /* unclosed_long_array_expr  */
    YYSYMBOL_double_array_expr = 337,                    /* double_array_expr  */
    YYSYMBOL_unclosed_double_array_expr = 338,           /* unclosed_double_array_expr  */
    YYSYMBOL_interval_expr = 339,                        /* interval_expr  */
    YYSYMBOL_copy_option_list = 340,                     /* copy_option_list  */
    YYSYMBOL_copy_option = 341,                          /* copy_option  */
    YYSYMBOL_file_path = 342,                            /* file_path  */
    YYSYMBOL_if_exists = 343,                            /* if_exists  */
    YYSYMBOL_if_not_exists = 344,                        /* if_not_exists  */
    YYSYMBOL_semicolon = 345,                            /* semicolon  */
    YYSYMBOL_if_not_exists_info = 346,                   /* if_not_exists_info  */
    YYSYMBOL_with_index_param_list = 347,                /* with_index_param_list  */
    YYSYMBOL_optional_table_properties_list = 348,       /* optional_table_properties_list  */
    YYSYMBOL_index_param_list = 349,                     /* index_param_list  */
    YYSYMBOL_index_param = 350,                          /* index_param  */
    YYSYMBOL_index_info = 351                            /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;

/* Unqualified %code blocks.  */
#line 99 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 478 "parser.cpp"

#ifdef short
#undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
#include <limits.h> /* INFRINGES ON USER NAME SPACE */
#if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#define YY_STDINT_H
#endif
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
#undef UINT_LEAST8_MAX
#undef UINT_LEAST16_MAX
#define UINT_LEAST8_MAX 255
#define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
#if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#define YYPTRDIFF_T __PTRDIFF_TYPE__
#define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
#elif defined PTRDIFF_MAX
#ifndef ptrdiff_t
#include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#endif
#define YYPTRDIFF_T ptrdiff_t
#define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
#else
#define YYPTRDIFF_T long
#define YYPTRDIFF_MAXIMUM LONG_MAX
#endif
#endif

#ifndef YYSIZE_T
#ifdef __SIZE_TYPE__
#define YYSIZE_T __SIZE_TYPE__
#elif defined size_t
#define YYSIZE_T size_t
#elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#define YYSIZE_T size_t
#else
#define YYSIZE_T unsigned
#endif
#endif

#define YYSIZE_MAXIMUM YY_CAST(YYPTRDIFF_T, (YYPTRDIFF_MAXIMUM < YY_CAST(YYSIZE_T, -1) ? YYPTRDIFF_MAXIMUM : YY_CAST(YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST(YYPTRDIFF_T, sizeof(X))

/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
#if defined YYENABLE_NLS && YYENABLE_NLS
#if ENABLE_NLS
#include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#define YY_(Msgid) dgettext("bison-runtime", Msgid)
#endif
#endif
#ifndef YY_
#define YY_(Msgid) Msgid
#endif
#endif

#ifndef YY_ATTRIBUTE_PURE
#if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#define YY_ATTRIBUTE_PURE __attribute__((__pure__))
#else
#define YY_ATTRIBUTE_PURE
#endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
#if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#define YY_ATTRIBUTE_UNUSED __attribute__((__unused__))
#else
#define YY_ATTRIBUTE_UNUSED
#endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if !defined lint || defined __GNUC__
#define YY_USE(E) ((void)(E))
#else
#define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && !defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
#if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wuninitialized\"")
#else
#define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                                                                                                          \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wuninitialized\"") _Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
#endif
#define YY_IGNORE_MAYBE_UNINITIALIZED_END _Pragma("GCC diagnostic pop")
#else
#define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
#define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
#define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
#define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && !defined __ICC && 6 <= __GNUC__
#define YY_IGNORE_USELESS_CAST_BEGIN _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wuseless-cast\"")
#define YY_IGNORE_USELESS_CAST_END _Pragma("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
#define YY_IGNORE_USELESS_CAST_BEGIN
#define YY_IGNORE_USELESS_CAST_END
#endif

#define YY_ASSERT(E) ((void)(0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

#ifdef YYSTACK_USE_ALLOCA
#if YYSTACK_USE_ALLOCA
#ifdef __GNUC__
#define YYSTACK_ALLOC __builtin_alloca
#elif defined __BUILTIN_VA_ARG_INCR
#include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#elif defined _AIX
#define YYSTACK_ALLOC __alloca
#elif defined _MSC_VER
#include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#define alloca _alloca
#else
#define YYSTACK_ALLOC alloca
#if !defined _ALLOCA_H && !defined EXIT_SUCCESS
#include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
/* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#endif
#endif
#endif
#endif

#ifdef YYSTACK_ALLOC
/* Pacify GCC's 'empty if-body' warning.  */
#define YYSTACK_FREE(Ptr)                                                                                                                            \
    do { /* empty */                                                                                                                                 \
        ;                                                                                                                                            \
    } while (0)
#ifndef YYSTACK_ALLOC_MAXIMUM
/* The OS might guarantee only one guard page at the bottom of the stack,
   and a page size can be as small as 4096 bytes.  So we cannot safely
   invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
   to allow for a few compiler-allocated temporary stack slots.  */
#define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#endif
#else
#define YYSTACK_ALLOC YYMALLOC
#define YYSTACK_FREE YYFREE
#ifndef YYSTACK_ALLOC_MAXIMUM
#define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#endif
#if (defined __cplusplus && !defined EXIT_SUCCESS && !((defined YYMALLOC || defined malloc) && (defined YYFREE || defined free)))
#include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#endif
#ifndef YYMALLOC
#define YYMALLOC malloc
#if !defined malloc && !defined EXIT_SUCCESS
void *malloc(YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#endif
#endif
#ifndef YYFREE
#define YYFREE free
#if !defined free && !defined EXIT_SUCCESS
void free(void *); /* INFRINGES ON USER NAME SPACE */
#endif
#endif
#endif
#endif /* 1 */

#if (!defined yyoverflow &&                                                                                                                          \
     (!defined __cplusplus || (defined SQLLTYPE_IS_TRIVIAL && SQLLTYPE_IS_TRIVIAL && defined SQLSTYPE_IS_TRIVIAL && SQLSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc {
    yy_state_t yyss_alloc;
    YYSTYPE yyvs_alloc;
    YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
#define YYSTACK_GAP_MAXIMUM (YYSIZEOF(union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
#define YYSTACK_BYTES(N) ((N) * (YYSIZEOF(yy_state_t) + YYSIZEOF(YYSTYPE) + YYSIZEOF(YYLTYPE)) + 2 * YYSTACK_GAP_MAXIMUM)

#define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
#define YYSTACK_RELOCATE(Stack_alloc, Stack)                                                                                                         \
    do {                                                                                                                                             \
        YYPTRDIFF_T yynewbytes;                                                                                                                      \
        YYCOPY(&yyptr->Stack_alloc, Stack, yysize);                                                                                                  \
        Stack = &yyptr->Stack_alloc;                                                                                                                 \
        yynewbytes = yystacksize * YYSIZEOF(*Stack) + YYSTACK_GAP_MAXIMUM;                                                                           \
        yyptr += yynewbytes / YYSIZEOF(*yyptr);                                                                                                      \
    } while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
#ifndef YYCOPY
#if defined __GNUC__ && 1 < __GNUC__
#define YYCOPY(Dst, Src, Count) __builtin_memcpy(Dst, Src, YY_CAST(YYSIZE_T, (Count)) * sizeof(*(Src)))
#else
#define YYCOPY(Dst, Src, Count)                                                                                                                      \
    do {                                                                                                                                             \
        YYPTRDIFF_T yyi;                                                                                                                             \
        for (yyi = 0; yyi < (Count); yyi++)                                                                                                          \
            (Dst)[yyi] = (Src)[yyi];                                                                                                                 \
    } while (0)
#endif
#endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL 139
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST 1586

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS 232
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS 120
/* YYNRULES -- Number of rules.  */
#define YYNRULES 566
/* YYNSTATES -- Number of states.  */
#define YYNSTATES 1269

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK 468

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX) (0 <= (YYX) && (YYX) <= YYMAXUTOK ? YY_CAST(yysymbol_kind_t, yytranslate[YYX]) : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] = {
    0,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   221, 2,   2,   223, 224, 219, 217, 225, 218, 226, 220, 2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   231, 222, 215, 214, 216, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   228, 2,   227, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   230, 2,   229, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
    25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,
    53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,
    81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108,
    109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136,
    137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164,
    165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192,
    193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] = {
    0,    516,  516,  520,  526,  533,  534,  535,  536,  537,  538,  539,  540,  541,  542,  543,  544,  545,  546,  547,  548,  550,  551,  552,
    553,  554,  555,  556,  557,  558,  559,  560,  561,  568,  585,  602,  618,  647,  662,  694,  712,  730,  758,  789,  793,  798,  802,  808,
    811,  818,  838,  860,  884,  910,  914,  920,  921,  922,  923,  924,  925,  926,  927,  928,  929,  930,  931,  932,  933,  934,  935,  936,
    937,  938,  939,  940,  941,  944,  946,  947,  948,  949,  952,  953,  954,  955,  956,  957,  958,  959,  960,  961,  962,  963,  964,  965,
    966,  967,  968,  969,  970,  971,  972,  973,  974,  975,  976,  977,  978,  979,  980,  981,  982,  983,  984,  985,  986,  987,  988,  989,
    990,  991,  992,  993,  994,  995,  996,  997,  998,  999,  1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1010, 1011, 1012, 1013,
    1014, 1015, 1016, 1017, 1018, 1022, 1045, 1049, 1059, 1062, 1065, 1068, 1072, 1075, 1080, 1085, 1092, 1098, 1108, 1124, 1162, 1178, 1181, 1188,
    1205, 1214, 1227, 1231, 1236, 1249, 1262, 1277, 1292, 1307, 1330, 1383, 1438, 1489, 1492, 1495, 1504, 1514, 1517, 1521, 1526, 1553, 1556, 1561,
    1578, 1581, 1585, 1589, 1594, 1600, 1603, 1606, 1610, 1613, 1616, 1619, 1622, 1625, 1629, 1632, 1636, 1639, 1643, 1648, 1652, 1655, 1659, 1662,
    1666, 1669, 1673, 1676, 1680, 1683, 1686, 1689, 1697, 1700, 1715, 1715, 1717, 1731, 1740, 1745, 1754, 1759, 1764, 1770, 1777, 1780, 1784, 1787,
    1792, 1804, 1811, 1825, 1828, 1831, 1834, 1837, 1840, 1843, 1850, 1854, 1858, 1862, 1866, 1873, 1877, 1881, 1885, 1889, 1894, 1898, 1903, 1907,
    1911, 1915, 1921, 1927, 1934, 1945, 1956, 1967, 1979, 1991, 2004, 2018, 2029, 2044, 2061, 2078, 2096, 2100, 2104, 2111, 2117, 2121, 2125, 2131,
    2135, 2139, 2143, 2150, 2154, 2161, 2165, 2169, 2173, 2178, 2182, 2187, 2192, 2196, 2201, 2205, 2209, 2214, 2223, 2227, 2231, 2235, 2243, 2257,
    2263, 2268, 2274, 2280, 2288, 2294, 2300, 2306, 2312, 2320, 2326, 2332, 2338, 2344, 2352, 2358, 2364, 2372, 2380, 2386, 2392, 2398, 2404, 2410,
    2414, 2426, 2439, 2445, 2452, 2460, 2469, 2479, 2489, 2500, 2511, 2523, 2535, 2545, 2556, 2568, 2581, 2585, 2590, 2595, 2601, 2605, 2609, 2616,
    2620, 2624, 2631, 2637, 2645, 2651, 2655, 2661, 2665, 2671, 2676, 2681, 2688, 2697, 2707, 2717, 2729, 2740, 2759, 2763, 2779, 2783, 2788, 2798,
    2820, 2826, 2830, 2831, 2832, 2833, 2834, 2836, 2839, 2845, 2848, 2849, 2850, 2851, 2852, 2853, 2854, 2855, 2856, 2857, 2861, 2877, 2895, 2913,
    2971, 3021, 3075, 3133, 3158, 3181, 3202, 3223, 3232, 3244, 3251, 3261, 3267, 3279, 3282, 3285, 3288, 3291, 3294, 3298, 3302, 3307, 3315, 3323,
    3332, 3339, 3346, 3353, 3360, 3367, 3374, 3381, 3388, 3395, 3402, 3409, 3417, 3425, 3433, 3441, 3449, 3457, 3465, 3473, 3481, 3489, 3497, 3505,
    3535, 3543, 3552, 3560, 3569, 3577, 3583, 3590, 3596, 3603, 3608, 3615, 3622, 3630, 3643, 3649, 3655, 3662, 3670, 3677, 3684, 3689, 3699, 3704,
    3709, 3714, 3719, 3724, 3731, 3736, 3741, 3746, 3751, 3754, 3757, 3760, 3764, 3767, 3770, 3773, 3777, 3780, 3783, 3787, 3791, 3796, 3801, 3804,
    3808, 3812, 3819, 3826, 3830, 3837, 3844, 3848, 3851, 3855, 3859, 3864, 3868, 3872, 3875, 3879, 3883, 3888, 3893, 3897, 3902, 3907, 3913, 3919,
    3925, 3931, 3937, 3943, 3949, 3955, 3961, 3967, 3973, 3984, 3988, 3993, 4025, 4035, 4040, 4045, 4050, 4056, 4060, 4061, 4063, 4064, 4066, 4067,
    4079, 4087, 4091, 4094, 4098, 4101, 4105, 4109, 4114, 4120, 4130, 4140, 4148, 4159, 4212};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST(yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name(yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] = {"\"end of file\"",
                                      "error",
                                      "\"invalid token\"",
                                      "IDENTIFIER",
                                      "STRING",
                                      "DOUBLE_VALUE",
                                      "LONG_VALUE",
                                      "CREATE",
                                      "SELECT",
                                      "INSERT",
                                      "DROP",
                                      "UPDATE",
                                      "DELETE",
                                      "COPY",
                                      "SET",
                                      "EXPLAIN",
                                      "SHOW",
                                      "ALTER",
                                      "EXECUTE",
                                      "PREPARE",
                                      "UNION",
                                      "ALL",
                                      "INTERSECT",
                                      "COMPACT",
                                      "ADD",
                                      "RENAME",
                                      "DUMP",
                                      "EXCEPT",
                                      "FLUSH",
                                      "USE",
                                      "OPTIMIZE",
                                      "PROPERTIES",
                                      "DATABASE",
                                      "TABLE",
                                      "COLLECTION",
                                      "TABLES",
                                      "INTO",
                                      "VALUES",
                                      "VIEW",
                                      "INDEX",
                                      "TASKS",
                                      "DATABASES",
                                      "SEGMENT",
                                      "SEGMENTS",
                                      "BLOCK",
                                      "BLOCKS",
                                      "COLUMN",
                                      "COLUMNS",
                                      "INDEXES",
                                      "CHUNK",
                                      "CHUNKS",
                                      "SYSTEM",
                                      "GROUP",
                                      "BY",
                                      "HAVING",
                                      "AS",
                                      "NATURAL",
                                      "JOIN",
                                      "LEFT",
                                      "RIGHT",
                                      "OUTER",
                                      "FULL",
                                      "ON",
                                      "INNER",
                                      "CROSS",
                                      "DISTINCT",
                                      "WHERE",
                                      "ORDER",
                                      "LIMIT",
                                      "OFFSET",
                                      "ASC",
                                      "DESC",
                                      "IF",
                                      "NOT",
                                      "EXISTS",
                                      "IN",
                                      "FROM",
                                      "TO",
                                      "WITH",
                                      "DELIMITER",
                                      "FORMAT",
                                      "HEADER",
                                      "HIGHLIGHT",
                                      "CAST",
                                      "END",
                                      "CASE",
                                      "ELSE",
                                      "THEN",
                                      "WHEN",
                                      "BOOLEAN",
                                      "JSON",
                                      "INTEGER",
                                      "INT",
                                      "TINYINT",
                                      "SMALLINT",
                                      "BIGINT",
                                      "HUGEINT",
                                      "VARCHAR",
                                      "FLOAT",
                                      "DOUBLE",
                                      "REAL",
                                      "DECIMAL",
                                      "DATE",
                                      "TIME",
                                      "DATETIME",
                                      "FLOAT16",
                                      "BFLOAT16",
                                      "UNSIGNED",
                                      "TIMESTAMP",
                                      "UUID",
                                      "POINT",
                                      "LINE",
                                      "LSEG",
                                      "BOX",
                                      "PATH",
                                      "POLYGON",
                                      "CIRCLE",
                                      "BLOB",
                                      "BITMAP",
                                      "ARRAY",
                                      "TUPLE",
                                      "EMBEDDING",
                                      "VECTOR",
                                      "MULTIVECTOR",
                                      "TENSOR",
                                      "SPARSE",
                                      "TENSORARRAY",
                                      "BIT",
                                      "TEXT",
                                      "PRIMARY",
                                      "KEY",
                                      "UNIQUE",
                                      "NULLABLE",
                                      "IS",
                                      "DEFAULT",
                                      "COMMENT",
                                      "IGNORE",
                                      "TRUE",
                                      "FALSE",
                                      "INTERVAL",
                                      "SECOND",
                                      "SECONDS",
                                      "MINUTE",
                                      "MINUTES",
                                      "HOUR",
                                      "HOURS",
                                      "DAY",
                                      "DAYS",
                                      "MONTH",
                                      "MONTHS",
                                      "YEAR",
                                      "YEARS",
                                      "EQUAL",
                                      "NOT_EQ",
                                      "LESS_EQ",
                                      "GREATER_EQ",
                                      "BETWEEN",
                                      "AND",
                                      "OR",
                                      "EXTRACT",
                                      "LIKE",
                                      "DATA",
                                      "LOG",
                                      "BUFFER",
                                      "TRANSACTIONS",
                                      "TRANSACTION",
                                      "MEMINDEX",
                                      "USING",
                                      "SESSION",
                                      "GLOBAL",
                                      "OFF",
                                      "EXPORT",
                                      "CONFIGS",
                                      "CONFIG",
                                      "PROFILES",
                                      "VARIABLES",
                                      "VARIABLE",
                                      "LOGS",
                                      "CATALOGS",
                                      "CATALOG",
                                      "SEARCH",
                                      "MATCH",
                                      "MAXSIM",
                                      "QUERY",
                                      "QUERIES",
                                      "FUSION",
                                      "ROWLIMIT",
                                      "ADMIN",
                                      "LEADER",
                                      "FOLLOWER",
                                      "LEARNER",
                                      "CONNECT",
                                      "STANDALONE",
                                      "NODES",
                                      "NODE",
                                      "REMOVE",
                                      "SNAPSHOT",
                                      "SNAPSHOTS",
                                      "RECOVER",
                                      "RESTORE",
                                      "CACHES",
                                      "CACHE",
                                      "PERSISTENCE",
                                      "OBJECT",
                                      "OBJECTS",
                                      "FILES",
                                      "MEMORY",
                                      "ALLOCATION",
                                      "HISTORY",
                                      "CHECK",
                                      "CLEAN",
                                      "CHECKPOINT",
                                      "IMPORT",
                                      "NUMBER",
                                      "'='",
                                      "'<'",
                                      "'>'",
                                      "'+'",
                                      "'-'",
                                      "'*'",
                                      "'/'",
                                      "'%'",
                                      "';'",
                                      "'('",
                                      "')'",
                                      "','",
                                      "'.'",
                                      "']'",
                                      "'['",
                                      "'}'",
                                      "'{'",
                                      "':'",
                                      "$accept",
                                      "input_pattern",
                                      "statement_list",
                                      "statement",
                                      "explainable_statement",
                                      "create_statement",
                                      "table_element_array",
                                      "column_def_array",
                                      "table_element",
                                      "table_column",
                                      "column_type_array",
                                      "column_type",
                                      "column_constraints",
                                      "column_constraint",
                                      "default_expr",
                                      "table_constraint",
                                      "identifier_array",
                                      "delete_statement",
                                      "insert_statement",
                                      "optional_identifier_array",
                                      "explain_statement",
                                      "update_statement",
                                      "update_expr_array",
                                      "update_expr",
                                      "drop_statement",
                                      "copy_statement",
                                      "select_statement",
                                      "select_with_paren",
                                      "select_without_paren",
                                      "select_clause_with_modifier",
                                      "select_clause_without_modifier_paren",
                                      "select_clause_without_modifier",
                                      "order_by_clause",
                                      "order_by_expr_list",
                                      "order_by_expr",
                                      "order_by_type",
                                      "limit_expr",
                                      "offset_expr",
                                      "distinct",
                                      "highlight_clause",
                                      "from_clause",
                                      "search_clause",
                                      "optional_search_filter_expr",
                                      "where_clause",
                                      "having_clause",
                                      "group_by_clause",
                                      "set_operator",
                                      "table_reference",
                                      "table_reference_unit",
                                      "table_reference_name",
                                      "table_name",
                                      "table_alias",
                                      "with_clause",
                                      "with_expr_list",
                                      "with_expr",
                                      "join_clause",
                                      "join_type",
                                      "show_statement",
                                      "flush_statement",
                                      "optimize_statement",
                                      "command_statement",
                                      "compact_statement",
                                      "admin_statement",
                                      "alter_statement",
                                      "check_statement",
                                      "expr_array",
                                      "insert_row_list",
                                      "expr_alias",
                                      "expr",
                                      "operand",
                                      "match_tensor_expr",
                                      "match_vector_expr",
                                      "match_sparse_expr",
                                      "match_text_expr",
                                      "query_expr",
                                      "fusion_expr",
                                      "sub_search",
                                      "sub_search_array",
                                      "function_expr",
                                      "conjunction_expr",
                                      "between_expr",
                                      "in_expr",
                                      "case_expr",
                                      "case_check_array",
                                      "cast_expr",
                                      "subquery_expr",
                                      "column_expr",
                                      "constant_expr",
                                      "common_array_expr",
                                      "common_sparse_array_expr",
                                      "subarray_array_expr",
                                      "unclosed_subarray_array_expr",
                                      "sparse_array_expr",
                                      "long_sparse_array_expr",
                                      "unclosed_long_sparse_array_expr",
                                      "double_sparse_array_expr",
                                      "unclosed_double_sparse_array_expr",
                                      "empty_array_expr",
                                      "curly_brackets_expr",
                                      "unclosed_curly_brackets_expr",
                                      "int_sparse_ele",
                                      "float_sparse_ele",
                                      "array_expr",
                                      "long_array_expr",
                                      "unclosed_long_array_expr",
                                      "double_array_expr",
                                      "unclosed_double_array_expr",
                                      "interval_expr",
                                      "copy_option_list",
                                      "copy_option",
                                      "file_path",
                                      "if_exists",
                                      "if_not_exists",
                                      "semicolon",
                                      "if_not_exists_info",
                                      "with_index_param_list",
                                      "optional_table_properties_list",
                                      "index_param_list",
                                      "index_param",
                                      "index_info",
                                      YY_NULLPTR};

static const char *yysymbol_name(yysymbol_kind_t yysymbol) { return yytname[yysymbol]; }
#endif

#define YYPACT_NINF (-782)

#define yypact_value_is_default(Yyn) ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-554)

#define yytable_value_is_error(Yyn) ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] = {
    1042, 33,   71,   152,  123,  122,  123,  333,  245,  942,  91,   239,  254,  385,  323,  123,  343,  178,  96,   415,  46,   198,  -31,  392,
    189,  -782, -782, -782, -782, -782, -782, -782, -782, 284,  -782, -782, 450,  -782, -782, -782, -782, -782, -782, -782, -782, 363,  363,  363,
    363,  49,   442,  123,  397,  397,  397,  397,  397,  475,  256,  471,  123,  -24,  511,  514,  525,  914,  -782, -782, -782, -782, -782, -782,
    -782, 284,  -782, -782, -782, -782, -782, 322,  476,  481,  553,  123,  -782, -782, -782, -782, -782, 45,   -782, -80,  157,  -782, 560,  -782,
    -782, 39,   594,  -782, 619,  -782, -782, -782, 390,  1,    69,   222,  -782, 567,  123,  123,  628,  -782, -782, -782, -782, -782, -782, 578,
    410,  -782, 633,  458,  472,  332,  427,  487,  655,  486,  590,  493,  508,  516,  123,  -782, -782, 446,  491,  -782, 1017, -782, 689,  -782,
    -782, 27,   658,  -782, 650,  654,  722,  123,  123,  123,  728,  671,  673,  519,  678,  757,  123,  123,  123,  761,  -782, 763,  767,  706,
    785,  785,  688,  55,   81,   126,  -782, 569,  663,  672,  -782, 479,  -782, -782, -782, 803,  -782, 805,  -782, -782, 785,  -782, -782, 807,
    -782, -782, -782, -782, -782, 677,  -782, 686,  123,  75,   -782, 758,  596,  343,  785,  -782, 818,  -782, 662,  -782, 825,  -782, -782, 834,
    -782, 833,  -782, 837,  839,  -782, 840,  788,  841,  653,  843,  845,  847,  -782, -782, -782, -782, -782, 27,   -782, -782, -782, 688,  798,
    787,  782,  729,  -14,  -782, 519,  -782, 123,  444,  854,  26,   -782, -782, -782, -782, -782, 801,  -782, 656,  -38,  -782, 688,  -782, -782,
    789,  790,  643,  -782, -782, 1201, 750,  649,  660,  488,  870,  880,  881,  882,  887,  -782, -782, 889,  670,  674,  676,  679,  680,  681,
    685,  455,  687,  690,  856,  856,  -782, 582,  16,   11,   190,  -782, 7,    661,  -782, -782, -782, -782, -782, -782, -782, -782, -782, -782,
    -782, -782, -782, 675,  -782, -782, -782, -145, -782, -782, 83,   -782, 144,  -782, -782, 231,  -782, -782, 179,  -782, 215,  -782, -782, -782,
    -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, 909,  908,  -782, -782, -782, -782, -782, -782,
    -782, -782, -782, 842,  869,  871,  855,  123,  450,  -782, -782, -782, 915,  249,  -782, 916,  -782, -782, 857,  298,  -782, 932,  -782, -782,
    -782, 714,  723,  -28,  688,  688,  879,  -782, 945,  -31,  95,   895,  730,  948,  951,  -782, -782, 287,  732,  -782, 123,  688,  767,  -782,
    417,  733,  734,  280,  -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, 856,  738,  949,  874,  688,  688,  -20,  278,
    -782, -782, -782, -782, -782, 1201, -782, 688,  688,  688,  688,  688,  688,  958,  740,  747,  748,  753,  962,  975,  423,  423,  760,  -69,
    -782, 754,  -782, -782, -782, -782, -782, -782, 904,  688,  983,  688,  688,  -30,  764,  217,  856,  856,  856,  856,  856,  856,  856,  856,
    856,  856,  856,  856,  856,  856,  17,   762,  -782, 982,  -782, 985,  -782, 32,   -782, 991,  -782, 984,  -782, 957,  498,  778,  -782, 780,
    784,  1002, -782, 786,  -782, 1003, -782, 290,  1004, 844,  846,  -782, -782, -782, 688,  936,  791,  -782, 232,  417,  688,  -782, -782, 54,
    1177, 884,  795,  335,  -782, -782, -782, -31,  1016, 885,  -782, -782, -782, 1018, 688,  800,  -782, 417,  -782, 78,   78,   688,  -782, 357,
    874,  863,  811,  108,  3,    366,  -782, 688,  688,  162,  167,  171,  181,  199,  203,  960,  688,  30,   688,  1034, 817,  428,  -782, -782,
    592,  785,  -782, -782, -782, 891,  821,  856,  582,  918,  -782, 182,  182,  159,  159,  398,  182,  182,  159,  159,  423,  423,  -782, -782,
    -782, -782, -782, -782, 829,  -782, 830,  -782, -782, -782, -782, 1056, 1057, -782, 1061, 854,  1063, -782, -782, -782, 1070, -782, -782, 1065,
    1073, 853,  15,   900,  688,  -782, -782, -782, 417,  1077, -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, 860,  -782,
    -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, 864,  865,  866,  867,  868,  873,  875,  876,  384,  877,  854,  1036, 95,
    284,  888,  1088, -782, 433,  894,  1087, 1091, 1090, 1115, -782, 1116, 437,  -782, 439,  463,  -782, 899,  -782, 1177, 688,  -782, 688,  186,
    143,  -782, -782, -782, -782, -782, -782, 856,  -102, -782, 470,  -52,  902,  88,   905,  -782, 1124, -782, -782, 1052, 582,  182,  907,  474,
    -782, 856,  1127, 1129, 651,  1086, 921,  478,  -782, 484,  489,  -782, 1130, -782, -782, -31,  920,  618,  -782, 275,  -782, 324,  706,  -782,
    -782, 1134, 1177, 1177, 652,  1080, 1103, 1126, 1213, 1230, 1009, 1019, -782, -782, 352,  -782, 1012, 854,  513,  924,  1015, -782, 988,  -782,
    -782, 688,  -782, -782, -782, -782, -782, -782, 78,   -782, -782, -782, 927,  417,  193,  -782, 688,  320,  688,  762,  928,  1153, 933,  688,
    -782, 935,  937,  938,  515,  -782, -782, 949,  1155, -782, 1170, 563,  -782, 1061, -782, -782, 1063, 247,  22,   15,   1120, -782, -782, -782,
    -782, -782, -782, 1123, -782, 1178, -782, -782, -782, -782, -782, -782, -782, -782, 959,  1131, 524,  965,  544,  -782, 966,  967,  968,  974,
    978,  980,  981,  986,  987,  1089, 989,  990,  997,  998,  1010, 1011, 1013, 1014, 1020, 1021, 1097, 1022, 1023, 1024, 1025, 1029, 1030, 1031,
    1032, 1033, 1035, 1107, 1037, 1038, 1039, 1059, 1066, 1067, 1069, 1084, 1085, 1092, 1135, 1101, 1102, 1105, 1106, 1108, 1109, 1113, 1114, 1128,
    1133, 1141, 1136, 1137, 1138, 1139, 1140, 1142, 1143, 1144, 1145, 1146, 1149, 1147, -782, -782, 32,   -782, 1160, 1179, 550,  -782, 1061, 1233,
    1240, 552,  -782, -782, -782, 417,  -782, 160,  1148, 18,   1150, -782, -782, -782, 1151, 1193, 1132, 417,  -782, 78,   -782, -782, -782, -782,
    -782, -782, -782, -782, -782, -782, 1238, -782, 275,  618,  15,   15,   1093, 324,  1208, 1259, -782, 1309, -782, -782, 1177, 1326, 1348, 1349,
    1353, 1354, 1360, 1368, 1371, 1372, 1154, 1374, 1375, 1376, 1377, 1378, 1379, 1380, 1381, 1382, 1383, 1165, 1385, 1386, 1387, 1388, 1389, 1390,
    1391, 1392, 1393, 1394, 1176, 1396, 1397, 1398, 1399, 1400, 1401, 1402, 1403, 1404, 1405, 1187, 1407, 1408, 1409, 1410, 1411, 1412, 1413, 1414,
    1415, 1416, 1198, 1418, 1419, 1420, 1421, 1422, 1423, 1424, 1425, 1426, 1427, 1209, 1429, -782, 1432, 1433, -782, 556,  -782, 842,  -782, 1434,
    1435, 1436, 135,  1217, -782, 558,  1437, -782, -782, 1384, 854,  -782, 688,  688,  -782, 1218, -782, 1220, 1221, 1223, 1224, 1225, 1226, 1227,
    1228, 1229, 1448, 1231, 1232, 1234, 1235, 1236, 1237, 1239, 1241, 1242, 1243, 1451, 1244, 1245, 1246, 1247, 1248, 1249, 1250, 1251, 1252, 1253,
    1456, 1254, 1255, 1256, 1257, 1258, 1260, 1261, 1262, 1263, 1264, 1458, 1265, 1266, 1267, 1268, 1269, 1270, 1271, 1272, 1273, 1274, 1477, 1275,
    1276, 1277, 1278, 1279, 1280, 1281, 1282, 1283, 1284, 1503, 1286, -782, -782, -782, -782, 1287, 1288, 1289, 933,  -782, -782, 598,  688,  561,
    853,  417,  -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, 1291, -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, 1292,
    -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, 1293, -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, 1294, -782, -782,
    -782, -782, -782, -782, -782, -782, -782, -782, 1295, -782, -782, -782, -782, -782, -782, -782, -782, -782, -782, 1296, -782, 1507, 1517, 67,
    1298, 1299, 1518, 1519, -782, -782, -782, 417,  -782, -782, -782, -782, -782, -782, -782, 1301, 1302, 933,  842,  -782, 1522, 743,  85,   1305,
    1526, 1307, -782, 755,  1527, -782, 933,  842,  933,  5,    1528, -782, 1485, 1311, -782, 1312, 1498, 1499, -782, -782, -782, 191,  -58,  -782,
    1316, 1501, 1502, -782, 1504, 1505, 1539, -782, 1322, -782, 1323, 1324, 1544, 1546, 842,  1327, 1328, -782, 842,  -782, -782};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] = {
    237, 0,   0,   0,   0,   0,   0,   0,   237, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   237, 0,   551, 3,   5,   10,  12,
    13,  11,  6,   7,   9,   182, 181, 0,   8,   14,  15,  16,  17,  18,  19,  20,  549, 549, 549, 549, 549, 0,   0,   547, 547, 547, 547, 547, 0,
    230, 0,   0,   0,   0,   0,   0,   237, 168, 21,  26,  28,  27,  22,  23,  25,  24,  29,  30,  31,  32,  0,   295, 300, 0,   0,   251, 252, 250,
    256, 260, 0,   257, 0,   0,   253, 0,   255, 280, 281, 0,   258, 0,   291, 293, 294, 0,   287, 303, 297, 302, 0,   0,   0,   0,   306, 307, 308,
    309, 311, 310, 0,   236, 238, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   378, 335, 0,   0,   1,   237, 2,   220, 222, 223,
    0,   205, 187, 193, 0,   0,   0,   0,   0,   0,   0,   0,   166, 0,   0,   0,   0,   0,   0,   331, 0,   0,   215, 0,   0,   0,   0,   0,   0,
    167, 0,   0,   0,   267, 268, 261, 262, 263, 0,   264, 0,   254, 282, 0,   259, 292, 0,   285, 284, 288, 289, 305, 0,   299, 0,   0,   0,   337,
    0,   0,   0,   0,   359, 0,   369, 0,   370, 0,   356, 357, 0,   352, 0,   365, 367, 0,   360, 0,   0,   0,   0,   0,   0,   0,   379, 186, 185,
    4,   221, 0,   183, 184, 204, 0,   0,   201, 0,   34,  0,   35,  166, 552, 0,   0,   0,   237, 546, 173, 175, 174, 176, 0,   231, 0,   215, 170,
    0,   162, 545, 0,   0,   474, 478, 481, 482, 0,   0,   0,   0,   0,   0,   0,   0,   0,   479, 480, 0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   476, 237, 0,   0,   0,   380, 385, 386, 400, 398, 401, 399, 402, 403, 395, 390, 389, 388, 396, 397, 387, 394, 393, 490, 493,
    0,   494, 502, 0,   503, 0,   495, 491, 0,   492, 517, 0,   518, 0,   489, 315, 317, 316, 313, 314, 320, 322, 321, 318, 319, 325, 327, 326, 323,
    324, 290, 296, 301, 0,   0,   270, 269, 275, 265, 266, 283, 286, 304, 298, 555, 0,   0,   0,   0,   0,   239, 312, 362, 0,   353, 358, 0,   366,
    361, 0,   0,   368, 0,   333, 334, 332, 0,   0,   207, 0,   0,   203, 548, 0,   237, 0,   0,   0,   0,   0,   330, 160, 0,   0,   164, 0,   0,
    0,   169, 214, 0,   0,   0,   526, 525, 528, 527, 530, 529, 532, 531, 534, 533, 536, 535, 0,   0,   440, 237, 0,   0,   0,   0,   483, 484, 485,
    486, 487, 0,   488, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   442, 441, 0,   0,   523, 520, 510, 500, 505, 508, 512, 513,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   499, 0,   504, 0,   507,
    0,   511, 0,   519, 0,   522, 276, 271, 0,   377, 0,   0,   0,   336, 0,   371, 0,   354, 0,   0,   0,   0,   364, 190, 189, 0,   209, 192, 194,
    199, 200, 0,   188, 33,  37,  0,   0,   0,   0,   43,  47,  48,  237, 0,   41,  329, 328, 165, 0,   0,   163, 177, 172, 171, 0,   0,   0,   429,
    0,   237, 0,   0,   0,   0,   0,   465, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   213, 0,   392, 391, 0,   0,   381, 384,
    458, 459, 0,   0,   237, 0,   439, 449, 450, 453, 454, 0,   456, 448, 451, 452, 444, 443, 445, 446, 447, 475, 477, 501, 0,   506, 0,   509, 514,
    521, 524, 0,   0,   272, 0,   0,   0,   374, 240, 355, 0,   338, 363, 0,   0,   206, 0,   211, 0,   197, 198, 196, 202, 0,   55,  56,  59,  60,
    57,  58,  61,  62,  78,  63,  65,  64,  81,  68,  69,  70,  66,  67,  71,  72,  73,  74,  75,  76,  77,  0,   0,   0,   0,   0,   0,   0,   0,
    555, 0,   0,   557, 0,   40,  0,   0,   161, 0,   0,   0,   0,   0,   0,   541, 0,   0,   537, 0,   0,   430, 0,   470, 0,   0,   463, 0,   0,
    0,   437, 436, 435, 434, 433, 432, 0,   0,   474, 0,   0,   0,   0,   0,   419, 0,   516, 515, 0,   237, 457, 0,   0,   438, 0,   0,   0,   277,
    273, 560, 0,   558, 0,   0,   45,  339, 372, 373, 237, 208, 224, 226, 235, 227, 0,   215, 195, 39,  0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   153, 154, 157, 150, 157, 0,   0,   0,   36,  44,  566, 42,  382, 0,   543, 542, 540, 539, 544, 180, 0,   178, 431, 471, 0,   467, 0,
    466, 0,   0,   0,   0,   0,   0,   213, 0,   417, 0,   0,   0,   0,   472, 461, 460, 0,   278, 0,   0,   554, 0,   376, 375, 0,   0,   0,   0,
    0,   244, 245, 246, 247, 243, 248, 0,   233, 0,   228, 423, 421, 424, 422, 425, 426, 427, 210, 219, 0,   0,   0,   53,  0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   155, 152, 0,   151, 50,  49,  0,   159, 0,   0,   0,   0,   538, 469, 464, 468, 455, 0,   0,   0,   0,   496, 498, 497, 213, 0,
    0,   212, 420, 0,   473, 462, 279, 274, 561, 562, 564, 563, 559, 46,  0,   340, 235, 225, 0,   0,   232, 0,   0,   217, 80,  0,   148, 149, 0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   156, 0,   0,   158, 0,   38,  555, 383, 0,   0,   0,   0,   0,   418, 0,   341, 229, 241, 0,   0,   428,
    0,   0,   191, 0,   54,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   52,  51,  556, 565, 0,   0,   213, 213, 415, 179, 0,   0,   0,   218, 216, 79,
    85,  86,  83,  84,  87,  88,  89,  90,  91,  0,   82,  129, 130, 127, 128, 131, 132, 133, 134, 135, 0,   126, 96,  97,  94,  95,  98,  99,  100,
    101, 102, 0,   93,  107, 108, 105, 106, 109, 110, 111, 112, 113, 0,   104, 140, 141, 138, 139, 142, 143, 144, 145, 146, 0,   137, 118, 119, 116,
    117, 120, 121, 122, 123, 124, 0,   115, 0,   0,   0,   0,   0,   0,   0,   343, 342, 348, 242, 234, 92,  136, 103, 114, 147, 125, 213, 0,   213,
    555, 416, 349, 344, 0,   0,   0,   0,   414, 0,   0,   345, 213, 555, 213, 555, 0,   350, 346, 0,   410, 0,   0,   0,   413, 351, 347, 555, 404,
    412, 0,   0,   0,   409, 0,   0,   0,   408, 0,   406, 0,   0,   0,   0,   555, 0,   0,   411, 555, 405, 407};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] = {
    -782, -782, -782, 1417, 1488, 110,  -782, -782, 883,  -576, -782, -677, -782, 792,  796,  -782, -605, 112,  308,  1310, -782, 347,  -782, 1152,
    364,  373,  -6,   1537, -19,  1192, 1329, -68,  -782, -782, 934,  -782, -782, -782, -782, -782, -782, -782, -781, -252, -782, -782, -782, -782,
    756,  -144, 42,   617,  -782, -782, 1359, -782, -782, 399,  402,  405,  424,  435,  -782, -782, -782, -237, -782, 1100, -261, -262, -722, -716,
    -715, -714, -712, -702, 620,  -782, -782, -782, -782, -782, -782, 1156, -782, -782, 999,  -293, -291, -782, -782, -782, 783,  -782, -782, -782,
    -782, 793,  -782, -782, 1081, 1075, 794,  -782, -782, -782, -782, 1297, -537, 797,  -156, 554,  602,  -782, -782, -661, -782, 664,  770,  -782};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] = {0,   23,  24,  25,  66,   26,  531, 730, 532, 533, 834, 667, 758, 759, 906, 534, 401, 27,  28,  250,
                                         29,  30,  259, 260, 31,   32,  33,  34,  35,  147, 235, 148, 240, 520, 521, 631, 390, 525, 238, 519,
                                         627, 742, 710, 262, 1046, 951, 145, 736, 737, 738, 739, 822, 36,  120, 121, 740, 819, 37,  38,  39,
                                         40,  41,  42,  43,  44,   298, 543, 299, 300, 301, 302, 303, 304, 305, 306, 307, 829, 830, 308, 309,
                                         310, 311, 312, 431, 313,  314, 315, 316, 317, 922, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327,
                                         460, 461, 328, 329, 330,  331, 332, 333, 684, 685, 264, 159, 150, 141, 155, 502, 764, 727, 728, 537};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] = {
    408,  387,  73,   138,  463,  459,  760,  407,  426,  928,  729,  686,  430,  265,  780,  267,  268,  269,  58,   823,  601,  456,  457,  456,
    457,  824,  825,  826,  261,  827,  452,  453,  359,  705,  455,  146,  267,  268,  269,  828,  731,  393,  142,  187,  143,  582,  59,   16,
    61,   144,  370,  180,  168,  169,  518,  467,  468,  118,  334,  73,   335,  336,  466,  402,  762,  45,   46,   47,   429,  833,  835,  48,
    49,   1217, 142,  196,  143,  236,  1252, 134,  487,  144,  488,  501,  339,  364,  340,  341,  467,  468,  692,  1230, 790,  157,  109,  182,
    183,  135,  528,  365,  366,  274,  167,  123,  16,   467,  468,  51,   124,  1253, 125,  -553, 126,  275,  276,  277,  188,  337,  67,   278,
    68,   149,  274,  786,  110,  179,  58,   522,  523,  344,  583,  345,  346,  791,  275,  276,  277,  467,  468,  1122, 278,  1240, 197,  342,
    545,  1035, 678,  679,  279,  280,  281,  791,  201,  202,  791,  575,  908,  680,  681,  682,  467,  468,  426,  691,  467,  468,  386,  555,
    556,  279,  280,  281,  1241, 788,  551,  67,   229,  68,   561,  562,  563,  564,  565,  566,  52,   53,   54,   406,  347,  633,  55,   56,
    22,   243,  244,  245,  603,  465,  60,   170,  608,  791,  253,  254,  255,  194,  580,  581,  195,  394,  587,  588,  589,  590,  591,  592,
    593,  594,  595,  596,  597,  598,  599,  600,  529,  338,  530,  823,  198,  50,   784,  403,  941,  824,  825,  826,  602,  827,  735,  296,
    462,  297,  363,  458,  296,  458,  944,  828,  65,   294,  234,  343,  1,    181,  2,    3,    4,    5,    6,    7,    296,  9,    297,  632,
    683,  467,  468,  127,  11,   501,  783,  12,   111,  13,   14,   15,   454,  916,  820,  1048, 942,  625,  943,  266,  267,  268,  269,  128,
    509,  396,  585,  129,  471,  112,  130,  199,  348,  510,  695,  696,  467,  468,  629,  630,  142,  676,  143,  704,  489,  707,  490,  144,
    687,  -554, -554, 471,  69,   467,  468,  467,  468,  717,  620,  16,   467,  468,  117,  1249, 467,  468,  821,  621,  184,  185,  -554, -554,
    474,  475,  467,  468,  1200, 1201, -554, 467,  468,  549,  119,  719,  57,   586,  467,  468,  122,  270,  271,  70,   467,  468,  1250, 136,
    467,  468,  558,  272,  559,  273,  560,  522,  1118, 491,  274,  492,  71,   69,   -554, -554, 481,  482,  483,  484,  485,  72,   275,  276,
    277,  1031, 697,  527,  278,  467,  468,  698,  139,  553,  1037, 699,  -554, 479,  480,  481,  482,  483,  484,  485,  495,  700,  496,  74,
    554,  506,  75,   140,  70,   76,   464,  465,  17,   279,  280,  281,  282,  715,  283,  701,  284,  754,  285,  702,  286,  71,   287,  781,
    77,   782,  1223, 149,  1225, 1127, 72,   288,  497,  785,  498,  78,   19,   156,  544,  131,  132,  1237, 693,  1239, 694,  471,  560,  21,
    493,  754,  146,  799,  494,  289,  501,  290,  74,   291,  133,  75,   22,   158,  76,   553,  472,  473,  474,  475,  397,  398,  164,  796,
    477,  755,  165,  756,  757,  166,  904,  513,  514,  77,   831,  266,  267,  268,  269,  399,  920,  292,  293,  294,  78,   62,   63,   295,
    550,  289,  64,   290,  296,  291,  297,  540,  541,  755,  171,  756,  757,  172,  352,  209,  210,  353,  354,  917,  211,  919,  355,  356,
    173,  672,  929,  471,  913,  689,  478,  479,  480,  481,  482,  483,  484,  485,  612,  613,  918,  175,  113,  114,  115,  176,  472,  473,
    474,  475,  177,  721,  178,  1226, 477,  670,  671,  270,  271,  186,  116,  718,  936,  937,  938,  939,  1238, 272,  1242, 273,  467,  468,
    429,  446,  274,  447,  448,  688,  465,  449,  1251, 266,  267,  268,  269,  212,  275,  276,  277,  191,  192,  193,  278,  713,  714,  213,
    189,  1265, 214,  215,  216,  1268, 217,  160,  161,  162,  163,  1023, 478,  479,  480,  481,  482,  483,  484,  485,  218,  219,  190,  220,
    221,  279,  280,  281,  282,  200,  283,  203,  284,  204,  285,  205,  286,  1202, 287,  206,  1203, 1204, 483,  484,  485,  1205, 1206, 288,
    151,  152,  153,  154,  711,  712,  207,  270,  271,  768,  465,  223,  16,   775,  776,  777,  776,  272,  225,  273,  208,  289,  230,  290,
    274,  291,  812,  -249, 813,  814,  815,  816,  224,  817,  818,  222,  275,  276,  277,  778,  465,  226,  278,  266,  267,  268,  269,  787,
    486,  795,  798,  465,  800,  801,  804,  805,  227,  292,  293,  294,  806,  541,  233,  295,  228,  807,  808,  231,  296,  239,  297,  279,
    280,  281,  282,  237,  283,  242,  284,  241,  285,  810,  286,  246,  287,  247,  469,  248,  470,  909,  541,  933,  465,  288,  249,  836,
    837,  838,  839,  840,  952,  953,  841,  842,  251,  266,  267,  268,  269,  843,  844,  845,  252,  270,  271,  289,  256,  290,  257,  291,
    955,  956,  258,  272,  261,  273,  1026, 541,  1030, 465,  274,  846,  1117, 805,  1124, 776,  1129, 1208, 541,  1228, 1229, 263,  275,  276,
    277,  349,  471,  350,  278,  1234, 1235, 292,  293,  294,  1040, 1041, 351,  295,  357,  1128, 358,  361,  296,  360,  297,  472,  473,  474,
    475,  476,  362,  368,  367,  477,  371,  424,  425,  279,  280,  281,  282,  372,  283,  373,  284,  272,  285,  273,  286,  374,  287,  375,
    274,  376,  377,  379,  378,  380,  382,  288,  383,  381,  384,  388,  275,  276,  277,  389,  391,  400,  278,  266,  267,  268,  269,  404,
    392,  1207, 411,  409,  410,  289,  405,  290,  427,  291,  432,  478,  479,  480,  481,  482,  483,  484,  485,  428,  433,  434,  435,  279,
    280,  281,  282,  436,  283,  439,  284,  437,  285,  440,  286,  441,  287,  486,  442,  443,  444,  292,  293,  294,  445,  288,  450,  295,
    499,  451,  500,  503,  296,  504,  297,  508,  501,  1,    511,  2,    3,    4,    5,    6,    7,    424,  9,    289,  505,  290,  512,  291,
    515,  11,   516,  272,  12,   273,  13,   14,   15,   79,   274,  517,  524,  526,  535,  538,  16,   536,  539,  542,  547,  548,  275,  276,
    277,  552,  567,  568,  278,  80,   572,  292,  293,  294,  569,  570,  81,   295,  82,   83,   571,  84,   296,  573,  297,  577,  85,   86,
    574,  576,  579,  584,  604,  610,  296,  606,  16,   279,  280,  281,  282,  609,  283,  611,  284,  614,  285,  615,  286,  617,  287,  616,
    622,  619,  618,  623,  626,  624,  668,  288,  628,  -550, 669,  673,  674,  675,  553,  583,  1,    677,  2,    3,    4,    5,    6,    7,
    8,    9,    10,   690,  703,  289,  708,  290,  11,   291,  709,  12,   716,  13,   14,   15,   467,  1,    720,  2,    3,    4,    5,    6,
    7,    8,    9,    10,   722,  723,  724,  725,  726,  11,   528,  763,  12,   733,  13,   14,   15,   292,  293,  294,  732,  734,  465,  295,
    741,  744,  471,  745,  296,  17,   297,  746,  747,  748,  749,  750,  767,  770,  772,  16,   751,  771,  752,  753,  761,  472,  473,  474,
    475,  87,   88,   89,   90,   477,  91,   92,   766,  19,   93,   94,   95,   769,  773,  96,   16,   97,   774,  779,  21,   98,   99,   789,
    793,  792,  794,  797,  802,  714,  713,  803,  809,  22,   100,  101,  832,  902,  102,  103,  104,  811,  904,  910,  105,  903,  911,  915,
    106,  107,  108,  912,  921,  926,  927,  930,  931,  934,  932,  478,  479,  480,  481,  482,  483,  484,  485,  847,  848,  849,  850,  851,
    935,  946,  852,  853,  947,  948,  966,  950,  949,  854,  855,  856,  17,   954,  977,  957,  958,  959,  858,  859,  860,  861,  862,  960,
    988,  863,  864,  961,  18,   962,  963,  857,  865,  866,  867,  964,  965,  17,   967,  968,  19,   869,  870,  871,  872,  873,  969,  970,
    874,  875,  20,   21,   999,  18,   868,  876,  877,  878,  1010, 971,  972,  1028, 973,  974,  22,   19,   1021, 1029, 1038, 975,  976,  978,
    979,  980,  981,  20,   21,   879,  982,  983,  984,  985,  986,  791,  987,  1044, 989,  990,  991,  22,   634,  635,  636,  637,  638,  639,
    640,  641,  642,  643,  644,  645,  646,  647,  648,  649,  650,  651,  992,  652,  653,  654,  655,  656,  657,  993,  994,  658,  995,  1024,
    659,  660,  661,  662,  663,  664,  665,  666,  880,  881,  882,  883,  884,  996,  997,  885,  886,  1045, 1025, 1047, 1042, 998,  887,  888,
    889,  891,  892,  893,  894,  895,  1000, 1001, 896,  897,  1002, 1003, 1049, 1004, 1005, 898,  899,  900,  1006, 1007, 890,  412,  413,  414,
    415,  416,  417,  418,  419,  420,  421,  422,  423,  1008, 1050, 1051, 1036, 901,  1009, 1052, 1053, 1011, 1012, 1013, 1014, 1015, 1054, 1016,
    1017, 1018, 1019, 1020, 1022, 1032, 1055, 1033, 1034, 1056, 1057, 1058, 1059, 1060, 1061, 1062, 1063, 1064, 1065, 1066, 1067, 1068, 1069, 1070,
    1071, 1072, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1088, 1089, 1090, 1091, 1092, 1093, 1094,
    1095, 1096, 1097, 1098, 1099, 1100, 1101, 1102, 1103, 1104, 1105, 1106, 1107, 1108, 1109, 1110, 1111, 1112, 1113, 1114, 1115, 1116, 1119, 1120,
    1121, 1123, 1130, 1125, 1131, 1132, 1126, 1133, 1134, 1135, 1136, 1137, 1138, 1139, 1140, 1141, 1142, 1151, 1143, 1144, 1145, 1146, 1162, 1147,
    1173, 1148, 1149, 1150, 1152, 1153, 1154, 1155, 1156, 1157, 1158, 1159, 1160, 1161, 1163, 1164, 1165, 1166, 1167, 1184, 1168, 1169, 1170, 1171,
    1172, 1174, 1175, 1176, 1177, 1178, 1179, 1180, 1181, 1182, 1183, 1185, 1186, 1187, 1188, 1189, 1190, 1191, 1192, 1193, 1194, 1195, 1196, 1215,
    1197, 1198, 1199, 1209, 1210, 1211, 1212, 1213, 1214, 1216, 1218, 1219, 1220, 1221, 1222, 1224, 1227, 1231, 1232, 1233, 1244, 1236, 1243, 1245,
    1246, 1247, 1248, 1254, 1255, 1256, 1259, 1257, 1258, 1260, 1261, 1263, 1262, 1264, 905,  1266, 1267, 174,  765,  395,  907,  232,  546,  137,
    507,  1039, 743,  385,  369,  578,  607,  945,  706,  1043, 605,  923,  0,    914,  1027, 940,  0,    0,    438,  0,    0,    924,  925,  0,
    0,    0,    557};

static const yytype_int16 yycheck[] = {
    261, 238,  8,    22,   297,  296,  667, 259,  270, 790, 615, 548,  273, 169, 691, 4,   5,    6,    3,   741,  3,   5,   6,   5,   6,    741, 741,
    741, 66,   741,  292,  293,  188,  3,   295,  8,   4,   5,   6,    741, 616, 55,  20,  4,    22,   75,  4,    78,  6,   27,  206, 6,    76,  77,
    82,  157,  158,  15,   3,    65,   5,   6,    55,  37,  669, 32,   33,  34,  88,  746, 747,  38,   39,  6,    20,  6,   22,  145, 136,  33,  225,
    27,  227,  78,   3,    10,   5,    6,   157,  158, 87,  6,   4,    51,  3,   175, 176, 51,   3,    24,  25,   90,  60,  7,   78,  157,  158, 36,
    12,  167,  14,   62,   16,   102,  103, 104,  77,  62,  8,   108,  8,   72,  90,  225, 33,   83,   3,   388,  389, 3,   160, 5,   6,    66,  102,
    103, 104,  157,  158,  4,    108,  136, 73,   62,  405, 926, 68,   69,  137, 138, 139, 66,   110,  111, 66,   224, 761, 79,  80,  81,   157, 158,
    424, 55,   157,  158,  234,  428,  429, 137,  138, 139, 167, 225,  411, 65,  134, 65,  439,  440,  441, 442,  443, 444, 32,  33,  34,   225, 62,
    135, 38,   39,   223,  151,  152,  153, 487,  225, 76,  223, 493,  66,  160, 161, 162, 204,  467,  468, 207,  223, 472, 473, 474, 475,  476, 477,
    478, 479,  480,  481,  482,  483,  484, 485,  129, 170, 131, 949,  6,   196, 87,  250, 808,  949,  949, 949,  219, 949, 223, 228, 229,  230, 200,
    227, 228,  227,  224,  949,  3,    219, 223,  170, 7,   208, 9,    10,  11,  12,  13,  14,   228,  16,  230,  524, 186, 157, 158, 171,  23,  78,
    84,  26,   33,   28,   29,   30,   295, 84,   3,   956, 33,  518,  35,  3,   4,   5,   6,    191,  39,  247,  73,  195, 133, 39,  198,  73,  170,
    48,  559,  560,  157,  158,  70,   71,  20,   542, 22,  568, 225,  570, 227, 27,  549, 154,  155,  133, 8,    157, 158, 157, 158, 583,  32,  78,
    157, 158,  3,    136,  157,  158,  55,  41,   175, 176, 152, 153,  154, 155, 157, 158, 1121, 1122, 160, 157,  158, 65,  3,   584, 196,  132, 157,
    158, 174,  73,   74,   8,    157,  158, 167,  161, 157, 158, 84,   83,  86,  85,  88,  628,  1029, 225, 90,   227, 8,   65,  215, 216,  217, 218,
    219, 220,  221,  8,    102,  103,  104, 225,  224, 393, 108, 157,  158, 224, 0,   73,  931,  224,  214, 215,  216, 217, 218, 219, 220,  221, 225,
    224, 227,  8,    427,  367,  8,    222, 65,   8,   224, 225, 171,  137, 138, 139, 140, 577,  142,  224, 144,  73,  146, 224, 148, 65,   150, 692,
    8,   694,  1215, 72,   1217, 1042, 65,  159,  225, 703, 227, 8,    199, 3,   404, 32,  33,   1230, 84,  1232, 86,  133, 88,  210, 225,  73,  8,
    721, 229,  181,  78,   183,  65,   185, 51,   65,  223, 72,  65,   73,  152, 153, 154, 155,  32,   33,  3,    716, 160, 129, 226, 131,  132, 14,
    134, 189,  190,  65,   742,  3,    4,   5,    6,   51,  787, 217,  218, 219, 65,  168, 169,  223,  224, 181,  173, 183, 228, 185, 230,  224, 225,
    129, 3,    131,  132,  3,    39,   187, 188,  42,  43,  784, 192,  786, 47,  48,  3,   535,  791,  133, 769,  552, 214, 215, 216, 217,  218, 219,
    220, 221,  44,   45,   224,  223,  161, 162,  163, 73,  152, 153,  154, 155, 73,  157, 3,    1218, 160, 224,  225, 73,  74,  3,   179,  584, 3,
    4,   5,    6,    1231, 83,   1233, 85,  157,  158, 88,  122, 90,   124, 125, 224, 225, 128,  1245, 3,   4,    5,   6,   162, 102, 103,  104, 203,
    204, 205,  108,  5,    6,    172,  6,   1262, 175, 176, 177, 1266, 179, 53,  54,  55,  56,   904,  214, 215,  216, 217, 218, 219, 220,  221, 193,
    194, 3,    196,  197,  137,  138,  139, 140,  62,  142, 3,   144,  55,  146, 225, 148, 39,   150,  6,   42,   43,  219, 220, 221, 47,   48,  159,
    46,  47,   48,   49,   224,  225,  196, 73,   74,  224, 225, 4,    78,  224, 225, 224, 225,  83,   76,  85,   196, 181, 224, 183, 90,   185, 56,
    57,  58,   59,   60,   61,   194,  63,  64,   196, 102, 103, 104,  224, 225, 196, 108, 3,    4,    5,   6,    225, 226, 716, 224, 225,  49,  50,
    224, 225,  196,  217,  218,  219,  224, 225,  21,  223, 196, 224,  225, 224, 228, 67,  230,  137,  138, 139,  140, 65,  142, 3,   144,  73,  146,
    735, 148,  3,    150,  62,   73,   62,  75,   224, 225, 224, 225,  159, 223, 91,  92,  93,   94,   95,  224,  225, 98,  99,  74,  3,    4,   5,
    6,   105,  106,  107,  3,    73,   74,  181,  3,   183, 3,   185,  224, 225, 3,   83,  66,   85,   224, 225,  224, 225, 90,  127, 224,  225, 224,
    225, 1045, 224,  225,  44,   45,   4,   102,  103, 104, 224, 133,  132, 108, 42,  43,  217,  218,  219, 946,  947, 132, 223, 3,   1044, 3,   132,
    228, 4,    230,  152,  153,  154,  155, 156,  132, 223, 62,  160,  4,   73,  74,  137, 138,  139,  140, 167,  142, 6,   144, 83,  146,  85,  148,
    3,   150,  6,    90,   4,    3,    55,  4,    4,   3,   159, 3,    196, 3,   53,  102, 103,  104,  68,  74,   3,   108, 3,   4,   5,    6,   62,
    135, 1126, 223,  78,   78,   181,  214, 183,  223, 185, 4,   214,  215, 216, 217, 218, 219,  220,  221, 223,  4,   4,   4,   137, 138,  139, 140,
    4,   142,  223,  144,  6,    146,  223, 148,  223, 150, 226, 223,  223, 223, 217, 218, 219,  223,  159, 223,  223, 3,   223, 6,   46,   228, 46,
    230, 4,    78,   7,    6,    9,    10,  11,   12,  13,  14,  73,   16,  181, 77,  183, 77,   185,  4,   23,   224, 83,  26,  85,  28,   29,  30,
    3,   90,   224,  69,   4,    55,   3,   78,   223, 3,   223, 223,  223, 102, 103, 104, 223,  4,    223, 108,  23,  4,   217, 218, 219,  223, 223,
    30,  223,  32,   33,   223,  35,   228, 4,    230, 77,  40,  41,   224, 231, 3,   223, 6,    5,    228, 6,    78,  137, 138, 139, 140,  6,   142,
    42,  144,  223,  146,  223,  148,  3,   150,  223, 4,   6,   224,  167, 76,  167, 130, 159,  225,  0,   223,  3,   135, 3,   73,  160,  7,   225,
    9,   10,   11,   12,   13,   14,   15,  16,   17,  224, 76,  181,  4,   183, 23,  185, 225,  26,   223, 28,   29,  30,  157, 7,   132,  9,   10,
    11,  12,   13,   14,   15,   16,   17,  231,  231, 6,   6,   3,    23,  3,   31,  26,  4,    28,   29,  30,   217, 218, 219, 6,   4,    225, 223,
    180, 4,    133,  223,  228,  171,  230, 223,  223, 223, 223, 223,  4,   6,   4,   78,  223,  6,    223, 223,  223, 152, 153, 154, 155,  163, 164,
    165, 166,  160,  168,  169,  224,  199, 172,  173, 174, 223, 3,    177, 78,  179, 6,   224,  210,  183, 184,  225, 4,   224, 78,  224,  46,  6,
    5,   214,  6,    223,  196,  197,  6,   132,  200, 201, 202, 225,  134, 223, 206, 130, 135,  224,  210, 211,  212, 167, 228, 4,   225,  224, 223,
    6,   224,  214,  215,  216,  217,  218, 219,  220, 221, 91,  92,   93,  94,  95,  6,   57,   98,   99,  57,   3,   93,  52,  225, 105,  106, 107,
    171, 224,  93,   225,  225,  225,  91,  92,   93,  94,  95,  225,  93,  98,  99,  225, 187,  225,  225, 127,  105, 106, 107, 225, 225,  171, 225,
    225, 199,  91,   92,   93,   94,   95,  225,  225, 98,  99,  209,  210, 93,  187, 127, 105,  106,  107, 93,   225, 225, 4,   225, 225,  223, 199,
    93,  3,    6,    225,  225,  225,  225, 225,  225, 209, 210, 127,  225, 225, 225, 225, 225,  66,   225, 53,   225, 225, 225, 223, 89,   90,  91,
    92,  93,   94,   95,   96,   97,   98,  99,   100, 101, 102, 103,  104, 105, 106, 225, 108,  109,  110, 111,  112, 113, 225, 225, 116,  225, 135,
    119, 120,  121,  122,  123,  124,  125, 126,  91,  92,  93,  94,   95,  225, 225, 98,  99,   54,   135, 6,    223, 225, 105, 106, 107,  91,  92,
    93,  94,   95,   225,  225,  98,   99,  225,  225, 6,   225, 225,  105, 106, 107, 225, 225,  127,  140, 141,  142, 143, 144, 145, 146,  147, 148,
    149, 150,  151,  225,  6,    6,    224, 127,  225, 6,   6,   225,  225, 225, 225, 225, 6,    225,  225, 225,  225, 225, 225, 225, 6,    225, 225,
    6,   6,    225,  6,    6,    6,    6,   6,    6,   6,   6,   6,    6,   225, 6,   6,   6,    6,    6,   6,    6,   6,   6,   6,   225,  6,   6,
    6,   6,    6,    6,    6,    6,    6,   6,    225, 6,   6,   6,    6,   6,   6,   6,   6,    6,    6,   225,  6,   6,   6,   6,   6,    6,   6,
    6,   6,    6,    225,  6,    4,    4,   4,    4,   4,   224, 224,  6,   224, 224, 62,  224,  224,  224, 224,  224, 224, 224, 6,   224,  224, 6,
    224, 224,  224,  224,  6,    224,  6,   224,  224, 224, 224, 224,  224, 224, 224, 224, 224,  224,  224, 224,  224, 224, 224, 224, 224,  6,   224,
    224, 224,  224,  224,  224,  224,  224, 224,  224, 224, 224, 224,  224, 224, 224, 224, 224,  224,  224, 224,  224, 224, 224, 224, 6,    224, 4,
    225, 225,  225,  224,  224,  224,  224, 224,  224, 4,   224, 224,  6,   6,   225, 225, 6,    224,  4,   224,  47,  6,   6,   224, 224,  39,  39,
    223, 39,   39,   3,    39,   39,   223, 223,  3,   224, 3,   758,  224, 224, 65,  671, 245,  760,  140, 406,  22,  368, 944, 628, 234,  205, 465,
    491, 811,  569,  949,  489,  788,  -1,  776,  910, 805, -1,  -1,   281, -1,  -1,  788, 788,  -1,   -1,  -1,   430};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] = {
    0,   7,   9,   10,  11,  12,  13,  14,  15,  16,  17,  23,  26,  28,  29,  30,  78,  171, 187, 199, 209, 210, 223, 233, 234, 235, 237, 249, 250,
    252, 253, 256, 257, 258, 259, 260, 284, 289, 290, 291, 292, 293, 294, 295, 296, 32,  33,  34,  38,  39,  196, 36,  32,  33,  34,  38,  39,  196,
    3,   282, 76,  282, 168, 169, 173, 3,   236, 237, 249, 250, 253, 256, 257, 258, 289, 290, 291, 292, 293, 3,   23,  30,  32,  33,  35,  40,  41,
    163, 164, 165, 166, 168, 169, 172, 173, 174, 177, 179, 183, 184, 196, 197, 200, 201, 202, 206, 210, 211, 212, 3,   33,  33,  39,  161, 162, 163,
    179, 3,   282, 3,   285, 286, 174, 7,   12,  14,  16,  171, 191, 195, 198, 32,  33,  51,  33,  51,  161, 259, 260, 0,   222, 345, 20,  22,  27,
    278, 8,   261, 263, 72,  344, 344, 344, 344, 344, 346, 3,   282, 72,  343, 343, 343, 343, 343, 3,   226, 14,  282, 76,  77,  223, 3,   3,   3,
    236, 223, 73,  73,  3,   282, 6,   208, 175, 176, 175, 176, 3,   4,   77,  6,   3,   203, 204, 205, 204, 207, 6,   73,  6,   73,  62,  282, 282,
    3,   55,  225, 6,   196, 196, 187, 188, 192, 162, 172, 175, 176, 177, 179, 193, 194, 196, 197, 196, 4,   194, 76,  196, 196, 196, 282, 224, 224,
    235, 21,  223, 262, 263, 65,  270, 67,  264, 73,  3,   282, 282, 282, 3,   62,  62,  223, 251, 74,  3,   282, 282, 282, 3,   3,   3,   254, 255,
    66,  275, 4,   342, 342, 3,   4,   5,   6,   73,  74,  83,  85,  90,  102, 103, 104, 108, 137, 138, 139, 140, 142, 144, 146, 148, 150, 159, 181,
    183, 185, 217, 218, 219, 223, 228, 230, 297, 299, 300, 301, 302, 303, 304, 305, 306, 307, 310, 311, 312, 313, 314, 316, 317, 318, 319, 320, 322,
    323, 324, 325, 326, 327, 328, 329, 330, 331, 334, 335, 336, 337, 338, 339, 3,   5,   6,   62,  170, 3,   5,   6,   62,  170, 3,   5,   6,   62,
    170, 224, 132, 132, 39,  42,  43,  47,  48,  3,   3,   342, 4,   132, 132, 282, 10,  24,  25,  62,  223, 286, 342, 4,   167, 6,   3,   6,   4,
    3,   4,   55,  4,   196, 3,   3,   3,   262, 263, 297, 53,  68,  268, 74,  135, 55,  223, 251, 282, 32,  33,  51,  3,   248, 37,  260, 62,  214,
    225, 275, 300, 78,  78,  223, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 73,  74,  301, 223, 223, 88,  300, 315, 4,   4,   4,
    4,   4,   6,   339, 223, 223, 223, 223, 223, 223, 223, 122, 124, 125, 128, 223, 223, 301, 301, 260, 300, 5,   6,   227, 320, 332, 333, 229, 319,
    224, 225, 55,  157, 158, 73,  75,  133, 152, 153, 154, 155, 156, 160, 214, 215, 216, 217, 218, 219, 220, 221, 226, 225, 227, 225, 227, 225, 227,
    225, 229, 225, 227, 225, 227, 3,   6,   78,  347, 46,  46,  77,  282, 261, 4,   39,  48,  6,   77,  189, 190, 4,   224, 224, 82,  271, 265, 266,
    300, 300, 69,  269, 4,   258, 3,   129, 131, 238, 240, 241, 247, 55,  223, 351, 3,   3,   224, 225, 223, 298, 282, 300, 255, 223, 223, 65,  224,
    297, 223, 73,  260, 300, 300, 315, 84,  86,  88,  300, 300, 300, 300, 300, 300, 4,   223, 223, 223, 223, 4,   4,   224, 224, 231, 77,  299, 3,
    300, 300, 75,  160, 223, 73,  132, 301, 301, 301, 301, 301, 301, 301, 301, 301, 301, 301, 301, 301, 301, 3,   219, 320, 6,   332, 6,   333, 319,
    6,   5,   42,  44,  45,  223, 223, 223, 3,   224, 6,   32,  41,  4,   167, 167, 297, 76,  272, 225, 70,  71,  267, 300, 135, 89,  90,  91,  92,
    93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 108, 109, 110, 111, 112, 113, 116, 119, 120, 121, 122, 123, 124, 125, 126,
    243, 130, 223, 224, 225, 258, 3,   135, 3,   297, 225, 68,  69,  79,  80,  81,  186, 340, 341, 340, 297, 224, 260, 224, 55,  87,  84,  86,  300,
    300, 224, 224, 224, 224, 224, 224, 76,  300, 3,   318, 300, 4,   225, 274, 224, 225, 5,   6,   342, 223, 301, 260, 297, 132, 157, 231, 231, 6,
    6,   3,   349, 350, 248, 239, 241, 6,   4,   4,   223, 279, 280, 281, 282, 287, 180, 273, 266, 4,   223, 223, 223, 223, 223, 223, 223, 223, 223,
    73,  129, 131, 132, 244, 245, 347, 223, 248, 31,  348, 240, 224, 4,   224, 223, 6,   6,   4,   3,   6,   224, 225, 224, 224, 224, 243, 300, 300,
    84,  87,  301, 225, 225, 225, 225, 4,   66,  224, 4,   78,  260, 297, 224, 224, 301, 49,  50,  46,  214, 224, 225, 224, 224, 225, 6,   258, 225,
    56,  58,  59,  60,  61,  63,  64,  288, 3,   55,  283, 302, 303, 304, 305, 306, 307, 308, 309, 275, 6,   243, 242, 243, 91,  92,  93,  94,  95,
    98,  99,  105, 106, 107, 127, 91,  92,  93,  94,  95,  98,  99,  105, 106, 107, 127, 91,  92,  93,  94,  95,  98,  99,  105, 106, 107, 127, 91,
    92,  93,  94,  95,  98,  99,  105, 106, 107, 127, 91,  92,  93,  94,  95,  98,  99,  105, 106, 107, 127, 91,  92,  93,  94,  95,  98,  99,  105,
    106, 107, 127, 132, 130, 134, 245, 246, 246, 248, 224, 223, 135, 167, 297, 341, 224, 84,  300, 224, 300, 320, 228, 321, 324, 329, 334, 4,   225,
    274, 300, 224, 223, 224, 224, 6,   6,   3,   4,   5,   6,   350, 241, 33,  35,  224, 280, 57,  57,  3,   225, 52,  277, 224, 225, 224, 224, 225,
    225, 225, 225, 225, 225, 225, 225, 225, 225, 93,  225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 93,  225, 225, 225, 225, 225, 225, 225, 225,
    225, 225, 93,  225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 93,  225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 93,  225, 225, 225, 225,
    225, 225, 225, 225, 225, 225, 93,  225, 319, 135, 135, 224, 349, 4,   3,   224, 225, 225, 225, 225, 274, 224, 340, 6,   283, 281, 281, 223, 308,
    53,  54,  276, 6,   243, 6,   6,   6,   6,   6,   6,   6,   6,   6,   225, 6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   225, 6,   6,   6,
    6,   6,   6,   6,   6,   6,   6,   225, 6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   225, 6,   6,   6,   6,   6,   6,   6,   6,   6,   6,
    225, 6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   225, 6,   4,   4,   224, 347, 4,   4,   4,   4,   224, 224, 6,   62,  248, 297, 300, 224,
    224, 224, 224, 224, 224, 224, 224, 224, 224, 6,   224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 6,   224, 224, 224, 224, 224, 224, 224, 224,
    224, 224, 6,   224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 6,   224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 6,   224, 224, 224, 224,
    224, 224, 224, 224, 224, 224, 6,   224, 225, 225, 225, 274, 274, 39,  42,  43,  47,  48,  300, 224, 224, 224, 224, 224, 224, 224, 4,   4,   6,
    224, 224, 6,   6,   225, 274, 225, 274, 347, 6,   44,  45,  6,   224, 4,   224, 42,  43,  6,   274, 347, 274, 136, 167, 347, 6,   47,  224, 224,
    39,  39,  136, 167, 347, 136, 167, 223, 39,  39,  39,  39,  3,   223, 223, 224, 3,   3,   347, 224, 224, 347};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] = {
    0,   232, 233, 234, 234, 235, 235, 235, 235, 235, 235, 235, 235, 235, 235, 235, 235, 235, 235, 235, 235, 236, 236, 236, 236, 236, 236, 236, 236,
    236, 236, 236, 236, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 238, 238, 239, 239, 240, 240, 241, 241, 241, 241, 242, 242, 243, 243, 243,
    243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243,
    243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243,
    243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243,
    243, 243, 243, 243, 243, 244, 244, 245, 245, 245, 245, 246, 246, 247, 247, 248, 248, 249, 250, 250, 251, 251, 252, 252, 253, 254, 254, 255, 256,
    256, 256, 256, 256, 257, 257, 257, 258, 258, 258, 258, 259, 259, 260, 261, 262, 262, 263, 264, 264, 265, 265, 266, 267, 267, 267, 268, 268, 269,
    269, 270, 270, 271, 271, 272, 272, 273, 273, 274, 274, 275, 275, 276, 276, 277, 277, 278, 278, 278, 278, 279, 279, 280, 280, 281, 281, 282, 282,
    283, 283, 283, 283, 284, 284, 285, 285, 286, 287, 287, 288, 288, 288, 288, 288, 288, 288, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289,
    289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289,
    289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 290, 290, 290, 290, 291, 292, 292, 292, 292, 292, 292, 292, 292,
    292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 293, 294, 294, 294, 294, 294, 294, 294, 294, 294, 294,
    294, 294, 294, 294, 294, 294, 294, 294, 294, 294, 294, 294, 294, 294, 294, 294, 294, 294, 294, 294, 294, 294, 294, 294, 294, 294, 295, 295, 295,
    295, 296, 296, 297, 297, 298, 298, 299, 299, 300, 300, 300, 300, 300, 301, 301, 301, 301, 301, 301, 301, 301, 301, 301, 301, 301, 301, 302, 302,
    302, 303, 303, 303, 303, 304, 304, 304, 304, 305, 305, 306, 306, 307, 307, 308, 308, 308, 308, 308, 308, 309, 309, 310, 310, 310, 310, 310, 310,
    310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 311, 311, 312, 313, 313, 314,
    314, 314, 314, 315, 315, 316, 317, 317, 317, 317, 318, 318, 318, 318, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 320,
    320, 320, 320, 321, 321, 321, 322, 323, 323, 324, 324, 325, 326, 326, 327, 328, 328, 329, 330, 330, 331, 331, 332, 333, 334, 334, 335, 336, 336,
    337, 338, 338, 339, 339, 339, 339, 339, 339, 339, 339, 339, 339, 339, 339, 340, 340, 341, 341, 341, 341, 341, 341, 342, 343, 343, 344, 344, 345,
    345, 346, 346, 347, 347, 348, 348, 349, 349, 350, 350, 350, 350, 350, 351, 351};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] = {
    0,  2,  2,  1,  3,  1, 1,  1, 1, 1, 1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 4, 4,  8,  6,  10, 8,  7,
    6,  8,  1,  3,  1,  3, 1,  1, 4, 4, 6, 6, 1, 3,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1,  1,  6,  4,  1,
    6,  6,  6,  6,  6,  6, 6,  6, 6, 6, 7, 6, 6, 6,  6,  6,  6,  6,  6,  6,  6,  7,  6,  6,  6, 6, 6, 6, 6, 6, 6, 6, 7, 6, 6, 6,  6,  6,  6,  6,  6,
    6,  6,  7,  6,  6,  6, 6,  6, 6, 6, 6, 6, 6, 7,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7, 4, 4, 1, 2, 2, 1, 1, 2, 2, 0, 5,  4,  1,  3,  4,  6,
    5,  3,  0,  3,  2,  5, 1,  3, 3, 4, 4, 4, 4, 6,  8,  11, 8,  1,  1,  3,  3,  3,  3,  2,  4, 3, 3, 9, 3, 0, 1, 3, 2, 1, 1, 0,  2,  0,  2,  0,  1,
    0,  2,  0,  2,  0,  2, 0,  3, 0, 2, 0, 2, 0, 3,  0,  1,  2,  1,  1,  1,  3,  1,  1,  2,  4, 1, 3, 2, 1, 5, 0, 2, 0, 1, 3, 5,  4,  6,  1,  1,  1,
    1,  1,  1,  0,  2,  2, 2,  2, 3, 2, 2, 2, 2, 3,  2,  3,  3,  3,  3,  4,  4,  3,  3,  4,  4, 5, 6, 7, 9, 4, 5, 7, 8, 9, 2, 2,  3,  4,  3,  3,  4,
    2,  3,  3,  4,  2,  3, 2,  2, 2, 4, 2, 4, 3, 2,  4,  2,  2,  4,  3,  2,  2,  2,  2,  2,  2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  4,  4,  4,  4,  4,
    6,  6,  5,  3,  4,  4, 4,  2, 5, 3, 6, 7, 9, 10, 12, 12, 13, 14, 15, 16, 12, 13, 15, 16, 3, 4, 5, 6, 3, 3, 4, 3, 3, 4, 4, 6,  5,  3,  4,  3,  4,
    3,  3,  5,  7,  7,  6, 8,  8, 5, 2, 3, 1, 3, 3,  5,  3,  1,  1,  1,  1,  1,  1,  3,  3,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 19, 16, 20, 16, 15,
    13, 18, 14, 13, 11, 8, 10, 5, 7, 4, 6, 1, 1, 1,  1,  1,  1,  1,  3,  3,  4,  5,  4,  4,  4, 4, 4, 4, 4, 3, 2, 2, 2, 3, 3, 3,  3,  3,  3,  3,  3,
    3,  3,  3,  3,  6,  3, 4,  3, 3, 5, 5, 6, 4, 6,  3,  5,  4,  5,  6,  4,  5,  5,  6,  1,  3, 1, 3, 1, 1, 1, 1, 1, 2, 2, 2, 2,  2,  2,  1,  1,  1,
    1,  1,  1,  1,  1,  1, 1,  2, 2, 3, 1, 1, 2, 2,  3,  2,  2,  3,  2,  2,  2,  2,  3,  3,  3, 1, 1, 2, 2, 3, 2, 2, 3, 2, 2, 2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  1,  3, 2,  2, 1, 2, 2, 2, 1, 2,  0,  3,  0,  1,  0,  2,  0,  4,  0,  4,  0, 1, 3, 1, 3, 3, 3, 3, 6, 3};

enum { YYENOMEM = -2 };

#define yyerrok (yyerrstatus = 0)
#define yyclearin (yychar = SQLEMPTY)

#define YYACCEPT goto yyacceptlab
#define YYABORT goto yyabortlab
#define YYERROR goto yyerrorlab
#define YYNOMEM goto yyexhaustedlab

#define YYRECOVERING() (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                                                                                                       \
    do                                                                                                                                               \
        if (yychar == SQLEMPTY) {                                                                                                                    \
            yychar = (Token);                                                                                                                        \
            yylval = (Value);                                                                                                                        \
            YYPOPSTACK(yylen);                                                                                                                       \
            yystate = *yyssp;                                                                                                                        \
            goto yybackup;                                                                                                                           \
        } else {                                                                                                                                     \
            yyerror(&yylloc, scanner, result, YY_("syntax error: cannot back up"));                                                                  \
            YYERROR;                                                                                                                                 \
        }                                                                                                                                            \
    while (0)

/* Backward compatibility with an undocumented macro.
   Use SQLerror or SQLUNDEF. */
#define YYERRCODE SQLUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
#define YYLLOC_DEFAULT(Current, Rhs, N)                                                                                                              \
    do                                                                                                                                               \
        if (N) {                                                                                                                                     \
            (Current).first_line = YYRHSLOC(Rhs, 1).first_line;                                                                                      \
            (Current).first_column = YYRHSLOC(Rhs, 1).first_column;                                                                                  \
            (Current).last_line = YYRHSLOC(Rhs, N).last_line;                                                                                        \
            (Current).last_column = YYRHSLOC(Rhs, N).last_column;                                                                                    \
        } else {                                                                                                                                     \
            (Current).first_line = (Current).last_line = YYRHSLOC(Rhs, 0).last_line;                                                                 \
            (Current).first_column = (Current).last_column = YYRHSLOC(Rhs, 0).last_column;                                                           \
        }                                                                                                                                            \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])

/* Enable debugging if requested.  */
#if SQLDEBUG

#ifndef YYFPRINTF
#include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#define YYFPRINTF fprintf
#endif

#define YYDPRINTF(Args)                                                                                                                              \
    do {                                                                                                                                             \
        if (yydebug)                                                                                                                                 \
            YYFPRINTF Args;                                                                                                                          \
    } while (0)

/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YYLOCATION_PRINT

#if defined YY_LOCATION_PRINT

/* Temporary convenience wrapper in case some people defined the
   undocumented and private YY_LOCATION_PRINT macros.  */
#define YYLOCATION_PRINT(File, Loc) YY_LOCATION_PRINT(File, *(Loc))

#elif defined SQLLTYPE_IS_TRIVIAL && SQLLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int yy_location_print_(FILE *yyo, YYLTYPE const *const yylocp) {
    int res = 0;
    int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
    if (0 <= yylocp->first_line) {
        res += YYFPRINTF(yyo, "%d", yylocp->first_line);
        if (0 <= yylocp->first_column)
            res += YYFPRINTF(yyo, ".%d", yylocp->first_column);
    }
    if (0 <= yylocp->last_line) {
        if (yylocp->first_line < yylocp->last_line) {
            res += YYFPRINTF(yyo, "-%d", yylocp->last_line);
            if (0 <= end_col)
                res += YYFPRINTF(yyo, ".%d", end_col);
        } else if (0 <= end_col && yylocp->first_column < end_col)
            res += YYFPRINTF(yyo, "-%d", end_col);
    }
    return res;
}

#define YYLOCATION_PRINT yy_location_print_

/* Temporary convenience wrapper in case some people defined the
   undocumented and private YY_LOCATION_PRINT macros.  */
#define YY_LOCATION_PRINT(File, Loc) YYLOCATION_PRINT(File, &(Loc))

#else

#define YYLOCATION_PRINT(File, Loc) ((void)0)
/* Temporary convenience wrapper in case some people defined the
   undocumented and private YY_LOCATION_PRINT macros.  */
#define YY_LOCATION_PRINT YYLOCATION_PRINT

#endif
#endif /* !defined YYLOCATION_PRINT */

#define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                                                                                                \
    do {                                                                                                                                             \
        if (yydebug) {                                                                                                                               \
            YYFPRINTF(stderr, "%s ", Title);                                                                                                         \
            yy_symbol_print(stderr, Kind, Value, Location, scanner, result);                                                                         \
            YYFPRINTF(stderr, "\n");                                                                                                                 \
        }                                                                                                                                            \
    } while (0)

/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void yy_symbol_value_print(FILE *yyo,
                                  yysymbol_kind_t yykind,
                                  YYSTYPE const *const yyvaluep,
                                  YYLTYPE const *const yylocationp,
                                  void *scanner,
                                  infinity::ParserResult *result) {
    FILE *yyoutput = yyo;
    YY_USE(yyoutput);
    YY_USE(yylocationp);
    YY_USE(scanner);
    YY_USE(result);
    if (!yyvaluep)
        return;
    YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
    YY_USE(yykind);
    YY_IGNORE_MAYBE_UNINITIALIZED_END
}

/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void yy_symbol_print(FILE *yyo,
                            yysymbol_kind_t yykind,
                            YYSTYPE const *const yyvaluep,
                            YYLTYPE const *const yylocationp,
                            void *scanner,
                            infinity::ParserResult *result) {
    YYFPRINTF(yyo, "%s %s (", yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name(yykind));

    YYLOCATION_PRINT(yyo, yylocationp);
    YYFPRINTF(yyo, ": ");
    yy_symbol_value_print(yyo, yykind, yyvaluep, yylocationp, scanner, result);
    YYFPRINTF(yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void yy_stack_print(yy_state_t *yybottom, yy_state_t *yytop) {
    YYFPRINTF(stderr, "Stack now");
    for (; yybottom <= yytop; yybottom++) {
        int yybot = *yybottom;
        YYFPRINTF(stderr, " %d", yybot);
    }
    YYFPRINTF(stderr, "\n");
}

#define YY_STACK_PRINT(Bottom, Top)                                                                                                                  \
    do {                                                                                                                                             \
        if (yydebug)                                                                                                                                 \
            yy_stack_print((Bottom), (Top));                                                                                                         \
    } while (0)

/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void yy_reduce_print(yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, void *scanner, infinity::ParserResult *result) {
    int yylno = yyrline[yyrule];
    int yynrhs = yyr2[yyrule];
    int yyi;
    YYFPRINTF(stderr, "Reducing stack by rule %d (line %d):\n", yyrule - 1, yylno);
    /* The symbols being reduced.  */
    for (yyi = 0; yyi < yynrhs; yyi++) {
        YYFPRINTF(stderr, "   $%d = ", yyi + 1);
        yy_symbol_print(stderr,
                        YY_ACCESSING_SYMBOL(+yyssp[yyi + 1 - yynrhs]),
                        &yyvsp[(yyi + 1) - (yynrhs)],
                        &(yylsp[(yyi + 1) - (yynrhs)]),
                        scanner,
                        result);
        YYFPRINTF(stderr, "\n");
    }
}

#define YY_REDUCE_PRINT(Rule)                                                                                                                        \
    do {                                                                                                                                             \
        if (yydebug)                                                                                                                                 \
            yy_reduce_print(yyssp, yyvsp, yylsp, Rule, scanner, result);                                                                             \
    } while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !SQLDEBUG */
#define YYDPRINTF(Args) ((void)0)
#define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
#define YY_STACK_PRINT(Bottom, Top)
#define YY_REDUCE_PRINT(Rule)
#endif /* !SQLDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
#define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Context of a parse error.  */
typedef struct {
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
static int yypcontext_expected_tokens(const yypcontext_t *yyctx, yysymbol_kind_t yyarg[], int yyargn) {
    /* Actual size of YYARG. */
    int yycount = 0;
    int yyn = yypact[+*yyctx->yyssp];
    if (!yypact_value_is_default(yyn)) {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        int yyxbegin = yyn < 0 ? -yyn : 0;
        /* Stay within bounds of both yycheck and yytname.  */
        int yychecklim = YYLAST - yyn + 1;
        int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        int yyx;
        for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror && !yytable_value_is_error(yytable[yyx + yyn])) {
                if (!yyarg)
                    ++yycount;
                else if (yycount == yyargn)
                    return 0;
                else
                    yyarg[yycount++] = YY_CAST(yysymbol_kind_t, yyx);
            }
    }
    if (yyarg && yycount == 0 && 0 < yyargn)
        yyarg[0] = YYSYMBOL_YYEMPTY;
    return yycount;
}

#ifndef yystrlen
#if defined __GLIBC__ && defined _STRING_H
#define yystrlen(S) (YY_CAST(YYPTRDIFF_T, strlen(S)))
#else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T yystrlen(const char *yystr) {
    YYPTRDIFF_T yylen;
    for (yylen = 0; yystr[yylen]; yylen++)
        continue;
    return yylen;
}
#endif
#endif

#ifndef yystpcpy
#if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#define yystpcpy stpcpy
#else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *yystpcpy(char *yydest, const char *yysrc) {
    char *yyd = yydest;
    const char *yys = yysrc;

    while ((*yyd++ = *yys++) != '\0')
        continue;

    return yyd - 1;
}
#endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T yytnamerr(char *yyres, const char *yystr) {
    if (*yystr == '"') {
        YYPTRDIFF_T yyn = 0;
        char const *yyp = yystr;
        for (;;)
            switch (*++yyp) {
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
    do_not_strip_quotes:;
    }

    if (yyres)
        return yystpcpy(yyres, yystr) - yyres;
    else
        return yystrlen(yystr);
}
#endif

static int yy_syntax_error_arguments(const yypcontext_t *yyctx, yysymbol_kind_t yyarg[], int yyargn) {
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
    if (yyctx->yytoken != YYSYMBOL_YYEMPTY) {
        int yyn;
        if (yyarg)
            yyarg[yycount] = yyctx->yytoken;
        ++yycount;
        yyn = yypcontext_expected_tokens(yyctx, yyarg ? yyarg + 1 : yyarg, yyargn - 1);
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
static int yysyntax_error(YYPTRDIFF_T *yymsg_alloc, char **yymsg, const yypcontext_t *yyctx) {
    enum { YYARGS_MAX = 5 };
    /* Internationalized format string. */
    const char *yyformat = YY_NULLPTR;
    /* Arguments of yyformat: reported tokens (one for the "unexpected",
       one per "expected"). */
    yysymbol_kind_t yyarg[YYARGS_MAX];
    /* Cumulated lengths of YYARG.  */
    YYPTRDIFF_T yysize = 0;

    /* Actual size of YYARG. */
    int yycount = yy_syntax_error_arguments(yyctx, yyarg, YYARGS_MAX);
    if (yycount == YYENOMEM)
        return YYENOMEM;

    switch (yycount) {
#define YYCASE_(N, S)                                                                                                                                \
    case N:                                                                                                                                          \
        yyformat = S;                                                                                                                                \
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
    yysize = yystrlen(yyformat) - 2 * yycount + 1;
    {
        int yyi;
        for (yyi = 0; yyi < yycount; ++yyi) {
            YYPTRDIFF_T yysize1 = yysize + yytnamerr(YY_NULLPTR, yytname[yyarg[yyi]]);
            if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                yysize = yysize1;
            else
                return YYENOMEM;
        }
    }

    if (*yymsg_alloc < yysize) {
        *yymsg_alloc = 2 * yysize;
        if (!(yysize <= *yymsg_alloc && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
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
            if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount) {
                yyp += yytnamerr(yyp, yytname[yyarg[yyi++]]);
                yyformat += 2;
            } else {
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
yydestruct(const char *yymsg, yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, void *scanner, infinity::ParserResult *result) {
    YY_USE(yyvaluep);
    YY_USE(yylocationp);
    YY_USE(scanner);
    YY_USE(result);
    if (!yymsg)
        yymsg = "Deleting";
    YY_SYMBOL_PRINT(yymsg, yykind, yyvaluep, yylocationp);

    YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
    switch (yykind) {
        case YYSYMBOL_IDENTIFIER: /* IDENTIFIER  */
#line 331 "parser.y"
        {
            free(((*yyvaluep).str_value));
        }
#line 2496 "parser.cpp"
        break;

        case YYSYMBOL_STRING: /* STRING  */
#line 331 "parser.y"
        {
            free(((*yyvaluep).str_value));
        }
#line 2504 "parser.cpp"
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
#line 2518 "parser.cpp"
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
#line 2532 "parser.cpp"
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
#line 2546 "parser.cpp"
        break;

        case YYSYMBOL_column_type_array: /* column_type_array  */
#line 192 "parser.y"
        {
            fprintf(stderr, "destroy column_type_array\n");
            delete (((*yyvaluep).column_type_array_t));
        }
#line 2555 "parser.cpp"
        break;

        case YYSYMBOL_column_type: /* column_type  */
#line 187 "parser.y"
        {
            fprintf(stderr, "destroy column_type\n");
            delete (((*yyvaluep).column_type_t));
        }
#line 2564 "parser.cpp"
        break;

        case YYSYMBOL_column_constraints: /* column_constraints  */
#line 324 "parser.y"
        {
            fprintf(stderr, "destroy constraints\n");
            if ((((*yyvaluep).column_constraints_t)) != nullptr) {
                delete (((*yyvaluep).column_constraints_t));
            }
        }
#line 2575 "parser.cpp"
        break;

        case YYSYMBOL_default_expr: /* default_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 2583 "parser.cpp"
        break;

        case YYSYMBOL_identifier_array: /* identifier_array  */
#line 335 "parser.y"
        {
            fprintf(stderr, "destroy identifier array\n");
            delete (((*yyvaluep).identifier_array_t));
        }
#line 2592 "parser.cpp"
        break;

        case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 335 "parser.y"
        {
            fprintf(stderr, "destroy identifier array\n");
            delete (((*yyvaluep).identifier_array_t));
        }
#line 2601 "parser.cpp"
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
#line 2615 "parser.cpp"
        break;

        case YYSYMBOL_update_expr: /* update_expr  */
#line 288 "parser.y"
        {
            fprintf(stderr, "destroy update expr\n");
            if (((*yyvaluep).update_expr_t) != nullptr) {
                delete ((*yyvaluep).update_expr_t);
            }
        }
#line 2626 "parser.cpp"
        break;

        case YYSYMBOL_select_statement: /* select_statement  */
#line 370 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2636 "parser.cpp"
        break;

        case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 370 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2646 "parser.cpp"
        break;

        case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 370 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2656 "parser.cpp"
        break;

        case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 370 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2666 "parser.cpp"
        break;

        case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 370 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2676 "parser.cpp"
        break;

        case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 370 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2686 "parser.cpp"
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
#line 2700 "parser.cpp"
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
#line 2714 "parser.cpp"
        break;

        case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 358 "parser.y"
        {
            fprintf(stderr, "destroy order by expr\n");
            delete ((*yyvaluep).order_by_expr_t)->expr_;
            delete ((*yyvaluep).order_by_expr_t);
        }
#line 2724 "parser.cpp"
        break;

        case YYSYMBOL_limit_expr: /* limit_expr  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2732 "parser.cpp"
        break;

        case YYSYMBOL_offset_expr: /* offset_expr  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2740 "parser.cpp"
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
#line 2754 "parser.cpp"
        break;

        case YYSYMBOL_from_clause: /* from_clause  */
#line 353 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2763 "parser.cpp"
        break;

        case YYSYMBOL_search_clause: /* search_clause  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2771 "parser.cpp"
        break;

        case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2779 "parser.cpp"
        break;

        case YYSYMBOL_where_clause: /* where_clause  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2787 "parser.cpp"
        break;

        case YYSYMBOL_having_clause: /* having_clause  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2795 "parser.cpp"
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
#line 2809 "parser.cpp"
        break;

        case YYSYMBOL_table_reference: /* table_reference  */
#line 353 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2818 "parser.cpp"
        break;

        case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 353 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2827 "parser.cpp"
        break;

        case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 353 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2836 "parser.cpp"
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
#line 2849 "parser.cpp"
        break;

        case YYSYMBOL_table_alias: /* table_alias  */
#line 348 "parser.y"
        {
            fprintf(stderr, "destroy table alias\n");
            delete (((*yyvaluep).table_alias_t));
        }
#line 2858 "parser.cpp"
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
#line 2872 "parser.cpp"
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
#line 2886 "parser.cpp"
        break;

        case YYSYMBOL_with_expr: /* with_expr  */
#line 364 "parser.y"
        {
            fprintf(stderr, "destroy with expr\n");
            delete ((*yyvaluep).with_expr_t)->select_;
            delete ((*yyvaluep).with_expr_t);
        }
#line 2896 "parser.cpp"
        break;

        case YYSYMBOL_join_clause: /* join_clause  */
#line 353 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2905 "parser.cpp"
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
#line 2919 "parser.cpp"
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
#line 2933 "parser.cpp"
        break;

        case YYSYMBOL_expr_alias: /* expr_alias  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2941 "parser.cpp"
        break;

        case YYSYMBOL_expr: /* expr  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2949 "parser.cpp"
        break;

        case YYSYMBOL_operand: /* operand  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2957 "parser.cpp"
        break;

        case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2965 "parser.cpp"
        break;

        case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2973 "parser.cpp"
        break;

        case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2981 "parser.cpp"
        break;

        case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2989 "parser.cpp"
        break;

        case YYSYMBOL_query_expr: /* query_expr  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2997 "parser.cpp"
        break;

        case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3005 "parser.cpp"
        break;

        case YYSYMBOL_sub_search: /* sub_search  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3013 "parser.cpp"
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
#line 3027 "parser.cpp"
        break;

        case YYSYMBOL_function_expr: /* function_expr  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3035 "parser.cpp"
        break;

        case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3043 "parser.cpp"
        break;

        case YYSYMBOL_between_expr: /* between_expr  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3051 "parser.cpp"
        break;

        case YYSYMBOL_in_expr: /* in_expr  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3059 "parser.cpp"
        break;

        case YYSYMBOL_case_expr: /* case_expr  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3067 "parser.cpp"
        break;

        case YYSYMBOL_case_check_array: /* case_check_array  */
#line 376 "parser.y"
        {
            fprintf(stderr, "destroy case check array\n");
            if (((*yyvaluep).case_check_array_t) != nullptr) {
                for (auto ptr : *(((*yyvaluep).case_check_array_t))) {
                    delete ptr;
                }
            }
        }
#line 3080 "parser.cpp"
        break;

        case YYSYMBOL_cast_expr: /* cast_expr  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3088 "parser.cpp"
        break;

        case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3096 "parser.cpp"
        break;

        case YYSYMBOL_column_expr: /* column_expr  */
#line 340 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3104 "parser.cpp"
        break;

        case YYSYMBOL_constant_expr: /* constant_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3112 "parser.cpp"
        break;

        case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3120 "parser.cpp"
        break;

        case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3128 "parser.cpp"
        break;

        case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3136 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3144 "parser.cpp"
        break;

        case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3152 "parser.cpp"
        break;

        case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3160 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3168 "parser.cpp"
        break;

        case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3176 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3184 "parser.cpp"
        break;

        case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3192 "parser.cpp"
        break;

        case YYSYMBOL_curly_brackets_expr: /* curly_brackets_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3200 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_curly_brackets_expr: /* unclosed_curly_brackets_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3208 "parser.cpp"
        break;

        case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 385 "parser.y"
        {
            delete (((*yyvaluep).int_sparse_ele_t));
        }
#line 3216 "parser.cpp"
        break;

        case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 389 "parser.y"
        {
            delete (((*yyvaluep).float_sparse_ele_t));
        }
#line 3224 "parser.cpp"
        break;

        case YYSYMBOL_array_expr: /* array_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3232 "parser.cpp"
        break;

        case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3240 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3248 "parser.cpp"
        break;

        case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3256 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3264 "parser.cpp"
        break;

        case YYSYMBOL_interval_expr: /* interval_expr  */
#line 344 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3272 "parser.cpp"
        break;

        case YYSYMBOL_file_path: /* file_path  */
#line 331 "parser.y"
        {
            free(((*yyvaluep).str_value));
        }
#line 3280 "parser.cpp"
        break;

        case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 221 "parser.y"
        {
            fprintf(stderr, "destroy if not exists info\n");
            if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
                delete (((*yyvaluep).if_not_exists_info_t));
            }
        }
#line 3291 "parser.cpp"
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
#line 3305 "parser.cpp"
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
#line 3319 "parser.cpp"
        break;

        case YYSYMBOL_index_info: /* index_info  */
#line 197 "parser.y"
        {
            fprintf(stderr, "destroy index info\n");
            if ((((*yyvaluep).index_info_t)) != nullptr) {
                delete (((*yyvaluep).index_info_t));
            }
        }
#line 3330 "parser.cpp"
        break;

        default:
            break;
    }
    YY_IGNORE_MAYBE_UNINITIALIZED_END
}

/*----------.
| yyparse.  |
`----------*/

int yyparse(void *scanner, infinity::ParserResult *result) {
    /* Lookahead token kind.  */
    int yychar;

    /* The semantic value of the lookahead symbol.  */
    /* Default value used for initialization, for pacifying older GCCs
       or non-GCC compilers.  */
    YY_INITIAL_VALUE(static YYSTYPE yyval_default;)
    YYSTYPE yylval YY_INITIAL_VALUE(= yyval_default);

    /* Location data for the lookahead symbol.  */
    static YYLTYPE yyloc_default
#if defined SQLLTYPE_IS_TRIVIAL && SQLLTYPE_IS_TRIVIAL
        = {1, 1, 1, 1}
#endif
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

#define YYPOPSTACK(N) (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

    /* The number of symbols on the RHS of the reduced rule.
       Keep to zero when no symbol should be popped.  */
    int yylen = 0;

    YYDPRINTF((stderr, "Starting parse\n"));

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

#line 3438 "parser.cpp"

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
    YYDPRINTF((stderr, "Entering state %d\n", yystate));
    YY_ASSERT(0 <= yystate && yystate < YYNSTATES);
    YY_IGNORE_USELESS_CAST_BEGIN
    *yyssp = YY_CAST(yy_state_t, yystate);
    YY_IGNORE_USELESS_CAST_END
    YY_STACK_PRINT(yyss, yyssp);

    if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
        YYNOMEM;
#else
    {
        /* Get the current used size of the three stacks, in elements.  */
        YYPTRDIFF_T yysize = yyssp - yyss + 1;

#if defined yyoverflow
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
            yyoverflow(YY_("memory exhausted"),
                       &yyss1,
                       yysize * YYSIZEOF(*yyssp),
                       &yyvs1,
                       yysize * YYSIZEOF(*yyvsp),
                       &yyls1,
                       yysize * YYSIZEOF(*yylsp),
                       &yystacksize);
            yyss = yyss1;
            yyvs = yyvs1;
            yyls = yyls1;
        }
#else /* defined YYSTACK_RELOCATE */
        /* Extend the stack our own way.  */
        if (YYMAXDEPTH <= yystacksize)
            YYNOMEM;
        yystacksize *= 2;
        if (YYMAXDEPTH < yystacksize)
            yystacksize = YYMAXDEPTH;

        {
            yy_state_t *yyss1 = yyss;
            union yyalloc *yyptr = YY_CAST(union yyalloc *, YYSTACK_ALLOC(YY_CAST(YYSIZE_T, YYSTACK_BYTES(yystacksize))));
            if (!yyptr)
                YYNOMEM;
            YYSTACK_RELOCATE(yyss_alloc, yyss);
            YYSTACK_RELOCATE(yyvs_alloc, yyvs);
            YYSTACK_RELOCATE(yyls_alloc, yyls);
#undef YYSTACK_RELOCATE
            if (yyss1 != yyssa)
                YYSTACK_FREE(yyss1);
        }
#endif

        yyssp = yyss + yysize - 1;
        yyvsp = yyvs + yysize - 1;
        yylsp = yyls + yysize - 1;

        YY_IGNORE_USELESS_CAST_BEGIN
        YYDPRINTF((stderr, "Stack size increased to %ld\n", YY_CAST(long, yystacksize)));
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
    if (yypact_value_is_default(yyn))
        goto yydefault;

    /* Not known => get a lookahead token if don't already have one.  */

    /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
    if (yychar == SQLEMPTY) {
        YYDPRINTF((stderr, "Reading a token\n"));
        yychar = yylex(&yylval, &yylloc, scanner);
    }

    if (yychar <= SQLEOF) {
        yychar = SQLEOF;
        yytoken = YYSYMBOL_YYEOF;
        YYDPRINTF((stderr, "Now at end of input.\n"));
    } else if (yychar == SQLerror) {
        /* The scanner already issued an error message, process directly
           to error recovery.  But do not keep the error token as
           lookahead, it is too special and may lead us to an endless
           loop in error recovery. */
        yychar = SQLUNDEF;
        yytoken = YYSYMBOL_YYerror;
        yyerror_range[1] = yylloc;
        goto yyerrlab1;
    } else {
        yytoken = YYTRANSLATE(yychar);
        YY_SYMBOL_PRINT("Next token is", yytoken, &yylval, &yylloc);
    }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
        goto yydefault;
    yyn = yytable[yyn];
    if (yyn <= 0) {
        if (yytable_value_is_error(yyn))
            goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
    }

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus)
        yyerrstatus--;

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT("Shifting", yytoken, &yylval, &yylloc);
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
    yyval = yyvsp[1 - yylen];

    /* Default location. */
    YYLLOC_DEFAULT(yyloc, (yylsp - yylen), yylen);
    yyerror_range[1] = yyloc;
    YY_REDUCE_PRINT(yyn);
    switch (yyn) {
        case 2: /* input_pattern: statement_list semicolon  */
#line 516 "parser.y"
        {
            result->statements_ptr_ = (yyvsp[-1].stmt_array);
        }
#line 3653 "parser.cpp"
        break;

        case 3: /* statement_list: statement  */
#line 520 "parser.y"
        {
            (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
            yylloc.string_length = 0;
            (yyval.stmt_array) = new std::vector<infinity::BaseStatement *>();
            (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
        }
#line 3664 "parser.cpp"
        break;

        case 4: /* statement_list: statement_list ';' statement  */
#line 526 "parser.y"
        {
            (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
            yylloc.string_length = 0;
            (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
            (yyval.stmt_array) = (yyvsp[-2].stmt_array);
        }
#line 3675 "parser.cpp"
        break;

        case 5: /* statement: create_statement  */
#line 533 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].create_stmt);
        }
#line 3681 "parser.cpp"
        break;

        case 6: /* statement: drop_statement  */
#line 534 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].drop_stmt);
        }
#line 3687 "parser.cpp"
        break;

        case 7: /* statement: copy_statement  */
#line 535 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].copy_stmt);
        }
#line 3693 "parser.cpp"
        break;

        case 8: /* statement: show_statement  */
#line 536 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].show_stmt);
        }
#line 3699 "parser.cpp"
        break;

        case 9: /* statement: select_statement  */
#line 537 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].select_stmt);
        }
#line 3705 "parser.cpp"
        break;

        case 10: /* statement: delete_statement  */
#line 538 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].delete_stmt);
        }
#line 3711 "parser.cpp"
        break;

        case 11: /* statement: update_statement  */
#line 539 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].update_stmt);
        }
#line 3717 "parser.cpp"
        break;

        case 12: /* statement: insert_statement  */
#line 540 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].insert_stmt);
        }
#line 3723 "parser.cpp"
        break;

        case 13: /* statement: explain_statement  */
#line 541 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].explain_stmt);
        }
#line 3729 "parser.cpp"
        break;

        case 14: /* statement: flush_statement  */
#line 542 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].flush_stmt);
        }
#line 3735 "parser.cpp"
        break;

        case 15: /* statement: optimize_statement  */
#line 543 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].optimize_stmt);
        }
#line 3741 "parser.cpp"
        break;

        case 16: /* statement: command_statement  */
#line 544 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].command_stmt);
        }
#line 3747 "parser.cpp"
        break;

        case 17: /* statement: compact_statement  */
#line 545 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].compact_stmt);
        }
#line 3753 "parser.cpp"
        break;

        case 18: /* statement: admin_statement  */
#line 546 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].admin_stmt);
        }
#line 3759 "parser.cpp"
        break;

        case 19: /* statement: alter_statement  */
#line 547 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].alter_stmt);
        }
#line 3765 "parser.cpp"
        break;

        case 20: /* statement: check_statement  */
#line 548 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].check_stmt);
        }
#line 3771 "parser.cpp"
        break;

        case 21: /* explainable_statement: create_statement  */
#line 550 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].create_stmt);
        }
#line 3777 "parser.cpp"
        break;

        case 22: /* explainable_statement: drop_statement  */
#line 551 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].drop_stmt);
        }
#line 3783 "parser.cpp"
        break;

        case 23: /* explainable_statement: copy_statement  */
#line 552 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].copy_stmt);
        }
#line 3789 "parser.cpp"
        break;

        case 24: /* explainable_statement: show_statement  */
#line 553 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].show_stmt);
        }
#line 3795 "parser.cpp"
        break;

        case 25: /* explainable_statement: select_statement  */
#line 554 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].select_stmt);
        }
#line 3801 "parser.cpp"
        break;

        case 26: /* explainable_statement: delete_statement  */
#line 555 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].delete_stmt);
        }
#line 3807 "parser.cpp"
        break;

        case 27: /* explainable_statement: update_statement  */
#line 556 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].update_stmt);
        }
#line 3813 "parser.cpp"
        break;

        case 28: /* explainable_statement: insert_statement  */
#line 557 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].insert_stmt);
        }
#line 3819 "parser.cpp"
        break;

        case 29: /* explainable_statement: flush_statement  */
#line 558 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].flush_stmt);
        }
#line 3825 "parser.cpp"
        break;

        case 30: /* explainable_statement: optimize_statement  */
#line 559 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].optimize_stmt);
        }
#line 3831 "parser.cpp"
        break;

        case 31: /* explainable_statement: command_statement  */
#line 560 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].command_stmt);
        }
#line 3837 "parser.cpp"
        break;

        case 32: /* explainable_statement: compact_statement  */
#line 561 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].compact_stmt);
        }
#line 3843 "parser.cpp"
        break;

        case 33: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER COMMENT STRING  */
#line 568 "parser.y"
        {
            (yyval.create_stmt) = new infinity::CreateStatement();
            std::shared_ptr<infinity::CreateSchemaInfo> create_schema_info = std::make_shared<infinity::CreateSchemaInfo>();

            ParserHelper::ToLower((yyvsp[-2].str_value));
            create_schema_info->schema_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));
            if (create_schema_info->schema_name_.empty()) {
                yyerror(&yyloc, scanner, result, "Empty database name is given.");
                YYERROR;
            }

            (yyval.create_stmt)->create_info_ = create_schema_info;
            (yyval.create_stmt)->create_info_->conflict_type_ =
                (yyvsp[-3].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
            (yyval.create_stmt)->create_info_->comment_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 3865 "parser.cpp"
        break;

        case 34: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 585 "parser.y"
        {
            (yyval.create_stmt) = new infinity::CreateStatement();
            std::shared_ptr<infinity::CreateSchemaInfo> create_schema_info = std::make_shared<infinity::CreateSchemaInfo>();

            ParserHelper::ToLower((yyvsp[0].str_value));
            create_schema_info->schema_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
            if (create_schema_info->schema_name_.empty()) {
                yyerror(&yyloc, scanner, result, "Empty database name is given.");
                YYERROR;
            }

            (yyval.create_stmt)->create_info_ = create_schema_info;
            (yyval.create_stmt)->create_info_->conflict_type_ =
                (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
        }
#line 3885 "parser.cpp"
        break;

        case 35: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 602 "parser.y"
        {
            (yyval.create_stmt) = new infinity::CreateStatement();
            std::shared_ptr<infinity::CreateCollectionInfo> create_collection_info = std::make_shared<infinity::CreateCollectionInfo>();
            if ((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
                create_collection_info->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
                free((yyvsp[0].table_name_t)->schema_name_ptr_);
            }
            create_collection_info->collection_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
            free((yyvsp[0].table_name_t)->table_name_ptr_);
            (yyval.create_stmt)->create_info_ = create_collection_info;
            (yyval.create_stmt)->create_info_->conflict_type_ =
                (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
            delete (yyvsp[0].table_name_t);
        }
#line 3903 "parser.cpp"
        break;

        case 36: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 618 "parser.y"
        {
            (yyval.create_stmt) = new infinity::CreateStatement();
            std::shared_ptr<infinity::CreateTableInfo> create_table_info = std::make_shared<infinity::CreateTableInfo>();
            if ((yyvsp[-4].table_name_t)->schema_name_ptr_ != nullptr) {
                create_table_info->schema_name_ = (yyvsp[-4].table_name_t)->schema_name_ptr_;
                free((yyvsp[-4].table_name_t)->schema_name_ptr_);
            }
            create_table_info->table_name_ = (yyvsp[-4].table_name_t)->table_name_ptr_;
            free((yyvsp[-4].table_name_t)->table_name_ptr_);
            delete (yyvsp[-4].table_name_t);

            for (infinity::TableElement *&element : *(yyvsp[-2].table_element_array_t)) {
                if (element->type_ == infinity::TableElementType::kColumn) {
                    create_table_info->column_defs_.emplace_back((infinity::ColumnDef *)element);
                } else {
                    create_table_info->constraints_.emplace_back((infinity::TableConstraint *)element);
                }
            }
            delete (yyvsp[-2].table_element_array_t);

            if ((yyvsp[0].with_index_param_list_t) != nullptr) {
                create_table_info->properties_ = std::move(*(yyvsp[0].with_index_param_list_t));
                delete (yyvsp[0].with_index_param_list_t);
            }

            (yyval.create_stmt)->create_info_ = create_table_info;
            (yyval.create_stmt)->create_info_->conflict_type_ =
                (yyvsp[-5].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
        }
#line 3936 "parser.cpp"
        break;

        case 37: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 647 "parser.y"
        {
            (yyval.create_stmt) = new infinity::CreateStatement();
            std::shared_ptr<infinity::CreateTableInfo> create_table_info = std::make_shared<infinity::CreateTableInfo>();
            if ((yyvsp[-2].table_name_t)->schema_name_ptr_ != nullptr) {
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
#line 3956 "parser.cpp"
        break;

        case 38: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list COMMENT STRING
                  */
#line 662 "parser.y"
        {
            (yyval.create_stmt) = new infinity::CreateStatement();
            std::shared_ptr<infinity::CreateTableInfo> create_table_info = std::make_shared<infinity::CreateTableInfo>();
            if ((yyvsp[-6].table_name_t)->schema_name_ptr_ != nullptr) {
                create_table_info->schema_name_ = (yyvsp[-6].table_name_t)->schema_name_ptr_;
                free((yyvsp[-6].table_name_t)->schema_name_ptr_);
            }
            create_table_info->table_name_ = (yyvsp[-6].table_name_t)->table_name_ptr_;
            free((yyvsp[-6].table_name_t)->table_name_ptr_);
            delete (yyvsp[-6].table_name_t);

            for (infinity::TableElement *&element : *(yyvsp[-4].table_element_array_t)) {
                if (element->type_ == infinity::TableElementType::kColumn) {
                    create_table_info->column_defs_.emplace_back((infinity::ColumnDef *)element);
                } else {
                    create_table_info->constraints_.emplace_back((infinity::TableConstraint *)element);
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
            (yyval.create_stmt)->create_info_->conflict_type_ =
                (yyvsp[-7].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
        }
#line 3992 "parser.cpp"
        break;

        case 39: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement COMMENT STRING  */
#line 694 "parser.y"
        {
            (yyval.create_stmt) = new infinity::CreateStatement();
            std::shared_ptr<infinity::CreateTableInfo> create_table_info = std::make_shared<infinity::CreateTableInfo>();
            if ((yyvsp[-4].table_name_t)->schema_name_ptr_ != nullptr) {
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
#line 4014 "parser.cpp"
        break;

        case 40: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 712 "parser.y"
        {
            (yyval.create_stmt) = new infinity::CreateStatement();
            std::shared_ptr<infinity::CreateViewInfo> create_view_info = std::make_shared<infinity::CreateViewInfo>();
            if ((yyvsp[-3].table_name_t)->schema_name_ptr_ != nullptr) {
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
#line 4035 "parser.cpp"
        break;

        case 41: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info  */
#line 730 "parser.y"
        {
            std::shared_ptr<infinity::CreateIndexInfo> create_index_info = std::make_shared<infinity::CreateIndexInfo>();
            if ((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
                create_index_info->schema_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
                free((yyvsp[-1].table_name_t)->schema_name_ptr_);
            }
            create_index_info->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
            free((yyvsp[-1].table_name_t)->table_name_ptr_);
            delete (yyvsp[-1].table_name_t);

            create_index_info->index_name_ = (yyvsp[-3].if_not_exists_info_t)->info_;
            if ((yyvsp[-3].if_not_exists_info_t)->exists_) {
                create_index_info->conflict_type_ =
                    (yyvsp[-3].if_not_exists_info_t)->if_not_exists_ ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
            } else {
                create_index_info->conflict_type_ = infinity::ConflictType::kIgnore;
            }
            delete (yyvsp[-3].if_not_exists_info_t);

            create_index_info->index_info_ = (yyvsp[0].index_info_t);

            if (create_index_info->index_name_.empty()) {
                yyerror(&yyloc, scanner, result, "No index name");
                YYERROR;
            }

            (yyval.create_stmt) = new infinity::CreateStatement();
            (yyval.create_stmt)->create_info_ = create_index_info;
        }
#line 4068 "parser.cpp"
        break;

        case 42: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info COMMENT STRING  */
#line 758 "parser.y"
        {
            std::shared_ptr<infinity::CreateIndexInfo> create_index_info = std::make_shared<infinity::CreateIndexInfo>();
            if ((yyvsp[-3].table_name_t)->schema_name_ptr_ != nullptr) {
                create_index_info->schema_name_ = (yyvsp[-3].table_name_t)->schema_name_ptr_;
                free((yyvsp[-3].table_name_t)->schema_name_ptr_);
            }
            create_index_info->table_name_ = (yyvsp[-3].table_name_t)->table_name_ptr_;
            free((yyvsp[-3].table_name_t)->table_name_ptr_);
            delete (yyvsp[-3].table_name_t);

            create_index_info->index_name_ = (yyvsp[-5].if_not_exists_info_t)->info_;
            if ((yyvsp[-5].if_not_exists_info_t)->exists_) {
                create_index_info->conflict_type_ =
                    (yyvsp[-5].if_not_exists_info_t)->if_not_exists_ ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
            } else {
                create_index_info->conflict_type_ = infinity::ConflictType::kIgnore;
            }
            delete (yyvsp[-5].if_not_exists_info_t);

            create_index_info->index_info_ = (yyvsp[-2].index_info_t);
            create_index_info->comment_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));

            if (create_index_info->index_name_.empty()) {
                yyerror(&yyloc, scanner, result, "No index name");
                YYERROR;
            }

            (yyval.create_stmt) = new infinity::CreateStatement();
            (yyval.create_stmt)->create_info_ = create_index_info;
        }
#line 4103 "parser.cpp"
        break;

        case 43: /* table_element_array: table_element  */
#line 789 "parser.y"
        {
            (yyval.table_element_array_t) = new std::vector<infinity::TableElement *>();
            (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
        }
#line 4112 "parser.cpp"
        break;

        case 44: /* table_element_array: table_element_array ',' table_element  */
#line 793 "parser.y"
        {
            (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
            (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
        }
#line 4121 "parser.cpp"
        break;

        case 45: /* column_def_array: table_column  */
#line 798 "parser.y"
        {
            (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef *>();
            (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
        }
#line 4130 "parser.cpp"
        break;

        case 46: /* column_def_array: column_def_array ',' table_column  */
#line 802 "parser.y"
        {
            (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
            (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
        }
#line 4139 "parser.cpp"
        break;

        case 47: /* table_element: table_column  */
#line 808 "parser.y"
        {
            (yyval.table_element_t) = (yyvsp[0].table_column_t);
        }
#line 4147 "parser.cpp"
        break;

        case 48: /* table_element: table_constraint  */
#line 811 "parser.y"
        {
            (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
        }
#line 4155 "parser.cpp"
        break;

        case 49: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 818 "parser.y"
        {
            std::shared_ptr<infinity::ParsedExpr> default_expr((yyvsp[0].const_expr_t));
            std::vector<std::unique_ptr<infinity::InitParameter>> index_param_list =
                infinity::InitParameter::MakeInitParameterList((yyvsp[-1].with_index_param_list_t));
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
#line 4180 "parser.cpp"
        break;

        case 50: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 838 "parser.y"
        {
            std::shared_ptr<infinity::ParsedExpr> default_expr((yyvsp[0].const_expr_t));
            auto [data_type_result, fail_reason] =
                infinity::ColumnType::GetDataTypeFromColumnType(*((yyvsp[-2].column_type_t)),
                                                                std::vector<std::unique_ptr<infinity::InitParameter>>{});
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
#line 4207 "parser.cpp"
        break;

        case 51: /* table_column: IDENTIFIER column_type with_index_param_list default_expr COMMENT STRING  */
#line 860 "parser.y"
        {
            std::shared_ptr<infinity::ParsedExpr> default_expr((yyvsp[-2].const_expr_t));
            std::vector<std::unique_ptr<infinity::InitParameter>> index_param_list =
                infinity::InitParameter::MakeInitParameterList((yyvsp[-3].with_index_param_list_t));
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
#line 4236 "parser.cpp"
        break;

        case 52: /* table_column: IDENTIFIER column_type column_constraints default_expr COMMENT STRING  */
#line 884 "parser.y"
        {
            std::shared_ptr<infinity::ParsedExpr> default_expr((yyvsp[-2].const_expr_t));
            auto [data_type_result, fail_reason] =
                infinity::ColumnType::GetDataTypeFromColumnType(*((yyvsp[-4].column_type_t)),
                                                                std::vector<std::unique_ptr<infinity::InitParameter>>{});
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
#line 4266 "parser.cpp"
        break;

        case 53: /* column_type_array: column_type  */
#line 910 "parser.y"
        {
            (yyval.column_type_array_t) = new std::vector<std::unique_ptr<infinity::ColumnType>>();
            (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
        }
#line 4275 "parser.cpp"
        break;

        case 54: /* column_type_array: column_type_array ',' column_type  */
#line 914 "parser.y"
        {
            (yyval.column_type_array_t) = (yyvsp[-2].column_type_array_t);
            (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
        }
#line 4284 "parser.cpp"
        break;

        case 55: /* column_type: BOOLEAN  */
#line 920 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4290 "parser.cpp"
        break;

        case 56: /* column_type: JSON  */
#line 921 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kJson, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4296 "parser.cpp"
        break;

        case 57: /* column_type: TINYINT  */
#line 922 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4302 "parser.cpp"
        break;

        case 58: /* column_type: SMALLINT  */
#line 923 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4308 "parser.cpp"
        break;

        case 59: /* column_type: INTEGER  */
#line 924 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4314 "parser.cpp"
        break;

        case 60: /* column_type: INT  */
#line 925 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4320 "parser.cpp"
        break;

        case 61: /* column_type: BIGINT  */
#line 926 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4326 "parser.cpp"
        break;

        case 62: /* column_type: HUGEINT  */
#line 927 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4332 "parser.cpp"
        break;

        case 63: /* column_type: FLOAT  */
#line 928 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4338 "parser.cpp"
        break;

        case 64: /* column_type: REAL  */
#line 929 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4344 "parser.cpp"
        break;

        case 65: /* column_type: DOUBLE  */
#line 930 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4350 "parser.cpp"
        break;

        case 66: /* column_type: FLOAT16  */
#line 931 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4356 "parser.cpp"
        break;

        case 67: /* column_type: BFLOAT16  */
#line 932 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4362 "parser.cpp"
        break;

        case 68: /* column_type: DATE  */
#line 933 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4368 "parser.cpp"
        break;

        case 69: /* column_type: TIME  */
#line 934 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4374 "parser.cpp"
        break;

        case 70: /* column_type: DATETIME  */
#line 935 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4380 "parser.cpp"
        break;

        case 71: /* column_type: TIMESTAMP  */
#line 936 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4386 "parser.cpp"
        break;

        case 72: /* column_type: UUID  */
#line 937 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4392 "parser.cpp"
        break;

        case 73: /* column_type: POINT  */
#line 938 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4398 "parser.cpp"
        break;

        case 74: /* column_type: LINE  */
#line 939 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4404 "parser.cpp"
        break;

        case 75: /* column_type: LSEG  */
#line 940 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4410 "parser.cpp"
        break;

        case 76: /* column_type: BOX  */
#line 941 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4416 "parser.cpp"
        break;

        case 77: /* column_type: CIRCLE  */
#line 944 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4422 "parser.cpp"
        break;

        case 78: /* column_type: VARCHAR  */
#line 946 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4428 "parser.cpp"
        break;

        case 79: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 947 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal,
                                                             0,
                                                             (yyvsp[-3].long_value),
                                                             (yyvsp[-1].long_value),
                                                             infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4434 "parser.cpp"
        break;

        case 80: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 948 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4440 "parser.cpp"
        break;

        case 81: /* column_type: DECIMAL  */
#line 949 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4446 "parser.cpp"
        break;

        case 82: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 952 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4452 "parser.cpp"
        break;

        case 83: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 953 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4458 "parser.cpp"
        break;

        case 84: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 954 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4464 "parser.cpp"
        break;

        case 85: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 955 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4470 "parser.cpp"
        break;

        case 86: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 956 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4476 "parser.cpp"
        break;

        case 87: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 957 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4482 "parser.cpp"
        break;

        case 88: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 958 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4488 "parser.cpp"
        break;

        case 89: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 959 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4494 "parser.cpp"
        break;

        case 90: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 960 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4500 "parser.cpp"
        break;

        case 91: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 961 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4506 "parser.cpp"
        break;

        case 92: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 962 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4512 "parser.cpp"
        break;

        case 93: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 963 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4518 "parser.cpp"
        break;

        case 94: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 964 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4524 "parser.cpp"
        break;

        case 95: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 965 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4530 "parser.cpp"
        break;

        case 96: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 966 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4536 "parser.cpp"
        break;

        case 97: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 967 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4542 "parser.cpp"
        break;

        case 98: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 968 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4548 "parser.cpp"
        break;

        case 99: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 969 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4554 "parser.cpp"
        break;

        case 100: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 970 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4560 "parser.cpp"
        break;

        case 101: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 971 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector,
                                                             (yyvsp[-1].long_value),
                                                             0,
                                                             0,
                                                             infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4566 "parser.cpp"
        break;

        case 102: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 972 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector,
                                                             (yyvsp[-1].long_value),
                                                             0,
                                                             0,
                                                             infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4572 "parser.cpp"
        break;

        case 103: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 973 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4578 "parser.cpp"
        break;

        case 104: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 974 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4584 "parser.cpp"
        break;

        case 105: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 975 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4590 "parser.cpp"
        break;

        case 106: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 976 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4596 "parser.cpp"
        break;

        case 107: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 977 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4602 "parser.cpp"
        break;

        case 108: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 978 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4608 "parser.cpp"
        break;

        case 109: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 979 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4614 "parser.cpp"
        break;

        case 110: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 980 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4620 "parser.cpp"
        break;

        case 111: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 981 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4626 "parser.cpp"
        break;

        case 112: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 982 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4632 "parser.cpp"
        break;

        case 113: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 983 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4638 "parser.cpp"
        break;

        case 114: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 984 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4644 "parser.cpp"
        break;

        case 115: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 985 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4650 "parser.cpp"
        break;

        case 116: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 986 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4656 "parser.cpp"
        break;

        case 117: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 987 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4662 "parser.cpp"
        break;

        case 118: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 988 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4668 "parser.cpp"
        break;

        case 119: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 989 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4674 "parser.cpp"
        break;

        case 120: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 990 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4680 "parser.cpp"
        break;

        case 121: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 991 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4686 "parser.cpp"
        break;

        case 122: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 992 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4692 "parser.cpp"
        break;

        case 123: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 993 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray,
                                                             (yyvsp[-1].long_value),
                                                             0,
                                                             0,
                                                             infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4698 "parser.cpp"
        break;

        case 124: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 994 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray,
                                                             (yyvsp[-1].long_value),
                                                             0,
                                                             0,
                                                             infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4704 "parser.cpp"
        break;

        case 125: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 995 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4710 "parser.cpp"
        break;

        case 126: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 996 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4716 "parser.cpp"
        break;

        case 127: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 997 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4722 "parser.cpp"
        break;

        case 128: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 998 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4728 "parser.cpp"
        break;

        case 129: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 999 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4734 "parser.cpp"
        break;

        case 130: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1000 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4740 "parser.cpp"
        break;

        case 131: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1001 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4746 "parser.cpp"
        break;

        case 132: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1002 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4752 "parser.cpp"
        break;

        case 133: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1003 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4758 "parser.cpp"
        break;

        case 134: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1004 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4764 "parser.cpp"
        break;

        case 135: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1005 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4770 "parser.cpp"
        break;

        case 136: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1006 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4776 "parser.cpp"
        break;

        case 137: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1007 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4782 "parser.cpp"
        break;

        case 138: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1008 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4788 "parser.cpp"
        break;

        case 139: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1009 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4794 "parser.cpp"
        break;

        case 140: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4800 "parser.cpp"
        break;

        case 141: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1011 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4806 "parser.cpp"
        break;

        case 142: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1012 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4812 "parser.cpp"
        break;

        case 143: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1013 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4818 "parser.cpp"
        break;

        case 144: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1014 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4824 "parser.cpp"
        break;

        case 145: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4830 "parser.cpp"
        break;

        case 146: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4836 "parser.cpp"
        break;

        case 147: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1017 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4842 "parser.cpp"
        break;

        case 148: /* column_type: ARRAY '(' column_type ')'  */
#line 1018 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kArray, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
            (yyval.column_type_t)->element_types_.emplace_back((yyvsp[-1].column_type_t));
        }
#line 4851 "parser.cpp"
        break;

        case 149: /* column_type: TUPLE '(' column_type_array ')'  */
#line 1022 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTuple, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
            (yyval.column_type_t)->element_types_ = std::move(*((yyvsp[-1].column_type_array_t)));
            delete (yyvsp[-1].column_type_array_t);
        }
#line 4861 "parser.cpp"
        break;

        case 150: /* column_constraints: column_constraint  */
#line 1045 "parser.y"
        {
            (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
            (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
        }
#line 4870 "parser.cpp"
        break;

        case 151: /* column_constraints: column_constraints column_constraint  */
#line 1049 "parser.y"
        {
            if ((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
                yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
                delete (yyvsp[-1].column_constraints_t);
                YYERROR;
            }
            (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
            (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
        }
#line 4884 "parser.cpp"
        break;

        case 152: /* column_constraint: PRIMARY KEY  */
#line 1059 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
        }
#line 4892 "parser.cpp"
        break;

        case 153: /* column_constraint: UNIQUE  */
#line 1062 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
        }
#line 4900 "parser.cpp"
        break;

        case 154: /* column_constraint: NULLABLE  */
#line 1065 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
        }
#line 4908 "parser.cpp"
        break;

        case 155: /* column_constraint: NOT NULLABLE  */
#line 1068 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
        }
#line 4916 "parser.cpp"
        break;

        case 156: /* default_expr: DEFAULT constant_expr  */
#line 1072 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 4924 "parser.cpp"
        break;

        case 157: /* default_expr: %empty  */
#line 1075 "parser.y"
        {
            (yyval.const_expr_t) = nullptr;
        }
#line 4932 "parser.cpp"
        break;

        case 158: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1080 "parser.y"
        {
            (yyval.table_constraint_t) = new infinity::TableConstraint();
            (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
            (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
        }
#line 4942 "parser.cpp"
        break;

        case 159: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1085 "parser.y"
        {
            (yyval.table_constraint_t) = new infinity::TableConstraint();
            (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
            (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
        }
#line 4952 "parser.cpp"
        break;

        case 160: /* identifier_array: IDENTIFIER  */
#line 1092 "parser.y"
        {
            (yyval.identifier_array_t) = new std::vector<std::string>();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 4963 "parser.cpp"
        break;

        case 161: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1098 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
            (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
        }
#line 4974 "parser.cpp"
        break;

        case 162: /* delete_statement: DELETE FROM table_name where_clause  */
#line 1108 "parser.y"
        {
            (yyval.delete_stmt) = new infinity::DeleteStatement();

            if ((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
                (yyval.delete_stmt)->schema_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
                free((yyvsp[-1].table_name_t)->schema_name_ptr_);
            }
            (yyval.delete_stmt)->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
            free((yyvsp[-1].table_name_t)->table_name_ptr_);
            delete (yyvsp[-1].table_name_t);
            (yyval.delete_stmt)->where_expr_ = (yyvsp[0].expr_t);
        }
#line 4991 "parser.cpp"
        break;

        case 163: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES insert_row_list  */
#line 1124 "parser.y"
        {
            bool is_error{false};
            for (auto expr_array : *(yyvsp[0].insert_row_list_t)) {
                for (const auto &expr : expr_array->values_) {
                    if (expr->type_ != infinity::ParsedExprType::kConstant) {
                        yyerror(&yyloc, scanner, result, ("Value list has non-constant expression: " + expr->ToString()).c_str());
                        is_error = true;
                    }
                }
            }
            if (is_error) {
                for (auto expr_array : *(yyvsp[0].insert_row_list_t)) {
                    delete (expr_array);
                }
                delete (yyvsp[0].insert_row_list_t);
                delete (yyvsp[-3].table_name_t);
                delete (yyvsp[-2].identifier_array_t);
                YYERROR;
            }

            (yyval.insert_stmt) = new infinity::InsertStatement();
            if ((yyvsp[-3].table_name_t)->schema_name_ptr_ != nullptr) {
                (yyval.insert_stmt)->schema_name_ = (yyvsp[-3].table_name_t)->schema_name_ptr_;
                free((yyvsp[-3].table_name_t)->schema_name_ptr_);
            }
            (yyval.insert_stmt)->table_name_ = (yyvsp[-3].table_name_t)->table_name_ptr_;
            free((yyvsp[-3].table_name_t)->table_name_ptr_);
            delete (yyvsp[-3].table_name_t);
            for (infinity::InsertRowExpr *&expr_ptr : *(yyvsp[0].insert_row_list_t)) {
                if ((yyvsp[-2].identifier_array_t)) {
                    expr_ptr->columns_ = *((yyvsp[-2].identifier_array_t));
                }
                (yyval.insert_stmt)->insert_rows_.emplace_back(expr_ptr);
                expr_ptr = nullptr;
            }
            delete (yyvsp[-2].identifier_array_t);
            delete (yyvsp[0].insert_row_list_t);
        }
#line 5034 "parser.cpp"
        break;

        case 164: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1162 "parser.y"
        {
            (yyval.insert_stmt) = new infinity::InsertStatement();
            if ((yyvsp[-2].table_name_t)->schema_name_ptr_ != nullptr) {
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
#line 5054 "parser.cpp"
        break;

        case 165: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1178 "parser.y"
        {
            (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
        }
#line 5062 "parser.cpp"
        break;

        case 166: /* optional_identifier_array: %empty  */
#line 1181 "parser.y"
        {
            (yyval.identifier_array_t) = nullptr;
        }
#line 5070 "parser.cpp"
        break;

        case 167: /* explain_statement: EXPLAIN IDENTIFIER explainable_statement  */
#line 1188 "parser.y"
        {
            (yyval.explain_stmt) = new infinity::ExplainStatement();
            ParserHelper::ToLower((yyvsp[-1].str_value));
            if (!strcmp((yyvsp[-1].str_value), "analyze"))
                (yyval.explain_stmt)->type_ = infinity::ExplainType::kAnalyze;
            else if (!strcmp((yyvsp[-1].str_value), "ast"))
                (yyval.explain_stmt)->type_ = infinity::ExplainType::kAst;
            else if (!strcmp((yyvsp[-1].str_value), "raw"))
                (yyval.explain_stmt)->type_ = infinity::ExplainType::kUnOpt;
            else if (!strcmp((yyvsp[-1].str_value), "logical"))
                (yyval.explain_stmt)->type_ = infinity::ExplainType::kOpt;
            else if (!strcmp((yyvsp[-1].str_value), "physical"))
                (yyval.explain_stmt)->type_ = infinity::ExplainType::kPhysical;
            else if (!strcmp((yyvsp[-1].str_value), "pipeline"))
                (yyval.explain_stmt)->type_ = infinity::ExplainType::kPipeline;
            else if (!strcmp((yyvsp[-1].str_value), "fragment"))
                (yyval.explain_stmt)->type_ = infinity::ExplainType::kFragment;
            else {
                free(yyvsp[-1].str_value);
                yyerror(&yyloc, scanner, result, "invalid explain option");
                YYERROR;
            }
            free((yyvsp[-1].str_value));
            (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
        }
#line 5093 "parser.cpp"
        break;

        case 168: /* explain_statement: EXPLAIN explainable_statement  */
#line 1205 "parser.y"
        {
            (yyval.explain_stmt) = new infinity::ExplainStatement();
            (yyval.explain_stmt)->type_ = infinity::ExplainType::kPhysical;
            (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
        }
#line 5103 "parser.cpp"
        break;

        case 169: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1214 "parser.y"
        {
            (yyval.update_stmt) = new infinity::UpdateStatement();
            if ((yyvsp[-3].table_name_t)->schema_name_ptr_ != nullptr) {
                (yyval.update_stmt)->schema_name_ = (yyvsp[-3].table_name_t)->schema_name_ptr_;
                free((yyvsp[-3].table_name_t)->schema_name_ptr_);
            }
            (yyval.update_stmt)->table_name_ = (yyvsp[-3].table_name_t)->table_name_ptr_;
            free((yyvsp[-3].table_name_t)->table_name_ptr_);
            delete (yyvsp[-3].table_name_t);
            (yyval.update_stmt)->where_expr_ = (yyvsp[0].expr_t);
            (yyval.update_stmt)->update_expr_array_ = (yyvsp[-1].update_expr_array_t);
        }
#line 5120 "parser.cpp"
        break;

        case 170: /* update_expr_array: update_expr  */
#line 1227 "parser.y"
        {
            (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr *>();
            (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
        }
#line 5129 "parser.cpp"
        break;

        case 171: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1231 "parser.y"
        {
            (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
            (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
        }
#line 5138 "parser.cpp"
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
#line 5150 "parser.cpp"
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
#line 5166 "parser.cpp"
        break;

        case 174: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1262 "parser.y"
        {
            (yyval.drop_stmt) = new infinity::DropStatement();
            std::shared_ptr<infinity::DropCollectionInfo> drop_collection_info = std::make_unique<infinity::DropCollectionInfo>();
            if ((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
                drop_collection_info->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
                free((yyvsp[0].table_name_t)->schema_name_ptr_);
            }
            drop_collection_info->collection_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
            free((yyvsp[0].table_name_t)->table_name_ptr_);
            (yyval.drop_stmt)->drop_info_ = drop_collection_info;
            (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
            delete (yyvsp[0].table_name_t);
        }
#line 5184 "parser.cpp"
        break;

        case 175: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1277 "parser.y"
        {
            (yyval.drop_stmt) = new infinity::DropStatement();
            std::shared_ptr<infinity::DropTableInfo> drop_table_info = std::make_unique<infinity::DropTableInfo>();
            if ((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
                drop_table_info->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
                free((yyvsp[0].table_name_t)->schema_name_ptr_);
            }
            drop_table_info->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
            free((yyvsp[0].table_name_t)->table_name_ptr_);
            (yyval.drop_stmt)->drop_info_ = drop_table_info;
            (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
            delete (yyvsp[0].table_name_t);
        }
#line 5202 "parser.cpp"
        break;

        case 176: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1292 "parser.y"
        {
            (yyval.drop_stmt) = new infinity::DropStatement();
            std::shared_ptr<infinity::DropViewInfo> drop_view_info = std::make_unique<infinity::DropViewInfo>();
            if ((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
                drop_view_info->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
                free((yyvsp[0].table_name_t)->schema_name_ptr_);
            }
            drop_view_info->view_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
            free((yyvsp[0].table_name_t)->table_name_ptr_);
            (yyval.drop_stmt)->drop_info_ = drop_view_info;
            (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
            delete (yyvsp[0].table_name_t);
        }
#line 5220 "parser.cpp"
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

            if ((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
                drop_index_info->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
                free((yyvsp[0].table_name_t)->schema_name_ptr_);
            }
            drop_index_info->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
            free((yyvsp[0].table_name_t)->table_name_ptr_);
            delete (yyvsp[0].table_name_t);
        }
#line 5243 "parser.cpp"
        break;

        case 178: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1330 "parser.y"
        {
            (yyval.copy_stmt) = new infinity::CopyStatement();

            // Copy To
            (yyval.copy_stmt)->copy_from_ = false;

            // table_name
            if ((yyvsp[-6].table_name_t)->schema_name_ptr_ != nullptr) {
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
            for (size_t idx = 0; idx < option_count; ++idx) {
                infinity::CopyOption *option_ptr = (*(yyvsp[-1].copy_option_array))[idx];
                switch (option_ptr->option_type_) {
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
#line 5301 "parser.cpp"
        break;

        case 179: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1383 "parser.y"
        {
            (yyval.copy_stmt) = new infinity::CopyStatement();

            // Copy To
            (yyval.copy_stmt)->copy_from_ = false;

            // table_name
            if ((yyvsp[-9].table_name_t)->schema_name_ptr_ != nullptr) {
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
            for (size_t idx = 0; idx < option_count; ++idx) {
                infinity::CopyOption *option_ptr = (*(yyvsp[-1].copy_option_array))[idx];
                switch (option_ptr->option_type_) {
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
#line 5361 "parser.cpp"
        break;

        case 180: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1438 "parser.y"
        {
            (yyval.copy_stmt) = new infinity::CopyStatement();

            // Copy From
            (yyval.copy_stmt)->copy_from_ = true;

            // table_name
            if ((yyvsp[-6].table_name_t)->schema_name_ptr_ != nullptr) {
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
            for (size_t idx = 0; idx < option_count; ++idx) {
                infinity::CopyOption *option_ptr = (*(yyvsp[-1].copy_option_array))[idx];
                switch (option_ptr->option_type_) {
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
#line 5413 "parser.cpp"
        break;

        case 181: /* select_statement: select_without_paren  */
#line 1489 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[0].select_stmt);
        }
#line 5421 "parser.cpp"
        break;

        case 182: /* select_statement: select_with_paren  */
#line 1492 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[0].select_stmt);
        }
#line 5429 "parser.cpp"
        break;

        case 183: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1495 "parser.y"
        {
            infinity::SelectStatement *node = (yyvsp[-2].select_stmt);
            while (node->nested_select_ != nullptr) {
                node = node->nested_select_;
            }
            node->set_op_ = (yyvsp[-1].set_operator_t);
            node->nested_select_ = (yyvsp[0].select_stmt);
            (yyval.select_stmt) = (yyvsp[-2].select_stmt);
        }
#line 5443 "parser.cpp"
        break;

        case 184: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1504 "parser.y"
        {
            infinity::SelectStatement *node = (yyvsp[-2].select_stmt);
            while (node->nested_select_ != nullptr) {
                node = node->nested_select_;
            }
            node->set_op_ = (yyvsp[-1].set_operator_t);
            node->nested_select_ = (yyvsp[0].select_stmt);
            (yyval.select_stmt) = (yyvsp[-2].select_stmt);
        }
#line 5457 "parser.cpp"
        break;

        case 185: /* select_with_paren: '(' select_without_paren ')'  */
#line 1514 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5465 "parser.cpp"
        break;

        case 186: /* select_with_paren: '(' select_with_paren ')'  */
#line 1517 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5473 "parser.cpp"
        break;

        case 187: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1521 "parser.y"
        {
            (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
            (yyval.select_stmt) = (yyvsp[0].select_stmt);
        }
#line 5482 "parser.cpp"
        break;

        case 188: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1526 "parser.y"
        {
            if ((yyvsp[-1].expr_t) == nullptr and (yyvsp[0].expr_t) != nullptr) {
                delete (yyvsp[-3].select_stmt);
                delete (yyvsp[-2].order_by_expr_list_t);
                delete (yyvsp[0].expr_t);
                yyerror(&yyloc, scanner, result, "Offset expression isn't valid without Limit expression");
                YYERROR;
            }
            if ((yyvsp[-3].select_stmt)->search_expr_ != nullptr and
                ((yyvsp[-2].order_by_expr_list_t) != nullptr /*or $3 != nullptr or $4 != nullptr*/)) {
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
#line 5513 "parser.cpp"
        break;

        case 189: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1553 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5521 "parser.cpp"
        break;

        case 190: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1556 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5529 "parser.cpp"
        break;

        case 191: /* select_clause_without_modifier: SELECT distinct expr_array highlight_clause from_clause search_clause where_clause
                     group_by_clause having_clause  */
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

            if ((yyval.select_stmt)->group_by_list_ == nullptr && (yyval.select_stmt)->having_expr_ != nullptr) {
                yyerror(&yyloc, scanner, result, "HAVING clause should follow after GROUP BY clause");
                YYERROR;
            }
        }
#line 5550 "parser.cpp"
        break;

        case 192: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1578 "parser.y"
        {
            (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
        }
#line 5558 "parser.cpp"
        break;

        case 193: /* order_by_clause: %empty  */
#line 1581 "parser.y"
        {
            (yyval.order_by_expr_list_t) = nullptr;
        }
#line 5566 "parser.cpp"
        break;

        case 194: /* order_by_expr_list: order_by_expr  */
#line 1585 "parser.y"
        {
            (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr *>();
            (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
        }
#line 5575 "parser.cpp"
        break;

        case 195: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1589 "parser.y"
        {
            (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
            (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
        }
#line 5584 "parser.cpp"
        break;

        case 196: /* order_by_expr: expr order_by_type  */
#line 1594 "parser.y"
        {
            (yyval.order_by_expr_t) = new infinity::OrderByExpr();
            (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
            (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
        }
#line 5594 "parser.cpp"
        break;

        case 197: /* order_by_type: ASC  */
#line 1600 "parser.y"
        {
            (yyval.order_by_type_t) = infinity::kAsc;
        }
#line 5602 "parser.cpp"
        break;

        case 198: /* order_by_type: DESC  */
#line 1603 "parser.y"
        {
            (yyval.order_by_type_t) = infinity::kDesc;
        }
#line 5610 "parser.cpp"
        break;

        case 199: /* order_by_type: %empty  */
#line 1606 "parser.y"
        {
            (yyval.order_by_type_t) = infinity::kAsc;
        }
#line 5618 "parser.cpp"
        break;

        case 200: /* limit_expr: LIMIT expr  */
#line 1610 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5626 "parser.cpp"
        break;

        case 201: /* limit_expr: %empty  */
#line 1614 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5632 "parser.cpp"
        break;

        case 202: /* offset_expr: OFFSET expr  */
#line 1616 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5640 "parser.cpp"
        break;

        case 203: /* offset_expr: %empty  */
#line 1620 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5646 "parser.cpp"
        break;

        case 204: /* distinct: DISTINCT  */
#line 1622 "parser.y"
        {
            (yyval.bool_value) = true;
        }
#line 5654 "parser.cpp"
        break;

        case 205: /* distinct: %empty  */
#line 1625 "parser.y"
        {
            (yyval.bool_value) = false;
        }
#line 5662 "parser.cpp"
        break;

        case 206: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1629 "parser.y"
        {
            (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
        }
#line 5670 "parser.cpp"
        break;

        case 207: /* highlight_clause: %empty  */
#line 1632 "parser.y"
        {
            (yyval.expr_array_t) = nullptr;
        }
#line 5678 "parser.cpp"
        break;

        case 208: /* from_clause: FROM table_reference  */
#line 1636 "parser.y"
        {
            (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
        }
#line 5686 "parser.cpp"
        break;

        case 209: /* from_clause: %empty  */
#line 1639 "parser.y"
        {
            (yyval.table_reference_t) = nullptr;
        }
#line 5694 "parser.cpp"
        break;

        case 210: /* search_clause: SEARCH sub_search_array  */
#line 1643 "parser.y"
        {
            infinity::SearchExpr *search_expr = new infinity::SearchExpr();
            search_expr->SetExprs((yyvsp[0].expr_array_t));
            (yyval.expr_t) = search_expr;
        }
#line 5704 "parser.cpp"
        break;

        case 211: /* search_clause: %empty  */
#line 1648 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5712 "parser.cpp"
        break;

        case 212: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1652 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5720 "parser.cpp"
        break;

        case 213: /* optional_search_filter_expr: %empty  */
#line 1655 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5728 "parser.cpp"
        break;

        case 214: /* where_clause: WHERE expr  */
#line 1659 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5736 "parser.cpp"
        break;

        case 215: /* where_clause: %empty  */
#line 1662 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5744 "parser.cpp"
        break;

        case 216: /* having_clause: HAVING expr  */
#line 1666 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5752 "parser.cpp"
        break;

        case 217: /* having_clause: %empty  */
#line 1669 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5760 "parser.cpp"
        break;

        case 218: /* group_by_clause: GROUP BY expr_array  */
#line 1673 "parser.y"
        {
            (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
        }
#line 5768 "parser.cpp"
        break;

        case 219: /* group_by_clause: %empty  */
#line 1676 "parser.y"
        {
            (yyval.expr_array_t) = nullptr;
        }
#line 5776 "parser.cpp"
        break;

        case 220: /* set_operator: UNION  */
#line 1680 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
        }
#line 5784 "parser.cpp"
        break;

        case 221: /* set_operator: UNION ALL  */
#line 1683 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
        }
#line 5792 "parser.cpp"
        break;

        case 222: /* set_operator: INTERSECT  */
#line 1686 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
        }
#line 5800 "parser.cpp"
        break;

        case 223: /* set_operator: EXCEPT  */
#line 1689 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
        }
#line 5808 "parser.cpp"
        break;

        case 224: /* table_reference: table_reference_unit  */
#line 1697 "parser.y"
        {
            (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
        }
#line 5816 "parser.cpp"
        break;

        case 225: /* table_reference: table_reference ',' table_reference_unit  */
#line 1700 "parser.y"
        {
            infinity::CrossProductReference *cross_product_ref = nullptr;
            if ((yyvsp[-2].table_reference_t)->type_ == infinity::TableRefType::kCrossProduct) {
                cross_product_ref = (infinity::CrossProductReference *)(yyvsp[-2].table_reference_t);
                cross_product_ref->tables_.emplace_back((yyvsp[0].table_reference_t));
            } else {
                cross_product_ref = new infinity::CrossProductReference();
                cross_product_ref->tables_.emplace_back((yyvsp[-2].table_reference_t));
                cross_product_ref->tables_.emplace_back((yyvsp[0].table_reference_t));
            }

            (yyval.table_reference_t) = cross_product_ref;
        }
#line 5834 "parser.cpp"
        break;

        case 228: /* table_reference_name: table_name table_alias  */
#line 1717 "parser.y"
        {
            infinity::TableReference *table_ref = new infinity::TableReference();
            if ((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
                table_ref->db_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
                free((yyvsp[-1].table_name_t)->schema_name_ptr_);
            }
            table_ref->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
            free((yyvsp[-1].table_name_t)->table_name_ptr_);
            delete (yyvsp[-1].table_name_t);

            table_ref->alias_ = (yyvsp[0].table_alias_t);
            (yyval.table_reference_t) = table_ref;
        }
#line 5852 "parser.cpp"
        break;

        case 229: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1731 "parser.y"
        {
            infinity::SubqueryReference *subquery_reference = new infinity::SubqueryReference();
            subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
            subquery_reference->alias_ = (yyvsp[0].table_alias_t);
            (yyval.table_reference_t) = subquery_reference;
        }
#line 5863 "parser.cpp"
        break;

        case 230: /* table_name: IDENTIFIER  */
#line 1740 "parser.y"
        {
            (yyval.table_name_t) = new infinity::TableName();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
        }
#line 5873 "parser.cpp"
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
#line 5885 "parser.cpp"
        break;

        case 232: /* table_alias: AS IDENTIFIER  */
#line 1754 "parser.y"
        {
            (yyval.table_alias_t) = new infinity::TableAlias();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
        }
#line 5895 "parser.cpp"
        break;

        case 233: /* table_alias: IDENTIFIER  */
#line 1759 "parser.y"
        {
            (yyval.table_alias_t) = new infinity::TableAlias();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
        }
#line 5905 "parser.cpp"
        break;

        case 234: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1764 "parser.y"
        {
            (yyval.table_alias_t) = new infinity::TableAlias();
            ParserHelper::ToLower((yyvsp[-3].str_value));
            (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
            (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
        }
#line 5916 "parser.cpp"
        break;

        case 235: /* table_alias: %empty  */
#line 1770 "parser.y"
        {
            (yyval.table_alias_t) = nullptr;
        }
#line 5924 "parser.cpp"
        break;

        case 236: /* with_clause: WITH with_expr_list  */
#line 1777 "parser.y"
        {
            (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
        }
#line 5932 "parser.cpp"
        break;

        case 237: /* with_clause: %empty  */
#line 1780 "parser.y"
        {
            (yyval.with_expr_list_t) = nullptr;
        }
#line 5940 "parser.cpp"
        break;

        case 238: /* with_expr_list: with_expr  */
#line 1784 "parser.y"
        {
            (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr *>();
            (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
        }
#line 5949 "parser.cpp"
        break;

        case 239: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1787 "parser.y"
        {
            (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
            (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
        }
#line 5958 "parser.cpp"
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
#line 5970 "parser.cpp"
        break;

        case 241: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1804 "parser.y"
        {
            infinity::JoinReference *join_reference = new infinity::JoinReference();
            join_reference->left_ = (yyvsp[-3].table_reference_t);
            join_reference->right_ = (yyvsp[0].table_reference_t);
            join_reference->join_type_ = infinity::JoinType::kNatural;
            (yyval.table_reference_t) = join_reference;
        }
#line 5982 "parser.cpp"
        break;

        case 242: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1811 "parser.y"
        {
            infinity::JoinReference *join_reference = new infinity::JoinReference();
            join_reference->left_ = (yyvsp[-5].table_reference_t);
            join_reference->right_ = (yyvsp[-2].table_reference_t);
            join_reference->join_type_ = (yyvsp[-4].join_type_t);
            join_reference->condition_ = (yyvsp[0].expr_t);
            (yyval.table_reference_t) = join_reference;
        }
#line 5995 "parser.cpp"
        break;

        case 243: /* join_type: INNER  */
#line 1825 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kInner;
        }
#line 6003 "parser.cpp"
        break;

        case 244: /* join_type: LEFT  */
#line 1828 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kLeft;
        }
#line 6011 "parser.cpp"
        break;

        case 245: /* join_type: RIGHT  */
#line 1831 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kRight;
        }
#line 6019 "parser.cpp"
        break;

        case 246: /* join_type: OUTER  */
#line 1834 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kFull;
        }
#line 6027 "parser.cpp"
        break;

        case 247: /* join_type: FULL  */
#line 1837 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kFull;
        }
#line 6035 "parser.cpp"
        break;

        case 248: /* join_type: CROSS  */
#line 1840 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kCross;
        }
#line 6043 "parser.cpp"
        break;

        case 249: /* join_type: %empty  */
#line 1843 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kInner;
        }
#line 6051 "parser.cpp"
        break;

        case 250: /* show_statement: SHOW DATABASES  */
#line 1850 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
        }
#line 6060 "parser.cpp"
        break;

        case 251: /* show_statement: SHOW TABLES  */
#line 1854 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
        }
#line 6069 "parser.cpp"
        break;

        case 252: /* show_statement: SHOW TASKS  */
#line 1858 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTasks;
        }
#line 6078 "parser.cpp"
        break;

        case 253: /* show_statement: SHOW CONFIGS  */
#line 1862 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
        }
#line 6087 "parser.cpp"
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
#line 6099 "parser.cpp"
        break;

        case 255: /* show_statement: SHOW PROFILES  */
#line 1873 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
        }
#line 6108 "parser.cpp"
        break;

        case 256: /* show_statement: SHOW BUFFER  */
#line 1877 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
        }
#line 6117 "parser.cpp"
        break;

        case 257: /* show_statement: SHOW MEMINDEX  */
#line 1881 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
        }
#line 6126 "parser.cpp"
        break;

        case 258: /* show_statement: SHOW QUERIES  */
#line 1885 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
        }
#line 6135 "parser.cpp"
        break;

        case 259: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1889 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
            (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
        }
#line 6145 "parser.cpp"
        break;

        case 260: /* show_statement: SHOW TRANSACTIONS  */
#line 1894 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
        }
#line 6154 "parser.cpp"
        break;

        case 261: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1898 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
            (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
        }
#line 6164 "parser.cpp"
        break;

        case 262: /* show_statement: SHOW TRANSACTION HISTORY  */
#line 1903 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactionHistory;
        }
#line 6173 "parser.cpp"
        break;

        case 263: /* show_statement: SHOW SESSION VARIABLES  */
#line 1907 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
        }
#line 6182 "parser.cpp"
        break;

        case 264: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1911 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
        }
#line 6191 "parser.cpp"
        break;

        case 265: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1915 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
            (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 6202 "parser.cpp"
        break;

        case 266: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1921 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
            (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 6213 "parser.cpp"
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
#line 6225 "parser.cpp"
        break;

        case 268: /* show_statement: SHOW TABLE table_name  */
#line 1934 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTable;
            if ((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
                (yyval.show_stmt)->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
                free((yyvsp[0].table_name_t)->schema_name_ptr_);
            }
            (yyval.show_stmt)->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
            free((yyvsp[0].table_name_t)->table_name_ptr_);
            delete (yyvsp[0].table_name_t);
        }
#line 6241 "parser.cpp"
        break;

        case 269: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1945 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kColumns;
            if ((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
                (yyval.show_stmt)->schema_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
                free((yyvsp[-1].table_name_t)->schema_name_ptr_);
            }
            (yyval.show_stmt)->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
            free((yyvsp[-1].table_name_t)->table_name_ptr_);
            delete (yyvsp[-1].table_name_t);
        }
#line 6257 "parser.cpp"
        break;

        case 270: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1956 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSegments;
            if ((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
                (yyval.show_stmt)->schema_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
                free((yyvsp[-1].table_name_t)->schema_name_ptr_);
            }
            (yyval.show_stmt)->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
            free((yyvsp[-1].table_name_t)->table_name_ptr_);
            delete (yyvsp[-1].table_name_t);
        }
#line 6273 "parser.cpp"
        break;

        case 271: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1967 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSegment;
            if ((yyvsp[-2].table_name_t)->schema_name_ptr_ != nullptr) {
                (yyval.show_stmt)->schema_name_ = (yyvsp[-2].table_name_t)->schema_name_ptr_;
                free((yyvsp[-2].table_name_t)->schema_name_ptr_);
            }
            (yyval.show_stmt)->table_name_ = (yyvsp[-2].table_name_t)->table_name_ptr_;
            free((yyvsp[-2].table_name_t)->table_name_ptr_);
            (yyval.show_stmt)->segment_id_ = (yyvsp[0].long_value);
            delete (yyvsp[-2].table_name_t);
        }
#line 6290 "parser.cpp"
        break;

        case 272: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1979 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBlocks;
            if ((yyvsp[-3].table_name_t)->schema_name_ptr_ != nullptr) {
                (yyval.show_stmt)->schema_name_ = (yyvsp[-3].table_name_t)->schema_name_ptr_;
                free((yyvsp[-3].table_name_t)->schema_name_ptr_);
            }
            (yyval.show_stmt)->table_name_ = (yyvsp[-3].table_name_t)->table_name_ptr_;
            free((yyvsp[-3].table_name_t)->table_name_ptr_);
            (yyval.show_stmt)->segment_id_ = (yyvsp[-1].long_value);
            delete (yyvsp[-3].table_name_t);
        }
#line 6307 "parser.cpp"
        break;

        case 273: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1991 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBlock;
            if ((yyvsp[-4].table_name_t)->schema_name_ptr_ != nullptr) {
                (yyval.show_stmt)->schema_name_ = (yyvsp[-4].table_name_t)->schema_name_ptr_;
                free((yyvsp[-4].table_name_t)->schema_name_ptr_);
            }
            (yyval.show_stmt)->table_name_ = (yyvsp[-4].table_name_t)->table_name_ptr_;
            free((yyvsp[-4].table_name_t)->table_name_ptr_);
            (yyval.show_stmt)->segment_id_ = (yyvsp[-2].long_value);
            (yyval.show_stmt)->block_id_ = (yyvsp[0].long_value);
            delete (yyvsp[-4].table_name_t);
        }
#line 6325 "parser.cpp"
        break;

        case 274: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 2004 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBlockColumn;
            if ((yyvsp[-6].table_name_t)->schema_name_ptr_ != nullptr) {
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
#line 6344 "parser.cpp"
        break;

        case 275: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 2018 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kIndexes;
            if ((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
                (yyval.show_stmt)->schema_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
                free((yyvsp[-1].table_name_t)->schema_name_ptr_);
            }
            (yyval.show_stmt)->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
            free((yyvsp[-1].table_name_t)->table_name_ptr_);
            delete (yyvsp[-1].table_name_t);
        }
#line 6360 "parser.cpp"
        break;

        case 276: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 2029 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kIndex;
            if ((yyvsp[-2].table_name_t)->schema_name_ptr_ != nullptr) {
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
#line 6380 "parser.cpp"
        break;

        case 277: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 2044 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kIndexSegment;
            if ((yyvsp[-4].table_name_t)->schema_name_ptr_ != nullptr) {
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
#line 6402 "parser.cpp"
        break;

        case 278: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNKS  */
#line 2061 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kIndexChunks;
            if ((yyvsp[-5].table_name_t)->schema_name_ptr_ != nullptr) {
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
#line 6424 "parser.cpp"
        break;

        case 279: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 2078 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kIndexChunk;
            if ((yyvsp[-6].table_name_t)->schema_name_ptr_ != nullptr) {
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
#line 6447 "parser.cpp"
        break;

        case 280: /* show_statement: SHOW LOGS  */
#line 2096 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
        }
#line 6456 "parser.cpp"
        break;

        case 281: /* show_statement: SHOW CATALOG  */
#line 2100 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalog;
        }
#line 6465 "parser.cpp"
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
#line 6477 "parser.cpp"
        break;

        case 283: /* show_statement: SHOW CATALOG TO file_path  */
#line 2111 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogToFile;
            (yyval.show_stmt)->file_path_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 6488 "parser.cpp"
        break;

        case 284: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2117 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
        }
#line 6497 "parser.cpp"
        break;

        case 285: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2121 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
        }
#line 6506 "parser.cpp"
        break;

        case 286: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2125 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
            (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 6517 "parser.cpp"
        break;

        case 287: /* show_statement: SHOW MEMORY  */
#line 2131 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
        }
#line 6526 "parser.cpp"
        break;

        case 288: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2135 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
        }
#line 6535 "parser.cpp"
        break;

        case 289: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2139 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
        }
#line 6544 "parser.cpp"
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
#line 6556 "parser.cpp"
        break;

        case 291: /* show_statement: SHOW SNAPSHOTS  */
#line 2150 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListSnapshots;
        }
#line 6565 "parser.cpp"
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
#line 6577 "parser.cpp"
        break;

        case 293: /* show_statement: SHOW CACHES  */
#line 2161 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCaches;
        }
#line 6586 "parser.cpp"
        break;

        case 294: /* show_statement: SHOW CACHE  */
#line 2165 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowCache;
        }
#line 6595 "parser.cpp"
        break;

        case 295: /* show_statement: SHOW COMPACT  */
#line 2169 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCompact;
        }
#line 6604 "parser.cpp"
        break;

        case 296: /* show_statement: SHOW COMPACT NOT NULLABLE  */
#line 2173 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCompact;
            (yyval.show_stmt)->show_nullable_ = false;
        }
#line 6614 "parser.cpp"
        break;

        case 297: /* show_statement: SHOW CHECKPOINT  */
#line 2178 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCheckpoint;
        }
#line 6623 "parser.cpp"
        break;

        case 298: /* show_statement: SHOW CHECKPOINT NOT NULLABLE  */
#line 2182 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCheckpoint;
            (yyval.show_stmt)->show_nullable_ = false;
        }
#line 6633 "parser.cpp"
        break;

        case 299: /* show_statement: SHOW CHECKPOINT LONG_VALUE  */
#line 2187 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowCheckpoint;
            (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
        }
#line 6643 "parser.cpp"
        break;

        case 300: /* show_statement: SHOW OPTIMIZE  */
#line 2192 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListOptimize;
        }
#line 6652 "parser.cpp"
        break;

        case 301: /* show_statement: SHOW OPTIMIZE NOT NULLABLE  */
#line 2196 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListOptimize;
            (yyval.show_stmt)->show_nullable_ = false;
        }
#line 6662 "parser.cpp"
        break;

        case 302: /* show_statement: SHOW IMPORT  */
#line 2201 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListImport;
        }
#line 6671 "parser.cpp"
        break;

        case 303: /* show_statement: SHOW CLEAN  */
#line 2205 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListClean;
        }
#line 6680 "parser.cpp"
        break;

        case 304: /* show_statement: SHOW CLEAN NOT NULLABLE  */
#line 2209 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListClean;
            (yyval.show_stmt)->show_nullable_ = false;
        }
#line 6690 "parser.cpp"
        break;

        case 305: /* show_statement: SHOW CLEAN LONG_VALUE  */
#line 2214 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowClean;
            (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
        }
#line 6700 "parser.cpp"
        break;

        case 306: /* flush_statement: FLUSH DATA  */
#line 2223 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
        }
#line 6709 "parser.cpp"
        break;

        case 307: /* flush_statement: FLUSH LOG  */
#line 2227 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
        }
#line 6718 "parser.cpp"
        break;

        case 308: /* flush_statement: FLUSH BUFFER  */
#line 2231 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
        }
#line 6727 "parser.cpp"
        break;

        case 309: /* flush_statement: FLUSH CATALOG  */
#line 2235 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kCatalog;
        }
#line 6736 "parser.cpp"
        break;

        case 310: /* optimize_statement: OPTIMIZE table_name  */
#line 2243 "parser.y"
        {
            (yyval.optimize_stmt) = new infinity::OptimizeStatement();
            if ((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
                (yyval.optimize_stmt)->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
                free((yyvsp[0].table_name_t)->schema_name_ptr_);
            }
            (yyval.optimize_stmt)->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
            free((yyvsp[0].table_name_t)->table_name_ptr_);
            delete (yyvsp[0].table_name_t);
        }
#line 6751 "parser.cpp"
        break;

        case 311: /* command_statement: USE IDENTIFIER  */
#line 2257 "parser.y"
        {
            (yyval.command_stmt) = new infinity::CommandStatement();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 6762 "parser.cpp"
        break;

        case 312: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2263 "parser.y"
        {
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
            free((yyvsp[0].str_value));
        }
#line 6772 "parser.cpp"
        break;

        case 313: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2268 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
            free((yyvsp[-1].str_value));
        }
#line 6783 "parser.cpp"
        break;

        case 314: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2274 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
            free((yyvsp[-1].str_value));
        }
#line 6794 "parser.cpp"
        break;

        case 315: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2280 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession,
                                                                                     infinity::SetVarType::kString,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].str_value));
            free((yyvsp[-1].str_value));
            free((yyvsp[0].str_value));
        }
#line 6807 "parser.cpp"
        break;

        case 316: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2288 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession,
                                                                                     infinity::SetVarType::kInteger,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].long_value));
            free((yyvsp[-1].str_value));
        }
#line 6818 "parser.cpp"
        break;

        case 317: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2294 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession,
                                                                                     infinity::SetVarType::kDouble,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].double_value));
            free((yyvsp[-1].str_value));
        }
#line 6829 "parser.cpp"
        break;

        case 318: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2300 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
            free((yyvsp[-1].str_value));
        }
#line 6840 "parser.cpp"
        break;

        case 319: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2306 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
            free((yyvsp[-1].str_value));
        }
#line 6851 "parser.cpp"
        break;

        case 320: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2312 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal,
                                                                                     infinity::SetVarType::kString,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].str_value));
            free((yyvsp[-1].str_value));
            free((yyvsp[0].str_value));
        }
#line 6864 "parser.cpp"
        break;

        case 321: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2320 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal,
                                                                                     infinity::SetVarType::kInteger,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].long_value));
            free((yyvsp[-1].str_value));
        }
#line 6875 "parser.cpp"
        break;

        case 322: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2326 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal,
                                                                                     infinity::SetVarType::kDouble,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].double_value));
            free((yyvsp[-1].str_value));
        }
#line 6886 "parser.cpp"
        break;

        case 323: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2332 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
            free((yyvsp[-1].str_value));
        }
#line 6897 "parser.cpp"
        break;

        case 324: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2338 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
            free((yyvsp[-1].str_value));
        }
#line 6908 "parser.cpp"
        break;

        case 325: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2344 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig,
                                                                                     infinity::SetVarType::kString,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].str_value));
            free((yyvsp[-1].str_value));
            free((yyvsp[0].str_value));
        }
#line 6921 "parser.cpp"
        break;

        case 326: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2352 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig,
                                                                                     infinity::SetVarType::kInteger,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].long_value));
            free((yyvsp[-1].str_value));
        }
#line 6932 "parser.cpp"
        break;

        case 327: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2358 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig,
                                                                                     infinity::SetVarType::kDouble,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].double_value));
            free((yyvsp[-1].str_value));
        }
#line 6943 "parser.cpp"
        break;

        case 328: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON TABLE IDENTIFIER  */
#line 2364 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-3].str_value));
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value),
                                                                                          infinity::SnapshotOp::kCreate,
                                                                                          infinity::SnapshotScope::kTable,
                                                                                          (yyvsp[0].str_value));
            free((yyvsp[-3].str_value));
            free((yyvsp[0].str_value));
        }
#line 6956 "parser.cpp"
        break;

        case 329: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON DATABASE IDENTIFIER  */
#line 2372 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-3].str_value));
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value),
                                                                                          infinity::SnapshotOp::kCreate,
                                                                                          infinity::SnapshotScope::kDatabase,
                                                                                          (yyvsp[0].str_value));
            free((yyvsp[-3].str_value));
            free((yyvsp[0].str_value));
        }
#line 6969 "parser.cpp"
        break;

        case 330: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON SYSTEM  */
#line 2380 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SnapshotCmd>((yyvsp[-2].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kSystem);
            free((yyvsp[-2].str_value));
        }
#line 6980 "parser.cpp"
        break;

        case 331: /* command_statement: DROP SNAPSHOT IDENTIFIER  */
#line 2386 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kDrop, infinity::SnapshotScope::kIgnore);
            free((yyvsp[0].str_value));
        }
#line 6991 "parser.cpp"
        break;

        case 332: /* command_statement: RESTORE SYSTEM SNAPSHOT IDENTIFIER  */
#line 2392 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kSystem);
            free((yyvsp[0].str_value));
        }
#line 7002 "parser.cpp"
        break;

        case 333: /* command_statement: RESTORE DATABASE SNAPSHOT IDENTIFIER  */
#line 2398 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kDatabase);
            free((yyvsp[0].str_value));
        }
#line 7013 "parser.cpp"
        break;

        case 334: /* command_statement: RESTORE TABLE SNAPSHOT IDENTIFIER  */
#line 2404 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kTable);
            free((yyvsp[0].str_value));
        }
#line 7024 "parser.cpp"
        break;

        case 335: /* command_statement: CLEAN DATA  */
#line 2410 "parser.y"
        {
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::CleanupCmd>();
        }
#line 7033 "parser.cpp"
        break;

        case 336: /* command_statement: DUMP INDEX IDENTIFIER ON table_name  */
#line 2414 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::DumpIndexCmd>((yyvsp[0].table_name_t)->schema_name_ptr_,
                                                                                           (yyvsp[0].table_name_t)->table_name_ptr_,
                                                                                           (yyvsp[-2].str_value));
            free((yyvsp[-2].str_value));
            if ((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
                free((yyvsp[0].table_name_t)->schema_name_ptr_);
            }
            free((yyvsp[0].table_name_t)->table_name_ptr_);
            delete (yyvsp[0].table_name_t);
        }
#line 7049 "parser.cpp"
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
#line 7066 "parser.cpp"
        break;

        case 338: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2439 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
            (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
            (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
        }
#line 7077 "parser.cpp"
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
#line 7089 "parser.cpp"
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
#line 7102 "parser.cpp"
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
#line 7116 "parser.cpp"
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
#line 7131 "parser.cpp"
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
#line 7146 "parser.cpp"
        break;

        case 344: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT
                     LONG_VALUE  */
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
#line 7162 "parser.cpp"
        break;

        case 345: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT
                     LONG_VALUE BLOCKS  */
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
#line 7178 "parser.cpp"
        break;

        case 346: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT
                     LONG_VALUE BLOCK LONG_VALUE  */
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
#line 7195 "parser.cpp"
        break;

        case 347: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT
                     LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
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
#line 7212 "parser.cpp"
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
#line 7227 "parser.cpp"
        break;

        case 349: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX
                     LONG_VALUE  */
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
#line 7243 "parser.cpp"
        break;

        case 350: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX
                     LONG_VALUE LONG_VALUE SEGMENTS  */
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
#line 7260 "parser.cpp"
        break;

        case 351: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX
                     LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
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
#line 7278 "parser.cpp"
        break;

        case 352: /* admin_statement: ADMIN SHOW LOGS  */
#line 2581 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
        }
#line 7287 "parser.cpp"
        break;

        case 353: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2585 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
            (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
        }
#line 7297 "parser.cpp"
        break;

        case 354: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2590 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
            (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
        }
#line 7307 "parser.cpp"
        break;

        case 355: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2595 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
            (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
            (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
        }
#line 7318 "parser.cpp"
        break;

        case 356: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2601 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
        }
#line 7327 "parser.cpp"
        break;

        case 357: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2605 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
        }
#line 7336 "parser.cpp"
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
#line 7348 "parser.cpp"
        break;

        case 359: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2616 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
        }
#line 7357 "parser.cpp"
        break;

        case 360: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2620 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
        }
#line 7366 "parser.cpp"
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
#line 7378 "parser.cpp"
        break;

        case 362: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2631 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
            (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7389 "parser.cpp"
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
#line 7402 "parser.cpp"
        break;

        case 364: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2645 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
            (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7413 "parser.cpp"
        break;

        case 365: /* admin_statement: ADMIN SHOW NODES  */
#line 2651 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
        }
#line 7422 "parser.cpp"
        break;

        case 366: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2655 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
            (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7433 "parser.cpp"
        break;

        case 367: /* admin_statement: ADMIN SHOW NODE  */
#line 2661 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
        }
#line 7442 "parser.cpp"
        break;

        case 368: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2665 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
            (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7453 "parser.cpp"
        break;

        case 369: /* admin_statement: ADMIN SET ADMIN  */
#line 2671 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
            (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
        }
#line 7463 "parser.cpp"
        break;

        case 370: /* admin_statement: ADMIN SET STANDALONE  */
#line 2676 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
            (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
        }
#line 7473 "parser.cpp"
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
#line 7485 "parser.cpp"
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
#line 7499 "parser.cpp"
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
#line 7513 "parser.cpp"
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
#line 7528 "parser.cpp"
        break;

        case 375: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2717 "parser.y"
        {
            auto *ret = new infinity::AddColumnsStatement((yyvsp[-5].table_name_t)->schema_name_ptr_, (yyvsp[-5].table_name_t)->table_name_ptr_);
            (yyval.alter_stmt) = ret;

            for (infinity::ColumnDef *&column_def : *(yyvsp[-1].column_def_array_t)) {
                ret->column_defs_.emplace_back(column_def);
            }
            delete (yyvsp[-1].column_def_array_t);
            free((yyvsp[-5].table_name_t)->schema_name_ptr_);
            free((yyvsp[-5].table_name_t)->table_name_ptr_);
            delete (yyvsp[-5].table_name_t);
        }
#line 7545 "parser.cpp"
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
#line 7561 "parser.cpp"
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
#line 7584 "parser.cpp"
        break;

        case 378: /* check_statement: CHECK SYSTEM  */
#line 2759 "parser.y"
        {
            (yyval.check_stmt) = new infinity::CheckStatement();
            (yyval.check_stmt)->check_type_ = infinity::CheckStmtType::kSystem;
        }
#line 7593 "parser.cpp"
        break;

        case 379: /* check_statement: CHECK TABLE table_name  */
#line 2763 "parser.y"
        {
            (yyval.check_stmt) = new infinity::CheckStatement();
            (yyval.check_stmt)->check_type_ = infinity::CheckStmtType::kTable;
            if ((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
                (yyval.check_stmt)->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
                free((yyvsp[0].table_name_t)->schema_name_ptr_);
            }
            (yyval.check_stmt)->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
            free((yyvsp[0].table_name_t)->table_name_ptr_);
            delete (yyvsp[0].table_name_t);
        }
#line 7609 "parser.cpp"
        break;

        case 380: /* expr_array: expr_alias  */
#line 2779 "parser.y"
        {
            (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr *>();
            (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
        }
#line 7618 "parser.cpp"
        break;

        case 381: /* expr_array: expr_array ',' expr_alias  */
#line 2783 "parser.y"
        {
            (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
        }
#line 7627 "parser.cpp"
        break;

        case 382: /* insert_row_list: '(' expr_array ')'  */
#line 2788 "parser.y"
        {
            auto res = std::make_unique<infinity::InsertRowExpr>();
            for (auto *&expr : *(yyvsp[-1].expr_array_t)) {
                res->values_.emplace_back(expr);
                expr = nullptr;
            }
            delete (yyvsp[-1].expr_array_t);
            (yyval.insert_row_list_t) = new std::vector<infinity::InsertRowExpr *>();
            (yyval.insert_row_list_t)->emplace_back(res.release());
        }
#line 7642 "parser.cpp"
        break;

        case 383: /* insert_row_list: insert_row_list ',' '(' expr_array ')'  */
#line 2798 "parser.y"
        {
            (yyval.insert_row_list_t) = (yyvsp[-4].insert_row_list_t);
            auto res = std::make_unique<infinity::InsertRowExpr>();
            for (auto *&expr : *(yyvsp[-1].expr_array_t)) {
                res->values_.emplace_back(expr);
                expr = nullptr;
            }
            delete (yyvsp[-1].expr_array_t);
            (yyval.insert_row_list_t)->emplace_back(res.release());
        }
#line 7657 "parser.cpp"
        break;

        case 384: /* expr_alias: expr AS IDENTIFIER  */
#line 2820 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[-2].expr_t);
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7668 "parser.cpp"
        break;

        case 385: /* expr_alias: expr  */
#line 2826 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 7676 "parser.cpp"
        break;

        case 391: /* operand: '(' expr ')'  */
#line 2836 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[-1].expr_t);
        }
#line 7684 "parser.cpp"
        break;

        case 392: /* operand: '(' select_without_paren ')'  */
#line 2839 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 7695 "parser.cpp"
        break;

        case 393: /* operand: constant_expr  */
#line 2845 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].const_expr_t);
        }
#line 7703 "parser.cpp"
        break;

        case 404: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING
                     optional_search_filter_expr ')'  */
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
#line 7723 "parser.cpp"
        break;

        case 405: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING
                     optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')'  */
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
#line 7745 "parser.cpp"
        break;

        case 406: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING
                     optional_search_filter_expr ')' IGNORE INDEX  */
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
#line 7766 "parser.cpp"
        break;

        case 407: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING
                     INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2913 "parser.y"
        {
            infinity::KnnExpr *match_vector_expr = new infinity::KnnExpr();
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
            if (auto *const_expr = dynamic_cast<infinity::ConstantExpr *>((yyvsp[-14].expr_t))) {
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
            for (auto *param_ptr : *(yyvsp[0].with_index_param_list_t)) {
                delete param_ptr;
            }
            delete (yyvsp[0].with_index_param_list_t);
            free((yyvsp[-12].str_value));
            free((yyvsp[-10].str_value));
            free((yyvsp[-2].str_value));
            if (auto *const_expr = dynamic_cast<infinity::ConstantExpr *>((yyvsp[-14].expr_t))) {
                delete (yyvsp[-14].expr_t);
            } else {
                delete (yyvsp[-14].expr_t);
            }
            delete (yyval.expr_t);
            yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
            YYERROR;
        Return1:;
        }
#line 7828 "parser.cpp"
        break;

        case 408: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE
                     INDEX  */
#line 2971 "parser.y"
        {
            infinity::KnnExpr *match_vector_expr = new infinity::KnnExpr();
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
            if (auto *const_expr = dynamic_cast<infinity::ConstantExpr *>((yyvsp[-10].expr_t))) {
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
            if (auto *const_expr = dynamic_cast<infinity::ConstantExpr *>((yyvsp[-10].expr_t))) {
                delete (yyvsp[-10].expr_t);
            } else {
                delete (yyvsp[-10].expr_t);
            }
            delete (yyval.expr_t);
            yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
            YYERROR;
        Return2:;
        }
#line 7882 "parser.cpp"
        break;

        case 409: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')'
                     with_index_param_list  */
#line 3021 "parser.y"
        {
            infinity::KnnExpr *match_vector_expr = new infinity::KnnExpr();
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
            if (auto *const_expr = dynamic_cast<infinity::ConstantExpr *>((yyvsp[-9].expr_t))) {
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
            for (auto *param_ptr : *(yyvsp[0].with_index_param_list_t)) {
                delete param_ptr;
            }
            delete (yyvsp[0].with_index_param_list_t);
            free((yyvsp[-7].str_value));
            free((yyvsp[-5].str_value));
            if (auto *const_expr = dynamic_cast<infinity::ConstantExpr *>((yyvsp[-9].expr_t))) {
                delete (yyvsp[-9].expr_t);
            } else {
                delete (yyvsp[-9].expr_t);
            }
            delete (yyval.expr_t);
            yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
            YYERROR;
        Return3:;
        }
#line 7940 "parser.cpp"
        break;

        case 410: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3075 "parser.y"
        {
            infinity::KnnExpr *match_vector_expr = new infinity::KnnExpr();
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
            if (auto *const_expr = dynamic_cast<infinity::ConstantExpr *>((yyvsp[-7].expr_t))) {
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
            for (auto *param_ptr : *(yyvsp[0].with_index_param_list_t)) {
                delete param_ptr;
            }
            delete (yyvsp[0].with_index_param_list_t);
            free((yyvsp[-5].str_value));
            free((yyvsp[-3].str_value));
            if (auto *const_expr = dynamic_cast<infinity::ConstantExpr *>((yyvsp[-7].expr_t))) {
                delete (yyvsp[-7].expr_t);
            } else {
                delete (yyvsp[-7].expr_t);
            }
            delete (yyval.expr_t);
            yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
            YYERROR;
        Return4:;
        }
#line 7999 "parser.cpp"
        break;

        case 411: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')'
                     USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
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
#line 8028 "parser.cpp"
        break;

        case 412: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')'
                     IGNORE INDEX  */
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
#line 8055 "parser.cpp"
        break;

        case 413: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')'
                     with_index_param_list  */
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
#line 8080 "parser.cpp"
        break;

        case 414: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')'
                     with_index_param_list  */
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
#line 8105 "parser.cpp"
        break;

        case 415: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3223 "parser.y"
        {
            infinity::MatchExpr *match_text_expr = new infinity::MatchExpr();
            match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
            match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
            match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
            free((yyvsp[-4].str_value));
            free((yyvsp[-2].str_value));
            (yyval.expr_t) = match_text_expr;
        }
#line 8119 "parser.cpp"
        break;

        case 416: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3232 "parser.y"
        {
            infinity::MatchExpr *match_text_expr = new infinity::MatchExpr();
            match_text_expr->fields_ = std::string((yyvsp[-6].str_value));
            match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
            match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
            match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
            free((yyvsp[-6].str_value));
            free((yyvsp[-4].str_value));
            free((yyvsp[-2].str_value));
            (yyval.expr_t) = match_text_expr;
        }
#line 8135 "parser.cpp"
        break;

        case 417: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 3244 "parser.y"
        {
            infinity::MatchExpr *match_text_expr = new infinity::MatchExpr();
            match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
            match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
            free((yyvsp[-2].str_value));
            (yyval.expr_t) = match_text_expr;
        }
#line 8147 "parser.cpp"
        break;

        case 418: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3251 "parser.y"
        {
            infinity::MatchExpr *match_text_expr = new infinity::MatchExpr();
            match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
            match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
            match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
            free((yyvsp[-4].str_value));
            free((yyvsp[-2].str_value));
            (yyval.expr_t) = match_text_expr;
        }
#line 8161 "parser.cpp"
        break;

        case 419: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3261 "parser.y"
        {
            infinity::FusionExpr *fusion_expr = new infinity::FusionExpr();
            fusion_expr->method_ = std::string((yyvsp[-1].str_value));
            free((yyvsp[-1].str_value));
            (yyval.expr_t) = fusion_expr;
        }
#line 8172 "parser.cpp"
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
#line 8188 "parser.cpp"
        break;

        case 421: /* sub_search: match_vector_expr  */
#line 3279 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8196 "parser.cpp"
        break;

        case 422: /* sub_search: match_text_expr  */
#line 3282 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8204 "parser.cpp"
        break;

        case 423: /* sub_search: match_tensor_expr  */
#line 3285 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8212 "parser.cpp"
        break;

        case 424: /* sub_search: match_sparse_expr  */
#line 3288 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8220 "parser.cpp"
        break;

        case 425: /* sub_search: query_expr  */
#line 3291 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8228 "parser.cpp"
        break;

        case 426: /* sub_search: fusion_expr  */
#line 3294 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8236 "parser.cpp"
        break;

        case 427: /* sub_search_array: sub_search  */
#line 3298 "parser.y"
        {
            (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr *>();
            (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
        }
#line 8245 "parser.cpp"
        break;

        case 428: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3302 "parser.y"
        {
            (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
        }
#line 8254 "parser.cpp"
        break;

        case 429: /* function_expr: IDENTIFIER '(' ')'  */
#line 3307 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            ParserHelper::ToLower((yyvsp[-2].str_value));
            func_expr->func_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));
            func_expr->arguments_ = nullptr;
            (yyval.expr_t) = func_expr;
        }
#line 8267 "parser.cpp"
        break;

        case 430: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3315 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            ParserHelper::ToLower((yyvsp[-3].str_value));
            func_expr->func_name_ = (yyvsp[-3].str_value);
            free((yyvsp[-3].str_value));
            func_expr->arguments_ = (yyvsp[-1].expr_array_t);
            (yyval.expr_t) = func_expr;
        }
#line 8280 "parser.cpp"
        break;

        case 431: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 3323 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            ParserHelper::ToLower((yyvsp[-4].str_value));
            func_expr->func_name_ = (yyvsp[-4].str_value);
            free((yyvsp[-4].str_value));
            func_expr->arguments_ = (yyvsp[-1].expr_array_t);
            func_expr->distinct_ = true;
            (yyval.expr_t) = func_expr;
        }
#line 8294 "parser.cpp"
        break;

        case 432: /* function_expr: YEAR '(' expr ')'  */
#line 3332 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "year";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8306 "parser.cpp"
        break;

        case 433: /* function_expr: MONTH '(' expr ')'  */
#line 3339 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "month";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8318 "parser.cpp"
        break;

        case 434: /* function_expr: DAY '(' expr ')'  */
#line 3346 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "day";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8330 "parser.cpp"
        break;

        case 435: /* function_expr: HOUR '(' expr ')'  */
#line 3353 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "hour";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8342 "parser.cpp"
        break;

        case 436: /* function_expr: MINUTE '(' expr ')'  */
#line 3360 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "minute";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8354 "parser.cpp"
        break;

        case 437: /* function_expr: SECOND '(' expr ')'  */
#line 3367 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "second";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8366 "parser.cpp"
        break;

        case 438: /* function_expr: operand IS NOT NULLABLE  */
#line 3374 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "is_not_null";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8378 "parser.cpp"
        break;

        case 439: /* function_expr: operand IS NULLABLE  */
#line 3381 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "is_null";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8390 "parser.cpp"
        break;

        case 440: /* function_expr: NOT operand  */
#line 3388 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "not";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8402 "parser.cpp"
        break;

        case 441: /* function_expr: '-' operand  */
#line 3395 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "-";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8414 "parser.cpp"
        break;

        case 442: /* function_expr: '+' operand  */
#line 3402 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "+";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8426 "parser.cpp"
        break;

        case 443: /* function_expr: operand '-' operand  */
#line 3409 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "-";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8439 "parser.cpp"
        break;

        case 444: /* function_expr: operand '+' operand  */
#line 3417 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "+";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8452 "parser.cpp"
        break;

        case 445: /* function_expr: operand '*' operand  */
#line 3425 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "*";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8465 "parser.cpp"
        break;

        case 446: /* function_expr: operand '/' operand  */
#line 3433 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "/";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8478 "parser.cpp"
        break;

        case 447: /* function_expr: operand '%' operand  */
#line 3441 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "%";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8491 "parser.cpp"
        break;

        case 448: /* function_expr: operand '=' operand  */
#line 3449 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8504 "parser.cpp"
        break;

        case 449: /* function_expr: operand EQUAL operand  */
#line 3457 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8517 "parser.cpp"
        break;

        case 450: /* function_expr: operand NOT_EQ operand  */
#line 3465 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "<>";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8530 "parser.cpp"
        break;

        case 451: /* function_expr: operand '<' operand  */
#line 3473 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "<";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8543 "parser.cpp"
        break;

        case 452: /* function_expr: operand '>' operand  */
#line 3481 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = ">";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8556 "parser.cpp"
        break;

        case 453: /* function_expr: operand LESS_EQ operand  */
#line 3489 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "<=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8569 "parser.cpp"
        break;

        case 454: /* function_expr: operand GREATER_EQ operand  */
#line 3497 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = ">=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8582 "parser.cpp"
        break;

        case 455: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3505 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            ParserHelper::ToLower((yyvsp[-3].str_value));
            if (strcmp((yyvsp[-3].str_value), "year") == 0) {
                func_expr->func_name_ = "extract_year";
                func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            } else if (strcmp((yyvsp[-3].str_value), "month") == 0) {
                func_expr->func_name_ = "extract_month";
                func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            } else if (strcmp((yyvsp[-3].str_value), "day") == 0) {
                func_expr->func_name_ = "extract_day";
                func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            } else if (strcmp((yyvsp[-3].str_value), "hour") == 0) {
                func_expr->func_name_ = "extract_hour";
                func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            } else if (strcmp((yyvsp[-3].str_value), "minute") == 0) {
                func_expr->func_name_ = "extract_minute";
                func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            } else if (strcmp((yyvsp[-3].str_value), "second") == 0) {
                func_expr->func_name_ = "extract_second";
                func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            } else {
                delete func_expr;
                yyerror(&yyloc, scanner, result, "Invalid column expression format");
                YYERROR;
            }
            free((yyvsp[-3].str_value));
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8617 "parser.cpp"
        break;

        case 456: /* function_expr: operand LIKE operand  */
#line 3535 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "like";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8630 "parser.cpp"
        break;

        case 457: /* function_expr: operand NOT LIKE operand  */
#line 3543 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "not_like";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8643 "parser.cpp"
        break;

        case 458: /* conjunction_expr: expr AND expr  */
#line 3552 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "and";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8656 "parser.cpp"
        break;

        case 459: /* conjunction_expr: expr OR expr  */
#line 3560 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "or";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8669 "parser.cpp"
        break;

        case 460: /* between_expr: operand BETWEEN operand AND operand  */
#line 3569 "parser.y"
        {
            infinity::BetweenExpr *between_expr = new infinity::BetweenExpr();
            between_expr->value_ = (yyvsp[-4].expr_t);
            between_expr->lower_bound_ = (yyvsp[-2].expr_t);
            between_expr->upper_bound_ = (yyvsp[0].expr_t);
            (yyval.expr_t) = between_expr;
        }
#line 8681 "parser.cpp"
        break;

        case 461: /* in_expr: operand IN '(' expr_array ')'  */
#line 3577 "parser.y"
        {
            infinity::InExpr *in_expr = new infinity::InExpr(true);
            in_expr->left_ = (yyvsp[-4].expr_t);
            in_expr->arguments_ = (yyvsp[-1].expr_array_t);
            (yyval.expr_t) = in_expr;
        }
#line 8692 "parser.cpp"
        break;

        case 462: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3583 "parser.y"
        {
            infinity::InExpr *in_expr = new infinity::InExpr(false);
            in_expr->left_ = (yyvsp[-5].expr_t);
            in_expr->arguments_ = (yyvsp[-1].expr_array_t);
            (yyval.expr_t) = in_expr;
        }
#line 8703 "parser.cpp"
        break;

        case 463: /* case_expr: CASE expr case_check_array END  */
#line 3590 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->expr_ = (yyvsp[-2].expr_t);
            case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
            (yyval.expr_t) = case_expr;
        }
#line 8714 "parser.cpp"
        break;

        case 464: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3596 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->expr_ = (yyvsp[-4].expr_t);
            case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
            case_expr->else_expr_ = (yyvsp[-1].expr_t);
            (yyval.expr_t) = case_expr;
        }
#line 8726 "parser.cpp"
        break;

        case 465: /* case_expr: CASE case_check_array END  */
#line 3603 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
            (yyval.expr_t) = case_expr;
        }
#line 8736 "parser.cpp"
        break;

        case 466: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3608 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
            case_expr->else_expr_ = (yyvsp[-1].expr_t);
            (yyval.expr_t) = case_expr;
        }
#line 8747 "parser.cpp"
        break;

        case 467: /* case_check_array: WHEN expr THEN expr  */
#line 3615 "parser.y"
        {
            (yyval.case_check_array_t) = new std::vector<infinity::WhenThen *>();
            infinity::WhenThen *when_then_ptr = new infinity::WhenThen();
            when_then_ptr->when_ = (yyvsp[-2].expr_t);
            when_then_ptr->then_ = (yyvsp[0].expr_t);
            (yyval.case_check_array_t)->emplace_back(when_then_ptr);
        }
#line 8759 "parser.cpp"
        break;

        case 468: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3622 "parser.y"
        {
            infinity::WhenThen *when_then_ptr = new infinity::WhenThen();
            when_then_ptr->when_ = (yyvsp[-2].expr_t);
            when_then_ptr->then_ = (yyvsp[0].expr_t);
            (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
            (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
        }
#line 8771 "parser.cpp"
        break;

        case 469: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3630 "parser.y"
        {
            auto [data_type_result, fail_reason] =
                infinity::ColumnType::GetDataTypeFromColumnType(*((yyvsp[-1].column_type_t)),
                                                                std::vector<std::unique_ptr<infinity::InitParameter>>{});
            delete (yyvsp[-1].column_type_t);
            if (!data_type_result) {
                yyerror(&yyloc, scanner, result, fail_reason.c_str());
                delete (yyvsp[-3].expr_t);
                YYERROR;
            }
            infinity::CastExpr *cast_expr = new infinity::CastExpr(std::move(*data_type_result));
            cast_expr->expr_ = (yyvsp[-3].expr_t);
            (yyval.expr_t) = cast_expr;
        }
#line 8788 "parser.cpp"
        break;

        case 470: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3643 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8799 "parser.cpp"
        break;

        case 471: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3649 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8810 "parser.cpp"
        break;

        case 472: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3655 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
            subquery_expr->left_ = (yyvsp[-4].expr_t);
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8822 "parser.cpp"
        break;

        case 473: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3662 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
            subquery_expr->left_ = (yyvsp[-5].expr_t);
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8834 "parser.cpp"
        break;

        case 474: /* column_expr: IDENTIFIER  */
#line 3670 "parser.y"
        {
            infinity::ColumnExpr *column_expr = new infinity::ColumnExpr();
            ParserHelper::ToLower((yyvsp[0].str_value));
            column_expr->names_.emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
            (yyval.expr_t) = column_expr;
        }
#line 8846 "parser.cpp"
        break;

        case 475: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3677 "parser.y"
        {
            infinity::ColumnExpr *column_expr = (infinity::ColumnExpr *)(yyvsp[-2].expr_t);
            ParserHelper::ToLower((yyvsp[0].str_value));
            column_expr->names_.emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
            (yyval.expr_t) = column_expr;
        }
#line 8858 "parser.cpp"
        break;

        case 476: /* column_expr: '*'  */
#line 3684 "parser.y"
        {
            infinity::ColumnExpr *column_expr = new infinity::ColumnExpr();
            column_expr->star_ = true;
            (yyval.expr_t) = column_expr;
        }
#line 8868 "parser.cpp"
        break;

        case 477: /* column_expr: column_expr '.' '*'  */
#line 3689 "parser.y"
        {
            infinity::ColumnExpr *column_expr = (infinity::ColumnExpr *)(yyvsp[-2].expr_t);
            if (column_expr->star_) {
                yyerror(&yyloc, scanner, result, "Invalid column expression format");
                YYERROR;
            }
            column_expr->star_ = true;
            (yyval.expr_t) = column_expr;
        }
#line 8882 "parser.cpp"
        break;

        case 478: /* constant_expr: STRING  */
#line 3699 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
            const_expr->str_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8892 "parser.cpp"
        break;

        case 479: /* constant_expr: TRUE  */
#line 3704 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
            const_expr->bool_value_ = true;
            (yyval.const_expr_t) = const_expr;
        }
#line 8902 "parser.cpp"
        break;

        case 480: /* constant_expr: FALSE  */
#line 3709 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
            const_expr->bool_value_ = false;
            (yyval.const_expr_t) = const_expr;
        }
#line 8912 "parser.cpp"
        break;

        case 481: /* constant_expr: DOUBLE_VALUE  */
#line 3714 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
            const_expr->double_value_ = (yyvsp[0].double_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8922 "parser.cpp"
        break;

        case 482: /* constant_expr: LONG_VALUE  */
#line 3719 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
            const_expr->integer_value_ = (yyvsp[0].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8932 "parser.cpp"
        break;

        case 483: /* constant_expr: JSON STRING  */
#line 3724 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kJson);
            auto value_str = infinity::JsonManager::parse((yyvsp[0].str_value));
            auto value_bson = infinity::JsonManager::to_bson(value_str);
            const_expr->json_value_ = value_bson;
            (yyval.const_expr_t) = const_expr;
        }
#line 8944 "parser.cpp"
        break;

        case 484: /* constant_expr: DATE STRING  */
#line 3731 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8954 "parser.cpp"
        break;

        case 485: /* constant_expr: TIME STRING  */
#line 3736 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8964 "parser.cpp"
        break;

        case 486: /* constant_expr: DATETIME STRING  */
#line 3741 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8974 "parser.cpp"
        break;

        case 487: /* constant_expr: TIMESTAMP STRING  */
#line 3746 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8984 "parser.cpp"
        break;

        case 488: /* constant_expr: INTERVAL interval_expr  */
#line 3751 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 8992 "parser.cpp"
        break;

        case 489: /* constant_expr: interval_expr  */
#line 3754 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9000 "parser.cpp"
        break;

        case 490: /* constant_expr: common_array_expr  */
#line 3757 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9008 "parser.cpp"
        break;

        case 491: /* constant_expr: curly_brackets_expr  */
#line 3760 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9016 "parser.cpp"
        break;

        case 492: /* common_array_expr: array_expr  */
#line 3764 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9024 "parser.cpp"
        break;

        case 493: /* common_array_expr: subarray_array_expr  */
#line 3767 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9032 "parser.cpp"
        break;

        case 494: /* common_array_expr: sparse_array_expr  */
#line 3770 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9040 "parser.cpp"
        break;

        case 495: /* common_array_expr: empty_array_expr  */
#line 3773 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9048 "parser.cpp"
        break;

        case 496: /* common_sparse_array_expr: sparse_array_expr  */
#line 3777 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9056 "parser.cpp"
        break;

        case 497: /* common_sparse_array_expr: array_expr  */
#line 3780 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9064 "parser.cpp"
        break;

        case 498: /* common_sparse_array_expr: empty_array_expr  */
#line 3783 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9072 "parser.cpp"
        break;

        case 499: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3787 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9080 "parser.cpp"
        break;

        case 500: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3791 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
            const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
            (yyval.const_expr_t) = const_expr;
        }
#line 9090 "parser.cpp"
        break;

        case 501: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3796 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9099 "parser.cpp"
        break;

        case 502: /* sparse_array_expr: long_sparse_array_expr  */
#line 3801 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9107 "parser.cpp"
        break;

        case 503: /* sparse_array_expr: double_sparse_array_expr  */
#line 3804 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9115 "parser.cpp"
        break;

        case 504: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3808 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9123 "parser.cpp"
        break;

        case 505: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3812 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
            const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
            const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
            delete (yyvsp[0].int_sparse_ele_t);
            (yyval.const_expr_t) = const_expr;
        }
#line 9135 "parser.cpp"
        break;

        case 506: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3819 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
            (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
            delete (yyvsp[0].int_sparse_ele_t);
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9146 "parser.cpp"
        break;

        case 507: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3826 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9154 "parser.cpp"
        break;

        case 508: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3830 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
            const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
            const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
            delete (yyvsp[0].float_sparse_ele_t);
            (yyval.const_expr_t) = const_expr;
        }
#line 9166 "parser.cpp"
        break;

        case 509: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3837 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
            (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
            delete (yyvsp[0].float_sparse_ele_t);
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9177 "parser.cpp"
        break;

        case 510: /* empty_array_expr: '[' ']'  */
#line 3844 "parser.y"
        {
            (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
        }
#line 9185 "parser.cpp"
        break;

        case 511: /* curly_brackets_expr: unclosed_curly_brackets_expr '}'  */
#line 3848 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9193 "parser.cpp"
        break;

        case 512: /* curly_brackets_expr: '{' '}'  */
#line 3851 "parser.y"
        {
            (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
        }
#line 9201 "parser.cpp"
        break;

        case 513: /* unclosed_curly_brackets_expr: '{' constant_expr  */
#line 3855 "parser.y"
        {
            (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
            (yyval.const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
        }
#line 9210 "parser.cpp"
        break;

        case 514: /* unclosed_curly_brackets_expr: unclosed_curly_brackets_expr ',' constant_expr  */
#line 3859 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9219 "parser.cpp"
        break;

        case 515: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3864 "parser.y"
        {
            (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
        }
#line 9227 "parser.cpp"
        break;

        case 516: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3868 "parser.y"
        {
            (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
        }
#line 9235 "parser.cpp"
        break;

        case 517: /* array_expr: long_array_expr  */
#line 3872 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9243 "parser.cpp"
        break;

        case 518: /* array_expr: double_array_expr  */
#line 3875 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9251 "parser.cpp"
        break;

        case 519: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3879 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9259 "parser.cpp"
        break;

        case 520: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3883 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
            const_expr->long_array_.emplace_back((yyvsp[0].long_value));
            (yyval.const_expr_t) = const_expr;
        }
#line 9269 "parser.cpp"
        break;

        case 521: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3888 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9278 "parser.cpp"
        break;

        case 522: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3893 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9286 "parser.cpp"
        break;

        case 523: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3897 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
            const_expr->double_array_.emplace_back((yyvsp[0].double_value));
            (yyval.const_expr_t) = const_expr;
        }
#line 9296 "parser.cpp"
        break;

        case 524: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3902 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9305 "parser.cpp"
        break;

        case 525: /* interval_expr: LONG_VALUE SECONDS  */
#line 3907 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kSecond;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9316 "parser.cpp"
        break;

        case 526: /* interval_expr: LONG_VALUE SECOND  */
#line 3913 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kSecond;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9327 "parser.cpp"
        break;

        case 527: /* interval_expr: LONG_VALUE MINUTES  */
#line 3919 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMinute;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9338 "parser.cpp"
        break;

        case 528: /* interval_expr: LONG_VALUE MINUTE  */
#line 3925 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMinute;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9349 "parser.cpp"
        break;

        case 529: /* interval_expr: LONG_VALUE HOURS  */
#line 3931 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kHour;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9360 "parser.cpp"
        break;

        case 530: /* interval_expr: LONG_VALUE HOUR  */
#line 3937 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kHour;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9371 "parser.cpp"
        break;

        case 531: /* interval_expr: LONG_VALUE DAYS  */
#line 3943 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kDay;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9382 "parser.cpp"
        break;

        case 532: /* interval_expr: LONG_VALUE DAY  */
#line 3949 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kDay;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9393 "parser.cpp"
        break;

        case 533: /* interval_expr: LONG_VALUE MONTHS  */
#line 3955 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMonth;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9404 "parser.cpp"
        break;

        case 534: /* interval_expr: LONG_VALUE MONTH  */
#line 3961 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMonth;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9415 "parser.cpp"
        break;

        case 535: /* interval_expr: LONG_VALUE YEARS  */
#line 3967 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kYear;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9426 "parser.cpp"
        break;

        case 536: /* interval_expr: LONG_VALUE YEAR  */
#line 3973 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kYear;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9437 "parser.cpp"
        break;

        case 537: /* copy_option_list: copy_option  */
#line 3984 "parser.y"
        {
            (yyval.copy_option_array) = new std::vector<infinity::CopyOption *>();
            (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
        }
#line 9446 "parser.cpp"
        break;

        case 538: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3988 "parser.y"
        {
            (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
            (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
        }
#line 9455 "parser.cpp"
        break;

        case 539: /* copy_option: FORMAT IDENTIFIER  */
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
#line 9492 "parser.cpp"
        break;

        case 540: /* copy_option: DELIMITER STRING  */
#line 4025 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kDelimiter;
            if (strlen((yyvsp[0].str_value)) > 1 && (yyvsp[0].str_value)[0] == '\\') {
                if ((yyvsp[0].str_value)[1] == 't')
                    (yyval.copy_option_t)->delimiter_ = '\t';
            } else {
                (yyval.copy_option_t)->delimiter_ = (yyvsp[0].str_value)[0];
            }
            free((yyvsp[0].str_value));
        }
#line 9507 "parser.cpp"
        break;

        case 541: /* copy_option: HEADER  */
#line 4035 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
            (yyval.copy_option_t)->header_ = true;
        }
#line 9517 "parser.cpp"
        break;

        case 542: /* copy_option: OFFSET LONG_VALUE  */
#line 4040 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
            (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
        }
#line 9527 "parser.cpp"
        break;

        case 543: /* copy_option: LIMIT LONG_VALUE  */
#line 4045 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
            (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
        }
#line 9537 "parser.cpp"
        break;

        case 544: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 4050 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
            (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
        }
#line 9547 "parser.cpp"
        break;

        case 545: /* file_path: STRING  */
#line 4056 "parser.y"
        {
            (yyval.str_value) = (yyvsp[0].str_value);
        }
#line 9555 "parser.cpp"
        break;

        case 546: /* if_exists: IF EXISTS  */
#line 4060 "parser.y"
        {
            (yyval.bool_value) = true;
        }
#line 9561 "parser.cpp"
        break;

        case 547: /* if_exists: %empty  */
#line 4061 "parser.y"
        {
            (yyval.bool_value) = false;
        }
#line 9567 "parser.cpp"
        break;

        case 548: /* if_not_exists: IF NOT EXISTS  */
#line 4063 "parser.y"
        {
            (yyval.bool_value) = true;
        }
#line 9573 "parser.cpp"
        break;

        case 549: /* if_not_exists: %empty  */
#line 4064 "parser.y"
        {
            (yyval.bool_value) = false;
        }
#line 9579 "parser.cpp"
        break;

        case 552: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 4079 "parser.y"
        {
            (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
            (yyval.if_not_exists_info_t)->exists_ = true;
            (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 9592 "parser.cpp"
        break;

        case 553: /* if_not_exists_info: %empty  */
#line 4087 "parser.y"
        {
            (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
        }
#line 9600 "parser.cpp"
        break;

        case 554: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 4091 "parser.y"
        {
            (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
        }
#line 9608 "parser.cpp"
        break;

        case 555: /* with_index_param_list: %empty  */
#line 4094 "parser.y"
        {
            (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter *>();
        }
#line 9616 "parser.cpp"
        break;

        case 556: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 4098 "parser.y"
        {
            (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
        }
#line 9624 "parser.cpp"
        break;

        case 557: /* optional_table_properties_list: %empty  */
#line 4101 "parser.y"
        {
            (yyval.with_index_param_list_t) = nullptr;
        }
#line 9632 "parser.cpp"
        break;

        case 558: /* index_param_list: index_param  */
#line 4105 "parser.y"
        {
            (yyval.index_param_list_t) = new std::vector<infinity::InitParameter *>();
            (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
        }
#line 9641 "parser.cpp"
        break;

        case 559: /* index_param_list: index_param_list ',' index_param  */
#line 4109 "parser.y"
        {
            (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
            (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
        }
#line 9650 "parser.cpp"
        break;

        case 560: /* index_param: IDENTIFIER  */
#line 4114 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 9661 "parser.cpp"
        break;

        case 561: /* index_param: IDENTIFIER '=' IDENTIFIER  */
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
#line 9676 "parser.cpp"
        break;

        case 562: /* index_param: IDENTIFIER '=' STRING  */
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
#line 9691 "parser.cpp"
        break;

        case 563: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 4140 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));

            (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
        }
#line 9704 "parser.cpp"
        break;

        case 564: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 4148 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));

            (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
        }
#line 9717 "parser.cpp"
        break;

        case 565: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 4159 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            infinity::IndexType index_type = infinity::IndexType::kInvalid;
            if (strcmp((yyvsp[-1].str_value), "fulltext") == 0) {
                index_type = infinity::IndexType::kFullText;
            } else if (strcmp((yyvsp[-1].str_value), "hnsw") == 0) {
                index_type = infinity::IndexType::kHnsw;
            } else if (strcmp((yyvsp[-1].str_value), "bmp") == 0) {
                index_type = infinity::IndexType::kBMP;
            } else if (strcmp((yyvsp[-1].str_value), "ivf") == 0) {
                index_type = infinity::IndexType::kIVF;
            } else if (strcmp((yyvsp[-1].str_value), "emvb") == 0) {
                index_type = infinity::IndexType::kEMVB;
            } else if (strcmp((yyvsp[-1].str_value), "diskann") == 0) {
                index_type = infinity::IndexType::kDiskAnn;
            } else if (strcmp((yyvsp[-1].str_value), "secondary") == 0) {
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
#line 9775 "parser.cpp"
        break;

        case 566: /* index_info: '(' IDENTIFIER ')'  */
#line 4212 "parser.y"
        {
            (yyval.index_info_t) = new infinity::IndexInfo();
            (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
            free((yyvsp[-1].str_value));
        }
#line 9787 "parser.cpp"
        break;

#line 9791 "parser.cpp"

        default:
            break;
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
    YY_SYMBOL_PRINT("-> $$ =", YY_CAST(yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

    YYPOPSTACK(yylen);
    yylen = 0;

    *++yyvsp = yyval;
    *++yylsp = yyloc;

    /* Now 'shift' the result of the reduction.  Determine what state
       that goes to, based on the state we popped back to and the rule
       number reduced by.  */
    {
        const int yylhs = yyr1[yyn] - YYNTOKENS;
        const int yyi = yypgoto[yylhs] + *yyssp;
        yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp ? yytable[yyi] : yydefgoto[yylhs]);
    }

    goto yynewstate;

/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
    /* Make sure we have latest lookahead translation.  See comments at
       user semantic actions for why this is necessary.  */
    yytoken = yychar == SQLEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE(yychar);
    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus) {
        ++yynerrs;
        {
            yypcontext_t yyctx = {yyssp, yytoken, &yylloc};
            char const *yymsgp = YY_("syntax error");
            int yysyntax_error_status;
            yysyntax_error_status = yysyntax_error(&yymsg_alloc, &yymsg, &yyctx);
            if (yysyntax_error_status == 0)
                yymsgp = yymsg;
            else if (yysyntax_error_status == -1) {
                if (yymsg != yymsgbuf)
                    YYSTACK_FREE(yymsg);
                yymsg = YY_CAST(char *, YYSTACK_ALLOC(YY_CAST(YYSIZE_T, yymsg_alloc)));
                if (yymsg) {
                    yysyntax_error_status = yysyntax_error(&yymsg_alloc, &yymsg, &yyctx);
                    yymsgp = yymsg;
                } else {
                    yymsg = yymsgbuf;
                    yymsg_alloc = sizeof yymsgbuf;
                    yysyntax_error_status = YYENOMEM;
                }
            }
            yyerror(&yylloc, scanner, result, yymsgp);
            if (yysyntax_error_status == YYENOMEM)
                YYNOMEM;
        }
    }

    yyerror_range[1] = yylloc;
    if (yyerrstatus == 3) {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        if (yychar <= SQLEOF) {
            /* Return failure if at end of input.  */
            if (yychar == SQLEOF)
                YYABORT;
        } else {
            yydestruct("Error: discarding", yytoken, &yylval, &yylloc, scanner, result);
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
    YYPOPSTACK(yylen);
    yylen = 0;
    YY_STACK_PRINT(yyss, yyssp);
    yystate = *yyssp;
    goto yyerrlab1;

/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
    yyerrstatus = 3; /* Each real token shifted decrements this.  */

    /* Pop stack until we find a state that shifts the error token.  */
    for (;;) {
        yyn = yypact[yystate];
        if (!yypact_value_is_default(yyn)) {
            yyn += YYSYMBOL_YYerror;
            if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror) {
                yyn = yytable[yyn];
                if (0 < yyn)
                    break;
            }
        }

        /* Pop the current state because it cannot handle the error token.  */
        if (yyssp == yyss)
            YYABORT;

        yyerror_range[1] = *yylsp;
        yydestruct("Error: popping", YY_ACCESSING_SYMBOL(yystate), yyvsp, yylsp, scanner, result);
        YYPOPSTACK(1);
        yystate = *yyssp;
        YY_STACK_PRINT(yyss, yyssp);
    }

    YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
    *++yyvsp = yylval;
    YY_IGNORE_MAYBE_UNINITIALIZED_END

    yyerror_range[2] = yylloc;
    ++yylsp;
    YYLLOC_DEFAULT(*yylsp, yyerror_range, 2);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT("Shifting", YY_ACCESSING_SYMBOL(yyn), yyvsp, yylsp);

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
    yyerror(&yylloc, scanner, result, YY_("memory exhausted"));
    yyresult = 2;
    goto yyreturnlab;

/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
    if (yychar != SQLEMPTY) {
        /* Make sure we have latest lookahead translation.  See comments at
           user semantic actions for why this is necessary.  */
        yytoken = YYTRANSLATE(yychar);
        yydestruct("Cleanup: discarding lookahead", yytoken, &yylval, &yylloc, scanner, result);
    }
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    YYPOPSTACK(yylen);
    YY_STACK_PRINT(yyss, yyssp);
    while (yyssp != yyss) {
        yydestruct("Cleanup: popping", YY_ACCESSING_SYMBOL(+*yyssp), yyvsp, yylsp, scanner, result);
        YYPOPSTACK(1);
    }
#ifndef yyoverflow
    if (yyss != yyssa)
        YYSTACK_FREE(yyss);
#endif
    if (yymsg != yymsgbuf)
        YYSTACK_FREE(yymsg);
    return yyresult;
}

#line 4221 "parser.y"

void yyerror(YYLTYPE *llocp, void *lexer, infinity::ParserResult *result, const char *msg) {
    if (result->IsError())
        return;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
    fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

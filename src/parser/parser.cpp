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
#include <stdint.h>
#include <stdio.h>

void yyerror(YYLTYPE *llocp, void *lexer, infinity::ParserResult *result, const char *msg);

#line 87 "parser.cpp"

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
    YYSYMBOL_SYSTEM = 50,                                /* SYSTEM  */
    YYSYMBOL_GROUP = 51,                                 /* GROUP  */
    YYSYMBOL_BY = 52,                                    /* BY  */
    YYSYMBOL_HAVING = 53,                                /* HAVING  */
    YYSYMBOL_AS = 54,                                    /* AS  */
    YYSYMBOL_NATURAL = 55,                               /* NATURAL  */
    YYSYMBOL_JOIN = 56,                                  /* JOIN  */
    YYSYMBOL_LEFT = 57,                                  /* LEFT  */
    YYSYMBOL_RIGHT = 58,                                 /* RIGHT  */
    YYSYMBOL_OUTER = 59,                                 /* OUTER  */
    YYSYMBOL_FULL = 60,                                  /* FULL  */
    YYSYMBOL_ON = 61,                                    /* ON  */
    YYSYMBOL_INNER = 62,                                 /* INNER  */
    YYSYMBOL_CROSS = 63,                                 /* CROSS  */
    YYSYMBOL_DISTINCT = 64,                              /* DISTINCT  */
    YYSYMBOL_WHERE = 65,                                 /* WHERE  */
    YYSYMBOL_ORDER = 66,                                 /* ORDER  */
    YYSYMBOL_LIMIT = 67,                                 /* LIMIT  */
    YYSYMBOL_OFFSET = 68,                                /* OFFSET  */
    YYSYMBOL_ASC = 69,                                   /* ASC  */
    YYSYMBOL_DESC = 70,                                  /* DESC  */
    YYSYMBOL_IF = 71,                                    /* IF  */
    YYSYMBOL_NOT = 72,                                   /* NOT  */
    YYSYMBOL_EXISTS = 73,                                /* EXISTS  */
    YYSYMBOL_IN = 74,                                    /* IN  */
    YYSYMBOL_FROM = 75,                                  /* FROM  */
    YYSYMBOL_TO = 76,                                    /* TO  */
    YYSYMBOL_WITH = 77,                                  /* WITH  */
    YYSYMBOL_DELIMITER = 78,                             /* DELIMITER  */
    YYSYMBOL_FORMAT = 79,                                /* FORMAT  */
    YYSYMBOL_HEADER = 80,                                /* HEADER  */
    YYSYMBOL_HIGHLIGHT = 81,                             /* HIGHLIGHT  */
    YYSYMBOL_CAST = 82,                                  /* CAST  */
    YYSYMBOL_END = 83,                                   /* END  */
    YYSYMBOL_CASE = 84,                                  /* CASE  */
    YYSYMBOL_ELSE = 85,                                  /* ELSE  */
    YYSYMBOL_THEN = 86,                                  /* THEN  */
    YYSYMBOL_WHEN = 87,                                  /* WHEN  */
    YYSYMBOL_BOOLEAN = 88,                               /* BOOLEAN  */
    YYSYMBOL_INTEGER = 89,                               /* INTEGER  */
    YYSYMBOL_INT = 90,                                   /* INT  */
    YYSYMBOL_TINYINT = 91,                               /* TINYINT  */
    YYSYMBOL_SMALLINT = 92,                              /* SMALLINT  */
    YYSYMBOL_BIGINT = 93,                                /* BIGINT  */
    YYSYMBOL_HUGEINT = 94,                               /* HUGEINT  */
    YYSYMBOL_VARCHAR = 95,                               /* VARCHAR  */
    YYSYMBOL_FLOAT = 96,                                 /* FLOAT  */
    YYSYMBOL_DOUBLE = 97,                                /* DOUBLE  */
    YYSYMBOL_REAL = 98,                                  /* REAL  */
    YYSYMBOL_DECIMAL = 99,                               /* DECIMAL  */
    YYSYMBOL_DATE = 100,                                 /* DATE  */
    YYSYMBOL_TIME = 101,                                 /* TIME  */
    YYSYMBOL_DATETIME = 102,                             /* DATETIME  */
    YYSYMBOL_FLOAT16 = 103,                              /* FLOAT16  */
    YYSYMBOL_BFLOAT16 = 104,                             /* BFLOAT16  */
    YYSYMBOL_UNSIGNED = 105,                             /* UNSIGNED  */
    YYSYMBOL_TIMESTAMP = 106,                            /* TIMESTAMP  */
    YYSYMBOL_UUID = 107,                                 /* UUID  */
    YYSYMBOL_POINT = 108,                                /* POINT  */
    YYSYMBOL_LINE = 109,                                 /* LINE  */
    YYSYMBOL_LSEG = 110,                                 /* LSEG  */
    YYSYMBOL_BOX = 111,                                  /* BOX  */
    YYSYMBOL_PATH = 112,                                 /* PATH  */
    YYSYMBOL_POLYGON = 113,                              /* POLYGON  */
    YYSYMBOL_CIRCLE = 114,                               /* CIRCLE  */
    YYSYMBOL_BLOB = 115,                                 /* BLOB  */
    YYSYMBOL_BITMAP = 116,                               /* BITMAP  */
    YYSYMBOL_ARRAY = 117,                                /* ARRAY  */
    YYSYMBOL_TUPLE = 118,                                /* TUPLE  */
    YYSYMBOL_EMBEDDING = 119,                            /* EMBEDDING  */
    YYSYMBOL_VECTOR = 120,                               /* VECTOR  */
    YYSYMBOL_MULTIVECTOR = 121,                          /* MULTIVECTOR  */
    YYSYMBOL_TENSOR = 122,                               /* TENSOR  */
    YYSYMBOL_SPARSE = 123,                               /* SPARSE  */
    YYSYMBOL_TENSORARRAY = 124,                          /* TENSORARRAY  */
    YYSYMBOL_BIT = 125,                                  /* BIT  */
    YYSYMBOL_TEXT = 126,                                 /* TEXT  */
    YYSYMBOL_PRIMARY = 127,                              /* PRIMARY  */
    YYSYMBOL_KEY = 128,                                  /* KEY  */
    YYSYMBOL_UNIQUE = 129,                               /* UNIQUE  */
    YYSYMBOL_NULLABLE = 130,                             /* NULLABLE  */
    YYSYMBOL_IS = 131,                                   /* IS  */
    YYSYMBOL_DEFAULT = 132,                              /* DEFAULT  */
    YYSYMBOL_COMMENT = 133,                              /* COMMENT  */
    YYSYMBOL_IGNORE = 134,                               /* IGNORE  */
    YYSYMBOL_TRUE = 135,                                 /* TRUE  */
    YYSYMBOL_FALSE = 136,                                /* FALSE  */
    YYSYMBOL_INTERVAL = 137,                             /* INTERVAL  */
    YYSYMBOL_SECOND = 138,                               /* SECOND  */
    YYSYMBOL_SECONDS = 139,                              /* SECONDS  */
    YYSYMBOL_MINUTE = 140,                               /* MINUTE  */
    YYSYMBOL_MINUTES = 141,                              /* MINUTES  */
    YYSYMBOL_HOUR = 142,                                 /* HOUR  */
    YYSYMBOL_HOURS = 143,                                /* HOURS  */
    YYSYMBOL_DAY = 144,                                  /* DAY  */
    YYSYMBOL_DAYS = 145,                                 /* DAYS  */
    YYSYMBOL_MONTH = 146,                                /* MONTH  */
    YYSYMBOL_MONTHS = 147,                               /* MONTHS  */
    YYSYMBOL_YEAR = 148,                                 /* YEAR  */
    YYSYMBOL_YEARS = 149,                                /* YEARS  */
    YYSYMBOL_EQUAL = 150,                                /* EQUAL  */
    YYSYMBOL_NOT_EQ = 151,                               /* NOT_EQ  */
    YYSYMBOL_LESS_EQ = 152,                              /* LESS_EQ  */
    YYSYMBOL_GREATER_EQ = 153,                           /* GREATER_EQ  */
    YYSYMBOL_BETWEEN = 154,                              /* BETWEEN  */
    YYSYMBOL_AND = 155,                                  /* AND  */
    YYSYMBOL_OR = 156,                                   /* OR  */
    YYSYMBOL_EXTRACT = 157,                              /* EXTRACT  */
    YYSYMBOL_LIKE = 158,                                 /* LIKE  */
    YYSYMBOL_DATA = 159,                                 /* DATA  */
    YYSYMBOL_LOG = 160,                                  /* LOG  */
    YYSYMBOL_BUFFER = 161,                               /* BUFFER  */
    YYSYMBOL_TRANSACTIONS = 162,                         /* TRANSACTIONS  */
    YYSYMBOL_TRANSACTION = 163,                          /* TRANSACTION  */
    YYSYMBOL_MEMINDEX = 164,                             /* MEMINDEX  */
    YYSYMBOL_USING = 165,                                /* USING  */
    YYSYMBOL_SESSION = 166,                              /* SESSION  */
    YYSYMBOL_GLOBAL = 167,                               /* GLOBAL  */
    YYSYMBOL_OFF = 168,                                  /* OFF  */
    YYSYMBOL_EXPORT = 169,                               /* EXPORT  */
    YYSYMBOL_CONFIGS = 170,                              /* CONFIGS  */
    YYSYMBOL_CONFIG = 171,                               /* CONFIG  */
    YYSYMBOL_PROFILES = 172,                             /* PROFILES  */
    YYSYMBOL_VARIABLES = 173,                            /* VARIABLES  */
    YYSYMBOL_VARIABLE = 174,                             /* VARIABLE  */
    YYSYMBOL_LOGS = 175,                                 /* LOGS  */
    YYSYMBOL_CATALOGS = 176,                             /* CATALOGS  */
    YYSYMBOL_CATALOG = 177,                              /* CATALOG  */
    YYSYMBOL_SEARCH = 178,                               /* SEARCH  */
    YYSYMBOL_MATCH = 179,                                /* MATCH  */
    YYSYMBOL_MAXSIM = 180,                               /* MAXSIM  */
    YYSYMBOL_QUERY = 181,                                /* QUERY  */
    YYSYMBOL_QUERIES = 182,                              /* QUERIES  */
    YYSYMBOL_FUSION = 183,                               /* FUSION  */
    YYSYMBOL_ROWLIMIT = 184,                             /* ROWLIMIT  */
    YYSYMBOL_ADMIN = 185,                                /* ADMIN  */
    YYSYMBOL_LEADER = 186,                               /* LEADER  */
    YYSYMBOL_FOLLOWER = 187,                             /* FOLLOWER  */
    YYSYMBOL_LEARNER = 188,                              /* LEARNER  */
    YYSYMBOL_CONNECT = 189,                              /* CONNECT  */
    YYSYMBOL_STANDALONE = 190,                           /* STANDALONE  */
    YYSYMBOL_NODES = 191,                                /* NODES  */
    YYSYMBOL_NODE = 192,                                 /* NODE  */
    YYSYMBOL_REMOVE = 193,                               /* REMOVE  */
    YYSYMBOL_SNAPSHOT = 194,                             /* SNAPSHOT  */
    YYSYMBOL_SNAPSHOTS = 195,                            /* SNAPSHOTS  */
    YYSYMBOL_RECOVER = 196,                              /* RECOVER  */
    YYSYMBOL_RESTORE = 197,                              /* RESTORE  */
    YYSYMBOL_PERSISTENCE = 198,                          /* PERSISTENCE  */
    YYSYMBOL_OBJECT = 199,                               /* OBJECT  */
    YYSYMBOL_OBJECTS = 200,                              /* OBJECTS  */
    YYSYMBOL_FILES = 201,                                /* FILES  */
    YYSYMBOL_MEMORY = 202,                               /* MEMORY  */
    YYSYMBOL_ALLOCATION = 203,                           /* ALLOCATION  */
    YYSYMBOL_HISTORY = 204,                              /* HISTORY  */
    YYSYMBOL_CHECK = 205,                                /* CHECK  */
    YYSYMBOL_CLEAN = 206,                                /* CLEAN  */
    YYSYMBOL_NUMBER = 207,                               /* NUMBER  */
    YYSYMBOL_208_ = 208,                                 /* '='  */
    YYSYMBOL_209_ = 209,                                 /* '<'  */
    YYSYMBOL_210_ = 210,                                 /* '>'  */
    YYSYMBOL_211_ = 211,                                 /* '+'  */
    YYSYMBOL_212_ = 212,                                 /* '-'  */
    YYSYMBOL_213_ = 213,                                 /* '*'  */
    YYSYMBOL_214_ = 214,                                 /* '/'  */
    YYSYMBOL_215_ = 215,                                 /* '%'  */
    YYSYMBOL_216_ = 216,                                 /* '['  */
    YYSYMBOL_217_ = 217,                                 /* ']'  */
    YYSYMBOL_218_ = 218,                                 /* '('  */
    YYSYMBOL_219_ = 219,                                 /* ')'  */
    YYSYMBOL_220_ = 220,                                 /* '.'  */
    YYSYMBOL_221_ = 221,                                 /* ';'  */
    YYSYMBOL_222_ = 222,                                 /* ','  */
    YYSYMBOL_223_ = 223,                                 /* '}'  */
    YYSYMBOL_224_ = 224,                                 /* '{'  */
    YYSYMBOL_225_ = 225,                                 /* ':'  */
    YYSYMBOL_YYACCEPT = 226,                             /* $accept  */
    YYSYMBOL_input_pattern = 227,                        /* input_pattern  */
    YYSYMBOL_statement_list = 228,                       /* statement_list  */
    YYSYMBOL_statement = 229,                            /* statement  */
    YYSYMBOL_explainable_statement = 230,                /* explainable_statement  */
    YYSYMBOL_create_statement = 231,                     /* create_statement  */
    YYSYMBOL_table_element_array = 232,                  /* table_element_array  */
    YYSYMBOL_column_def_array = 233,                     /* column_def_array  */
    YYSYMBOL_table_element = 234,                        /* table_element  */
    YYSYMBOL_table_column = 235,                         /* table_column  */
    YYSYMBOL_column_type_array = 236,                    /* column_type_array  */
    YYSYMBOL_column_type = 237,                          /* column_type  */
    YYSYMBOL_column_constraints = 238,                   /* column_constraints  */
    YYSYMBOL_column_constraint = 239,                    /* column_constraint  */
    YYSYMBOL_default_expr = 240,                         /* default_expr  */
    YYSYMBOL_table_constraint = 241,                     /* table_constraint  */
    YYSYMBOL_identifier_array = 242,                     /* identifier_array  */
    YYSYMBOL_delete_statement = 243,                     /* delete_statement  */
    YYSYMBOL_insert_statement = 244,                     /* insert_statement  */
    YYSYMBOL_optional_identifier_array = 245,            /* optional_identifier_array  */
    YYSYMBOL_explain_statement = 246,                    /* explain_statement  */
    YYSYMBOL_update_statement = 247,                     /* update_statement  */
    YYSYMBOL_update_expr_array = 248,                    /* update_expr_array  */
    YYSYMBOL_update_expr = 249,                          /* update_expr  */
    YYSYMBOL_drop_statement = 250,                       /* drop_statement  */
    YYSYMBOL_copy_statement = 251,                       /* copy_statement  */
    YYSYMBOL_select_statement = 252,                     /* select_statement  */
    YYSYMBOL_select_with_paren = 253,                    /* select_with_paren  */
    YYSYMBOL_select_without_paren = 254,                 /* select_without_paren  */
    YYSYMBOL_select_clause_with_modifier = 255,          /* select_clause_with_modifier  */
    YYSYMBOL_select_clause_without_modifier_paren = 256, /* select_clause_without_modifier_paren  */
    YYSYMBOL_select_clause_without_modifier = 257,       /* select_clause_without_modifier  */
    YYSYMBOL_order_by_clause = 258,                      /* order_by_clause  */
    YYSYMBOL_order_by_expr_list = 259,                   /* order_by_expr_list  */
    YYSYMBOL_order_by_expr = 260,                        /* order_by_expr  */
    YYSYMBOL_order_by_type = 261,                        /* order_by_type  */
    YYSYMBOL_limit_expr = 262,                           /* limit_expr  */
    YYSYMBOL_offset_expr = 263,                          /* offset_expr  */
    YYSYMBOL_distinct = 264,                             /* distinct  */
    YYSYMBOL_highlight_clause = 265,                     /* highlight_clause  */
    YYSYMBOL_from_clause = 266,                          /* from_clause  */
    YYSYMBOL_search_clause = 267,                        /* search_clause  */
    YYSYMBOL_optional_search_filter_expr = 268,          /* optional_search_filter_expr  */
    YYSYMBOL_where_clause = 269,                         /* where_clause  */
    YYSYMBOL_having_clause = 270,                        /* having_clause  */
    YYSYMBOL_group_by_clause = 271,                      /* group_by_clause  */
    YYSYMBOL_set_operator = 272,                         /* set_operator  */
    YYSYMBOL_table_reference = 273,                      /* table_reference  */
    YYSYMBOL_table_reference_unit = 274,                 /* table_reference_unit  */
    YYSYMBOL_table_reference_name = 275,                 /* table_reference_name  */
    YYSYMBOL_table_name = 276,                           /* table_name  */
    YYSYMBOL_table_alias = 277,                          /* table_alias  */
    YYSYMBOL_with_clause = 278,                          /* with_clause  */
    YYSYMBOL_with_expr_list = 279,                       /* with_expr_list  */
    YYSYMBOL_with_expr = 280,                            /* with_expr  */
    YYSYMBOL_join_clause = 281,                          /* join_clause  */
    YYSYMBOL_join_type = 282,                            /* join_type  */
    YYSYMBOL_show_statement = 283,                       /* show_statement  */
    YYSYMBOL_flush_statement = 284,                      /* flush_statement  */
    YYSYMBOL_optimize_statement = 285,                   /* optimize_statement  */
    YYSYMBOL_command_statement = 286,                    /* command_statement  */
    YYSYMBOL_compact_statement = 287,                    /* compact_statement  */
    YYSYMBOL_admin_statement = 288,                      /* admin_statement  */
    YYSYMBOL_alter_statement = 289,                      /* alter_statement  */
    YYSYMBOL_check_statement = 290,                      /* check_statement  */
    YYSYMBOL_expr_array = 291,                           /* expr_array  */
    YYSYMBOL_insert_row_list = 292,                      /* insert_row_list  */
    YYSYMBOL_expr_alias = 293,                           /* expr_alias  */
    YYSYMBOL_expr = 294,                                 /* expr  */
    YYSYMBOL_operand = 295,                              /* operand  */
    YYSYMBOL_match_tensor_expr = 296,                    /* match_tensor_expr  */
    YYSYMBOL_match_vector_expr = 297,                    /* match_vector_expr  */
    YYSYMBOL_match_sparse_expr = 298,                    /* match_sparse_expr  */
    YYSYMBOL_match_text_expr = 299,                      /* match_text_expr  */
    YYSYMBOL_query_expr = 300,                           /* query_expr  */
    YYSYMBOL_fusion_expr = 301,                          /* fusion_expr  */
    YYSYMBOL_sub_search = 302,                           /* sub_search  */
    YYSYMBOL_sub_search_array = 303,                     /* sub_search_array  */
    YYSYMBOL_function_expr = 304,                        /* function_expr  */
    YYSYMBOL_conjunction_expr = 305,                     /* conjunction_expr  */
    YYSYMBOL_between_expr = 306,                         /* between_expr  */
    YYSYMBOL_in_expr = 307,                              /* in_expr  */
    YYSYMBOL_case_expr = 308,                            /* case_expr  */
    YYSYMBOL_case_check_array = 309,                     /* case_check_array  */
    YYSYMBOL_cast_expr = 310,                            /* cast_expr  */
    YYSYMBOL_subquery_expr = 311,                        /* subquery_expr  */
    YYSYMBOL_column_expr = 312,                          /* column_expr  */
    YYSYMBOL_constant_expr = 313,                        /* constant_expr  */
    YYSYMBOL_common_array_expr = 314,                    /* common_array_expr  */
    YYSYMBOL_common_sparse_array_expr = 315,             /* common_sparse_array_expr  */
    YYSYMBOL_subarray_array_expr = 316,                  /* subarray_array_expr  */
    YYSYMBOL_unclosed_subarray_array_expr = 317,         /* unclosed_subarray_array_expr  */
    YYSYMBOL_sparse_array_expr = 318,                    /* sparse_array_expr  */
    YYSYMBOL_long_sparse_array_expr = 319,               /* long_sparse_array_expr  */
    YYSYMBOL_unclosed_long_sparse_array_expr = 320,      /* unclosed_long_sparse_array_expr  */
    YYSYMBOL_double_sparse_array_expr = 321,             /* double_sparse_array_expr  */
    YYSYMBOL_unclosed_double_sparse_array_expr = 322,    /* unclosed_double_sparse_array_expr  */
    YYSYMBOL_empty_array_expr = 323,                     /* empty_array_expr  */
    YYSYMBOL_curly_brackets_expr = 324,                  /* curly_brackets_expr  */
    YYSYMBOL_unclosed_curly_brackets_expr = 325,         /* unclosed_curly_brackets_expr  */
    YYSYMBOL_int_sparse_ele = 326,                       /* int_sparse_ele  */
    YYSYMBOL_float_sparse_ele = 327,                     /* float_sparse_ele  */
    YYSYMBOL_array_expr = 328,                           /* array_expr  */
    YYSYMBOL_long_array_expr = 329,                      /* long_array_expr  */
    YYSYMBOL_unclosed_long_array_expr = 330,             /* unclosed_long_array_expr  */
    YYSYMBOL_double_array_expr = 331,                    /* double_array_expr  */
    YYSYMBOL_unclosed_double_array_expr = 332,           /* unclosed_double_array_expr  */
    YYSYMBOL_interval_expr = 333,                        /* interval_expr  */
    YYSYMBOL_copy_option_list = 334,                     /* copy_option_list  */
    YYSYMBOL_copy_option = 335,                          /* copy_option  */
    YYSYMBOL_file_path = 336,                            /* file_path  */
    YYSYMBOL_if_exists = 337,                            /* if_exists  */
    YYSYMBOL_if_not_exists = 338,                        /* if_not_exists  */
    YYSYMBOL_semicolon = 339,                            /* semicolon  */
    YYSYMBOL_if_not_exists_info = 340,                   /* if_not_exists_info  */
    YYSYMBOL_with_index_param_list = 341,                /* with_index_param_list  */
    YYSYMBOL_optional_table_properties_list = 342,       /* optional_table_properties_list  */
    YYSYMBOL_index_param_list = 343,                     /* index_param_list  */
    YYSYMBOL_index_param = 344,                          /* index_param  */
    YYSYMBOL_index_info = 345                            /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;

/* Unqualified %code blocks.  */
#line 97 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 474 "parser.cpp"

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
#define YYFINAL 131
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST 1602

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS 226
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS 120
/* YYNRULES -- Number of rules.  */
#define YYNRULES 550
/* YYNSTATES -- Number of states.  */
#define YYNSTATES 1254

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK 462

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX) (0 <= (YYX) && (YYX) <= YYMAXUTOK ? YY_CAST(yysymbol_kind_t, yytranslate[YYX]) : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] = {
    0,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   215, 2,   2,   218, 219, 213, 211, 222, 212, 220, 214, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    225, 221, 209, 208, 210, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   216, 2,   217, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   224, 2,   223, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,   2,   3,   4,   5,
    6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,
    35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
    64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,
    93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
    122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150,
    151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
    180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] = {
    0,    519,  519,  523,  529,  536,  537,  538,  539,  540,  541,  542,  543,  544,  545,  546,  547,  548,  549,  550,  551,  553,  554,  555,
    556,  557,  558,  559,  560,  561,  562,  563,  564,  571,  588,  605,  621,  650,  665,  697,  715,  733,  761,  792,  796,  801,  805,  811,
    814,  821,  841,  863,  887,  913,  917,  923,  924,  925,  926,  927,  928,  929,  930,  931,  932,  933,  934,  935,  936,  937,  938,  939,
    940,  941,  942,  943,  946,  948,  949,  950,  951,  954,  955,  956,  957,  958,  959,  960,  961,  962,  963,  964,  965,  966,  967,  968,
    969,  970,  971,  972,  973,  974,  975,  976,  977,  978,  979,  980,  981,  982,  983,  984,  985,  986,  987,  988,  989,  990,  991,  992,
    993,  994,  995,  996,  997,  998,  999,  1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1010, 1011, 1012, 1013, 1014, 1015, 1016,
    1017, 1018, 1019, 1020, 1024, 1047, 1051, 1061, 1064, 1067, 1070, 1074, 1077, 1082, 1087, 1094, 1100, 1110, 1126, 1164, 1180, 1183, 1190, 1207,
    1216, 1229, 1233, 1238, 1251, 1264, 1279, 1294, 1309, 1332, 1385, 1440, 1491, 1494, 1497, 1506, 1516, 1519, 1523, 1528, 1555, 1558, 1563, 1580,
    1583, 1587, 1591, 1596, 1602, 1605, 1608, 1612, 1616, 1618, 1622, 1624, 1627, 1631, 1634, 1638, 1641, 1645, 1650, 1654, 1657, 1661, 1664, 1668,
    1671, 1675, 1678, 1682, 1685, 1688, 1691, 1699, 1702, 1717, 1717, 1719, 1733, 1742, 1747, 1756, 1761, 1766, 1772, 1779, 1782, 1786, 1789, 1794,
    1806, 1813, 1827, 1830, 1833, 1836, 1839, 1842, 1845, 1851, 1855, 1859, 1863, 1867, 1874, 1878, 1882, 1886, 1890, 1895, 1899, 1904, 1908, 1912,
    1916, 1922, 1928, 1934, 1945, 1956, 1967, 1979, 1991, 2004, 2018, 2029, 2043, 2059, 2076, 2080, 2084, 2090, 2094, 2098, 2104, 2108, 2112, 2116,
    2122, 2126, 2136, 2140, 2144, 2148, 2156, 2167, 2190, 2196, 2201, 2207, 2213, 2221, 2227, 2233, 2239, 2245, 2253, 2259, 2265, 2271, 2277, 2285,
    2291, 2297, 2305, 2313, 2319, 2325, 2331, 2337, 2341, 2355, 2368, 2374, 2381, 2389, 2398, 2408, 2418, 2429, 2440, 2452, 2464, 2474, 2485, 2497,
    2510, 2514, 2519, 2524, 2530, 2534, 2538, 2544, 2548, 2552, 2558, 2564, 2572, 2578, 2582, 2588, 2592, 2598, 2603, 2608, 2615, 2624, 2634, 2643,
    2655, 2667, 2671, 2687, 2691, 2696, 2706, 2728, 2734, 2738, 2739, 2740, 2741, 2742, 2744, 2747, 2753, 2756, 2757, 2758, 2759, 2760, 2761, 2762,
    2763, 2764, 2765, 2769, 2785, 2802, 2820, 2877, 2927, 2981, 3039, 3063, 3086, 3107, 3128, 3137, 3148, 3159, 3173, 3180, 3190, 3196, 3208, 3211,
    3214, 3217, 3220, 3223, 3227, 3231, 3236, 3244, 3252, 3261, 3268, 3275, 3282, 3289, 3296, 3303, 3310, 3317, 3324, 3331, 3338, 3346, 3354, 3362,
    3370, 3378, 3386, 3394, 3402, 3410, 3418, 3426, 3434, 3464, 3472, 3481, 3489, 3498, 3506, 3512, 3519, 3525, 3532, 3537, 3544, 3551, 3559, 3572,
    3578, 3584, 3591, 3599, 3606, 3613, 3618, 3628, 3633, 3638, 3643, 3648, 3653, 3658, 3663, 3668, 3673, 3676, 3679, 3682, 3686, 3689, 3692, 3695,
    3699, 3702, 3705, 3709, 3713, 3718, 3723, 3726, 3730, 3734, 3741, 3748, 3752, 3759, 3766, 3770, 3773, 3777, 3781, 3786, 3790, 3794, 3797, 3801,
    3805, 3810, 3815, 3819, 3824, 3829, 3835, 3841, 3847, 3853, 3859, 3865, 3871, 3877, 3883, 3889, 3895, 3906, 3910, 3915, 3946, 3956, 3961, 3966,
    3971, 3977, 3981, 3982, 3984, 3985, 3987, 3988, 4000, 4008, 4012, 4015, 4019, 4022, 4026, 4030, 4035, 4041, 4051, 4061, 4069, 4080, 4111};
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
                                      "PERSISTENCE",
                                      "OBJECT",
                                      "OBJECTS",
                                      "FILES",
                                      "MEMORY",
                                      "ALLOCATION",
                                      "HISTORY",
                                      "CHECK",
                                      "CLEAN",
                                      "NUMBER",
                                      "'='",
                                      "'<'",
                                      "'>'",
                                      "'+'",
                                      "'-'",
                                      "'*'",
                                      "'/'",
                                      "'%'",
                                      "'['",
                                      "']'",
                                      "'('",
                                      "')'",
                                      "'.'",
                                      "';'",
                                      "','",
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

#define YYPACT_NINF (-759)

#define yypact_value_is_default(Yyn) ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-538)

#define yytable_value_is_error(Yyn) ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] = {
    365,  151,  64,   290,  105,  65,   105,  322,  241,  846,  149,  168,  169,  301,  232,  328,  332,  218,  120,  293,  199,  237,  -53,  411,
    220,  -759, -759, -759, -759, -759, -759, -759, -759, 430,  -759, -759, 441,  -759, -759, -759, -759, -759, -759, -759, -759, 387,  387,  387,
    387,  36,   460,  105,  398,  398,  398,  398,  398,  478,  272,  494,  105,  -31,  495,  509,  518,  818,  -759, -759, -759, -759, -759, -759,
    -759, 430,  -759, -759, -759, -759, -759, 309,  534,  105,  -759, -759, -759, -759, -759, 9,    -759, 392,  444,  -759, 541,  -759, -759, 470,
    546,  -759, 558,  -759, 400,  -140, 105,  105,  573,  -759, -759, -759, -759, -759, -29,  -759, 533,  359,  -759, 589,  447,  449,  333,  527,
    463,  626,  486,  601,  497,  517,  105,  -759, -759, 461,  508,  -759, 76,   -759, 708,  -759, -759, 18,   667,  -759, 669,  661,  736,  105,
    105,  105,  743,  692,  696,  550,  686,  769,  105,  105,  105,  780,  -759, 792,  800,  746,  813,  813,  588,  35,   260,  298,  -759, 600,
    -759, 516,  -759, -759, -759, 820,  -759, 830,  -759, 813,  -759, -759, 817,  -759, -759, -759, -759, 337,  -759, 797,  105,  653,  332,  813,
    -759, 847,  -759, 707,  -759, 867,  -759, -759, 871,  -759, 869,  -759, 872,  874,  -759, 876,  831,  888,  699,  900,  904,  -759, -759, -759,
    -759, -759, 18,   -759, -759, -759, 588,  854,  841,  836,  777,  -37,  -759, 550,  -759, 105,  422,  911,  109,  -759, -759, -759, -759, -759,
    855,  -759, 713,  -30,  -759, 588,  -759, -759, 838,  848,  705,  -759, -759, 1093, 678,  709,  711,  206,  927,  928,  936,  939,  -759, -759,
    938,  727,  728,  729,  733,  735,  738,  739,  532,  740,  741,  782,  782,  -759, 17,   496,  14,   -144, -759, -8,   454,  -759, -759, -759,
    -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, 742,  -759, -759, -759, 98,   -759, -759, 248,  -759, 303,  -759, -759, 388,  -759,
    -759, 313,  -759, 331,  -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, 951,  949,  -759,
    -759, -759, -759, -759, -759, -759, 914,  918,  890,  105,  891,  441,  -759, -759, -759, 963,  62,   -759, 965,  -759, -759, 893,  438,  -759,
    968,  -759, -759, 755,  757,  -34,  588,  588,  905,  -759, 973,  -53,  50,   924,  761,  977,  978,  -759, -759, 226,  764,  -759, 105,  588,
    800,  -759, 543,  765,  767,  403,  -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, 782,  768,  912,  913,  588,  588,
    243,  266,  -759, -759, -759, -759, 1093, -759, 588,  588,  588,  588,  588,  588,  984,  771,  773,  774,  778,  993,  995,  502,  502,  -759,
    776,  -759, -759, -759, -759, 783,  -106, -759, -759, 929,  588,  1000, 588,  588,  -17,  786,  22,   782,  782,  782,  782,  782,  782,  782,
    782,  782,  782,  782,  782,  782,  782,  30,   -759, 798,  -759, 1013, -759, 1014, 23,   -759, -759, 1016, -759, 1020, 987,  693,  819,  821,
    1023, -759, 824,  -759, 816,  -759, 1039, -759, 166,  1042, 882,  884,  -759, -759, -759, 588,  975,  829,  -759, 11,   543,  588,  -759, -759,
    108,  1049, 931,  839,  261,  -759, -759, -759, -53,  1057, 933,  -759, -759, -759, 1064, 588,  853,  -759, 543,  -759, 217,  217,  588,  -759,
    314,  913,  910,  850,  8,    213,  492,  -759, 588,  588,  201,  249,  258,  271,  276,  288,  1001, 588,  33,   588,  1073, 856,  335,  680,
    -759, -759, 813,  -759, -759, -759, 925,  863,  782,  496,  952,  -759, 921,  921,  436,  436,  903,  921,  921,  436,  436,  502,  502,  -759,
    -759, -759, -759, -759, -759, 858,  -759, 859,  -759, -759, -759, -759, 1079, 1080, -759, 911,  1084, -759, 1094, -759, -759, 1090, -759, -759,
    1096, 1097, 881,  13,   920,  588,  -759, -759, -759, 543,  1098, -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, 886,  -759,
    -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, 887,  889,  892,  901,  943,  944,  946,  947,  402,  961,  911,  1075, 50,
    430,  909,  1104, -759, 437,  962,  1103, 1148, 1179, 1181, -759, 1180, 452,  -759, 473,  474,  -759, 966,  -759, 1049, 588,  -759, 588,  -11,
    228,  -759, -759, -759, -759, -759, -759, 782,  -85,  -759, 97,   178,  974,  44,   981,  -759, 1183, -759, -759, 1120, 496,  921,  982,  484,
    -759, 782,  1196, 1198, 1155, 1162, 491,  530,  -759, 1003, 539,  -759, 1203, -759, -759, -53,  990,  734,  -759, 63,   -759, 456,  746,  -759,
    -759, 1207, 1049, 1049, 651,  808,  845,  1085, 1102, 1125, 1089, 1092, -759, -759, 384,  -759, 1091, 911,  551,  1006, 1110, -759, 1060, -759,
    -759, 588,  -759, -759, -759, -759, -759, -759, 217,  -759, -759, -759, 1007, 543,  131,  -759, 588,  880,  588,  798,  1028, 1241, 1024, 588,
    -759, 1029, 1031, 1032, 555,  -759, -759, 912,  1246, 1247, -759, -759, 1084, 760,  -759, 1094, 338,  57,   13,   1191, -759, -759, -759, -759,
    -759, -759, 1199, -759, 1251, -759, -759, -759, -759, -759, -759, -759, -759, 1034, 1206, 583,  1040, 621,  -759, 1036, 1038, 1041, 1043, 1044,
    1045, 1046, 1047, 1048, 1170, 1050, 1051, 1052, 1053, 1054, 1055, 1056, 1058, 1059, 1061, 1171, 1062, 1063, 1065, 1066, 1067, 1068, 1069, 1070,
    1071, 1072, 1173, 1074, 1076, 1077, 1078, 1081, 1082, 1083, 1086, 1087, 1088, 1188, 1095, 1099, 1100, 1101, 1105, 1106, 1107, 1108, 1109, 1111,
    1195, 1112, 1113, 1114, 1115, 1116, 1117, 1118, 1119, 1121, 1122, 1204, 1123, -759, -759, 23,   -759, 1138, 1149, 623,  -759, 1094, 1293, 1298,
    631,  -759, -759, -759, 543,  -759, 181,  1124, 25,   1126, -759, -759, -759, 1127, 1237, 1128, 543,  -759, 217,  -759, -759, -759, -759, -759,
    -759, -759, -759, -759, -759, 1300, -759, 63,   734,  13,   13,   1132, 456,  1255, 1258, -759, 1306, -759, -759, 1049, 1307, 1308, 1309, 1310,
    1312, 1313, 1314, 1318, 1319, 1129, 1320, 1326, 1336, 1346, 1347, 1348, 1349, 1350, 1351, 1352, 1137, 1354, 1355, 1356, 1357, 1358, 1359, 1360,
    1361, 1362, 1363, 1150, 1364, 1365, 1367, 1368, 1369, 1370, 1371, 1372, 1373, 1374, 1159, 1376, 1377, 1378, 1379, 1380, 1381, 1382, 1383, 1384,
    1385, 1172, 1386, 1387, 1389, 1390, 1391, 1392, 1393, 1394, 1395, 1396, 1182, 1397, -759, 1401, 1402, -759, 633,  -759, 891,  -759, 1403, 1404,
    1405, 115,  1192, -759, 641,  1406, -759, -759, 1353, 911,  -759, 588,  588,  -759, 1194, -759, 1197, 1200, 1201, 1202, 1205, 1208, 1209, 1210,
    1211, 1409, 1212, 1213, 1214, 1215, 1216, 1217, 1218, 1219, 1220, 1221, 1411, 1222, 1223, 1224, 1225, 1226, 1227, 1228, 1229, 1230, 1231, 1412,
    1232, 1233, 1234, 1235, 1236, 1238, 1239, 1240, 1242, 1243, 1416, 1244, 1245, 1248, 1249, 1250, 1252, 1253, 1254, 1256, 1257, 1417, 1259, 1260,
    1261, 1262, 1263, 1264, 1265, 1266, 1267, 1268, 1419, 1269, -759, -759, -759, -759, 1270, 1271, 1272, 1024, 1291, -759, 581,  588,  643,  881,
    543,  -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, 1276, -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, 1277, -759,
    -759, -759, -759, -759, -759, -759, -759, -759, -759, 1278, -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, 1279, -759, -759, -759,
    -759, -759, -759, -759, -759, -759, -759, 1280, -759, -759, -759, -759, -759, -759, -759, -759, -759, -759, 1281, -759, 1422, 1456, 92,   1282,
    1283, 1418, 1459, 1464, -759, -759, -759, 543,  -759, -759, -759, -759, -759, -759, -759, 1284, 1285, 1024, 891,  1324, 1273, 1468, 737,  305,
    1286, 1473, 1289, -759, 1442, 1499, 825,  1498, -759, 1024, 891,  1024, -23,  1292, 1290, 1505, -759, 1465, 1294, -759, 1295, 1476, 1477, -759,
    1513, -759, -759, -759, 5,    -39,  -759, 1301, 1299, 1481, 1482, -759, 1483, 1484, 1407, -759, -759, 1311, -759, 1315, 1305, 1522, 1523, 891,
    1316, 1317, -759, 891,  -759, -759};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] = {
    236, 0,   0,   0,   0,   0,   0,   0,   236, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   236, 0,   535, 3,   5,   10,  12,
    13,  11,  6,   7,   9,   181, 180, 0,   8,   14,  15,  16,  17,  18,  19,  20,  533, 533, 533, 533, 533, 0,   0,   531, 531, 531, 531, 531, 0,
    229, 0,   0,   0,   0,   0,   0,   236, 167, 21,  26,  28,  27,  22,  23,  25,  24,  29,  30,  31,  32,  0,   0,   0,   250, 251, 249, 255, 259,
    0,   256, 0,   0,   252, 0,   254, 278, 279, 0,   257, 0,   288, 0,   284, 0,   0,   0,   290, 291, 292, 293, 296, 229, 294, 0,   235, 237, 0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   361, 319, 0,   0,   1,   236, 2,   219, 221, 222, 0,   204, 186, 192, 0,   0,   0,   0,
    0,   0,   0,   0,   165, 0,   0,   0,   0,   0,   0,   316, 0,   0,   214, 0,   0,   0,   0,   0,   0,   166, 0,   266, 267, 260, 261, 262, 0,
    263, 0,   253, 0,   258, 289, 0,   282, 281, 285, 286, 0,   321, 0,   0,   0,   0,   0,   343, 0,   353, 0,   354, 0,   340, 341, 0,   336, 0,
    349, 351, 0,   344, 0,   0,   0,   0,   0,   0,   362, 185, 184, 4,   220, 0,   182, 183, 203, 0,   0,   200, 0,   34,  0,   35,  165, 536, 0,
    0,   0,   236, 530, 172, 174, 173, 175, 0,   230, 0,   214, 169, 0,   161, 529, 0,   0,   459, 463, 466, 467, 0,   0,   0,   0,   0,   0,   0,
    0,   464, 465, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   461, 0,   236, 0,   0,   363, 368, 369, 383, 381, 384, 382, 385,
    386, 378, 373, 372, 371, 379, 380, 370, 377, 376, 474, 477, 0,   478, 486, 0,   487, 0,   479, 475, 0,   476, 501, 0,   502, 0,   473, 300, 302,
    301, 298, 299, 305, 307, 306, 303, 304, 310, 312, 311, 308, 309, 287, 0,   0,   269, 268, 274, 264, 265, 280, 283, 0,   0,   0,   0,   539, 0,
    238, 297, 346, 0,   337, 342, 0,   350, 345, 0,   0,   352, 0,   317, 318, 0,   0,   206, 0,   0,   202, 532, 0,   236, 0,   0,   0,   0,   0,
    315, 159, 0,   0,   163, 0,   0,   0,   168, 213, 0,   0,   0,   510, 509, 512, 511, 514, 513, 516, 515, 518, 517, 520, 519, 0,   0,   425, 236,
    0,   0,   0,   0,   468, 469, 470, 471, 0,   472, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   427, 426, 507, 504, 494, 484,
    489, 492, 0,   0,   496, 497, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    483, 0,   488, 0,   491, 0,   0,   495, 503, 0,   506, 0,   275, 270, 0,   0,   0,   320, 0,   295, 0,   355, 0,   338, 0,   0,   0,   0,   348,
    189, 188, 0,   208, 191, 193, 198, 199, 0,   187, 33,  37,  0,   0,   0,   0,   43,  47,  48,  236, 0,   41,  314, 313, 164, 0,   0,   162, 176,
    171, 170, 0,   0,   0,   414, 0,   236, 0,   0,   0,   0,   0,   450, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   212, 0,
    0,   375, 374, 0,   364, 367, 443, 444, 0,   0,   236, 0,   424, 434, 435, 438, 439, 0,   441, 433, 436, 437, 429, 428, 430, 431, 432, 460, 462,
    485, 0,   490, 0,   493, 498, 505, 508, 0,   0,   271, 0,   0,   358, 0,   239, 339, 0,   322, 347, 0,   0,   205, 0,   210, 0,   196, 197, 195,
    201, 0,   55,  58,  59,  56,  57,  60,  61,  77,  62,  64,  63,  80,  67,  68,  69,  65,  66,  70,  71,  72,  73,  74,  75,  76,  0,   0,   0,
    0,   0,   0,   0,   0,   539, 0,   0,   541, 0,   40,  0,   0,   160, 0,   0,   0,   0,   0,   0,   525, 0,   0,   521, 0,   0,   415, 0,   455,
    0,   0,   448, 0,   0,   0,   422, 421, 420, 419, 418, 417, 0,   0,   459, 0,   0,   0,   0,   0,   404, 0,   500, 499, 0,   236, 442, 0,   0,
    423, 0,   0,   0,   276, 272, 0,   0,   45,  544, 0,   542, 323, 356, 357, 236, 207, 223, 225, 234, 226, 0,   214, 194, 39,  0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   152, 153, 156, 149, 156, 0,   0,   0,   36,  44,  550, 42,  365, 0,   527, 526, 524, 523, 528, 179, 0,   177,
    416, 456, 0,   452, 0,   451, 0,   0,   0,   0,   0,   0,   212, 0,   402, 0,   0,   0,   0,   457, 446, 445, 0,   0,   360, 359, 0,   0,   538,
    0,   0,   0,   0,   0,   243, 244, 245, 246, 242, 247, 0,   232, 0,   227, 408, 406, 409, 407, 410, 411, 412, 209, 218, 0,   0,   0,   53,  0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   154, 151, 0,   150, 50,  49,  0,   158, 0,   0,   0,   0,   522, 454, 449, 453, 440, 0,   0,   0,   0,   480,
    482, 481, 212, 0,   0,   211, 405, 0,   458, 447, 277, 273, 46,  545, 546, 548, 547, 543, 0,   324, 234, 224, 0,   0,   231, 0,   0,   216, 79,
    0,   147, 148, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   155, 0,   0,   157, 0,   38,  539, 366, 0,   0,   0,   0,   0,   403, 0,   325, 228,
    240, 0,   0,   413, 0,   0,   190, 0,   54,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   52,  51,  540, 549, 0,   0,   212, 212, 398, 178, 0,   0,
    0,   217, 215, 78,  84,  85,  82,  83,  86,  87,  88,  89,  90,  0,   81,  128, 129, 126, 127, 130, 131, 132, 133, 134, 0,   125, 95,  96,  93,
    94,  97,  98,  99,  100, 101, 0,   92,  106, 107, 104, 105, 108, 109, 110, 111, 112, 0,   103, 139, 140, 137, 138, 141, 142, 143, 144, 145, 0,
    136, 117, 118, 115, 116, 119, 120, 121, 122, 123, 0,   114, 0,   0,   0,   0,   0,   0,   0,   0,   327, 326, 332, 241, 233, 91,  135, 102, 113,
    146, 124, 212, 0,   212, 539, 399, 0,   333, 328, 0,   0,   0,   0,   397, 0,   0,   0,   0,   329, 212, 539, 212, 539, 0,   0,   0,   334, 330,
    0,   393, 0,   0,   0,   396, 0,   400, 335, 331, 539, 387, 395, 0,   0,   0,   0,   392, 0,   0,   0,   401, 391, 0,   389, 0,   0,   0,   0,
    539, 0,   0,   394, 539, 388, 390};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] = {
    -759, -759, -759, 1398, 1462, 66,   -759, -759, 885,  -578, -759, -654, -759, 794,  795,  -759, -581, 267,  302,  1321, -759, 347,  -759, 1153,
    350,  356,  -6,   1512, -19,  1193, 1323, -100, -759, -759, 934,  -759, -759, -759, -759, -759, -759, -759, -758, -236, -759, -759, -759, -759,
    752,  -52,  52,   624,  -759, -759, 1366, -759, -759, 408,  421,  429,  431,  432,  -759, -759, -759, -221, -759, 1130, -245, -233, -666, -665,
    -662, -656, -652, -644, 618,  -759, -759, -759, -759, -759, -759, 1136, -759, -759, 999,  -276, -272, -759, -759, -759, 784,  -759, -759, -759,
    -759, 785,  -759, -759, 1131, 1133, 787,  -759, -759, -759, -759, 1288, -514, 801,  -152, 754,  790,  -759, -759, -638, -759, 662,  763,  -759};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] = {0,   23,  24,  25,  66,   26,  508, 703, 509, 510, 809, 643, 734, 735, 881, 511, 379, 27,  28,  234,
                                         29,  30,  243, 244, 31,   32,  33,  34,  35,  139, 219, 140, 224, 497, 498, 608, 368, 502, 222, 496,
                                         604, 718, 686, 246, 1021, 926, 137, 712, 713, 714, 715, 797, 36,  113, 114, 716, 794, 37,  38,  39,
                                         40,  41,  42,  43,  44,   281, 520, 282, 283, 284, 285, 286, 287, 288, 289, 290, 804, 805, 291, 292,
                                         293, 294, 295, 409, 296,  297, 298, 299, 300, 897, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310,
                                         435, 436, 311, 312, 313,  314, 315, 316, 660, 661, 248, 151, 142, 133, 147, 483, 740, 706, 707, 514};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] = {
    386,  365,  73,   130,  440,  736,  434,  385,  903,  249,  702,  662,  408,  756,  704,  170,  58,   371,  251,  252,  253,  404,  431,  432,
    16,   340,  138,  251,  252,  253,  431,  432,  188,  578,  438,  245,  681,  220,  317,  349,  318,  319,  429,  430,  160,  161,  443,  495,
    766,  444,  445,  798,  799,  505,  482,  800,  59,   559,  61,   73,   183,  801,  667,  184,  738,  802,  795,  111,  808,  810,  444,  445,
    759,  803,  67,   441,  -534, 134,  442,  135,  606,  607,  482,  1,    136,  2,    3,    4,    5,    6,    7,    8,    9,    10,   562,  1236,
    320,  -537, 1193, 11,   51,   486,  12,   149,  13,   14,   15,   141,  58,   767,  487,  1221, 159,  553,  258,  259,  260,  796,  364,  1097,
    261,  499,  500,  258,  259,  260,  1237, 116,  134,  261,  135,  67,   117,  169,  118,  136,  119,  762,  522,  1233, 60,   560,  1222, 1010,
    444,  445,  380,  444,  445,  262,  263,  264,  563,  16,   185,  186,  883,  767,  262,  263,  264,  532,  533,  444,  445,  22,   444,  445,
    528,  404,  1234, 538,  539,  540,  541,  542,  543,  506,  213,  507,  767,  372,  102,  45,   46,   47,   16,   162,  442,  48,   49,   157,
    384,  580,  585,  227,  228,  229,  597,  557,  558,  103,  911,  321,  237,  238,  239,  598,  104,  250,  251,  252,  253,  171,  891,  381,
    564,  565,  566,  567,  568,  569,  570,  571,  572,  573,  574,  575,  576,  577,  278,  711,  126,  278,  433,  109,  218,  439,  280,  278,
    346,  610,  433,  579,  65,   17,   277,  280,  1,    127,  2,    3,    4,    5,    6,    7,    609,  9,    798,  799,  437,  18,   800,  322,
    11,   323,  324,  12,   801,  13,   14,   15,   802,  19,   602,  68,   919,  1023, 254,  255,  803,  20,   21,   374,  654,  655,  444,  445,
    256,  120,  257,  671,  672,  407,  22,   656,  657,  658,  652,  668,  680,  327,  683,  328,  329,  663,  258,  259,  260,  121,  69,   1209,
    261,  122,  760,  464,  123,  463,  16,   763,  465,  325,  52,   53,   54,   124,  125,  693,  55,   56,   407,  110,  68,   444,  445,  112,
    444,  445,  1175, 1176, 695,  262,  263,  264,  265,  50,   266,  342,  267,  535,  268,  536,  269,  537,  270,  70,   444,  445,  71,   330,
    499,  343,  344,  271,  72,   504,  1093, 69,   444,  445,  767,  917,  1,    918,  2,    3,    4,    5,    6,    7,    8,    9,    10,   444,
    445,  272,  531,  273,  11,   274,  115,  12,   1012, 13,   14,   15,   128,  481,  444,  445,  764,  659,  691,  1006, 444,  445,  250,  251,
    252,  253,  17,   131,  70,   444,  445,  71,   74,   275,  276,  277,  673,  72,   278,  757,  279,  758,  444,  445,  326,  75,   280,  444,
    445,  1200, 521,  1202, 1102, 76,   19,   77,   78,   132,  16,   444,  445,  517,  761,  21,   518,  138,  134,  1218, 135,  1220, 375,  376,
    730,  136,  141,  22,   105,  106,  107,  148,  775,  466,  331,  526,  674,  150,  467,  772,  377,  74,   730,  254,  255,  675,  108,  482,
    646,  156,  806,  647,  57,   256,  75,   257,  62,   63,   676,  895,  157,  64,   76,   677,  77,   78,   163,  250,  251,  252,  253,  258,
    259,  260,  648,  678,  158,  261,  665,  731,  164,  732,  733,  892,  879,  894,  194,  195,  468,  165,  904,  196,  888,  469,  446,  167,
    447,  731,  472,  732,  733,  664,  17,   473,  442,  168,  262,  263,  264,  265,  694,  266,  176,  267,  177,  268,  474,  269,  18,   270,
    178,  475,  687,  333,  1203, 688,  334,  335,  271,  179,  19,   336,  337,  172,  173,  448,  254,  255,  20,   21,   1219, 16,   1223, 669,
    187,  670,  256,  537,  257,  190,  272,  22,   273,  448,  274,  189,  -538, -538, 1235, 250,  251,  252,  253,  191,  258,  259,  260,  180,
    181,  182,  261,  998,  449,  450,  451,  452,  453,  1250, 470,  471,  454,  1253, 275,  276,  277,  174,  175,  278,  1178, 279,  527,  1179,
    1180, 490,  491,  280,  1181, 1182, 208,  262,  263,  264,  265,  272,  266,  273,  267,  274,  268,  192,  269,  193,  270,  -538, -538, 458,
    459,  460,  461,  462,  423,  271,  424,  425,  744,  207,  426,  442,  254,  255,  455,  456,  457,  458,  459,  460,  461,  462,  256,  751,
    257,  771,  752,  272,  210,  273,  209,  274,  214,  250,  251,  252,  253,  689,  690,  197,  258,  259,  260,  211,  753,  754,  261,  752,
    442,  198,  444,  445,  199,  200,  201,  774,  202,  785,  442,  275,  276,  277,  778,  212,  278,  518,  279,  460,  461,  462,  203,  204,
    280,  205,  206,  262,  263,  264,  265,  215,  266,  217,  267,  221,  268,  225,  269,  223,  270,  589,  590,  226,  811,  812,  813,  814,
    815,  271,  230,  816,  817,  779,  402,  403,  780,  231,  818,  819,  820,  232,  782,  235,  256,  783,  257,  912,  913,  914,  915,  272,
    233,  273,  884,  274,  236,  518,  908,  1104, 821,  442,  258,  259,  260,  1207, 1208, 240,  261,  250,  251,  252,  253,  787,  -248, 788,
    789,  790,  791,  241,  792,  793,  1103, 275,  276,  277,  927,  242,  278,  928,  279,  152,  153,  154,  155,  245,  280,  262,  263,  264,
    265,  247,  266,  332,  267,  341,  268,  338,  269,  1,    270,  2,    3,    4,    5,    6,    7,    339,  9,    271,  143,  144,  145,  146,
    930,  11,   1001, 931,  12,   518,  13,   14,   15,   79,   1005, 350,  1092, 442,  402,  783,  1183, 272,  345,  273,  1099, 274,  1184, 752,
    256,  518,  257,  1215, 1216, 1015, 1016, 347,  351,  352,  353,  354,  355,  356,  80,   81,   357,  82,   258,  259,  260,  358,  83,   84,
    261,  275,  276,  277,  359,  360,  278,  16,   279,  822,  823,  824,  825,  826,  280,  361,  827,  828,  366,  362,  367,  369,  370,  829,
    830,  831,  378,  387,  382,  262,  263,  264,  265,  383,  266,  389,  267,  388,  268,  405,  269,  406,  270,  410,  411,  832,  833,  834,
    835,  836,  837,  271,  412,  838,  839,  413,  414,  416,  417,  418,  840,  841,  842,  419,  530,  420,  476,  477,  421,  422,  427,  428,
    478,  272,  463,  273,  479,  274,  480,  485,  482,  489,  843,  488,  492,  501,  493,  530,  494,  503,  512,  513,  515,  516,  519,  524,
    530,  525,  529,  17,   544,  545,  16,   546,  547,  275,  276,  277,  548,  549,  278,  550,  279,  551,  552,  556,  561,  554,  280,  85,
    86,   87,   88,   448,  89,   90,   278,  19,   91,   92,   93,   581,  583,  94,   586,  95,   21,   587,  593,  96,   97,   588,  449,  450,
    451,  452,  448,  595,  22,   591,  454,  592,  98,   99,   594,  448,  100,  596,  599,  600,  101,  601,  603,  605,  448,  449,  450,  451,
    452,  645,  697,  644,  649,  454,  449,  450,  451,  452,  650,  651,  560,  666,  454,  -538, -538, 451,  452,  653,  679,  684,  685,  -538,
    444,  692,  696,  698,  699,  700,  701,  505,  455,  456,  457,  458,  459,  460,  461,  462,  708,  705,  717,  893,  709,  710,  720,  442,
    721,  722,  739,  723,  743,  746,  724,  455,  456,  457,  458,  459,  460,  461,  462,  725,  455,  456,  457,  458,  459,  460,  461,  462,
    742,  -538, 456,  457,  458,  459,  460,  461,  462,  611,  612,  613,  614,  615,  616,  617,  618,  619,  620,  621,  622,  623,  624,  625,
    626,  627,  747,  628,  629,  630,  631,  632,  633,  726,  727,  634,  728,  729,  635,  636,  637,  638,  639,  640,  641,  642,  844,  845,
    846,  847,  848,  737,  745,  849,  850,  748,  749,  755,  750,  769,  851,  852,  853,  855,  856,  857,  858,  859,  765,  770,  860,  861,
    768,  773,  690,  689,  776,  862,  863,  864,  777,  784,  854,  781,  786,  807,  866,  867,  868,  869,  870,  877,  878,  871,  872,  879,
    885,  887,  890,  865,  873,  874,  875,  390,  391,  392,  393,  394,  395,  396,  397,  398,  399,  400,  401,  886,  896,  901,  902,  921,
    905,  906,  876,  907,  909,  910,  923,  922,  924,  925,  932,  929,  933,  941,  952,  934,  963,  935,  936,  937,  938,  939,  940,  999,
    942,  943,  944,  945,  946,  947,  948,  974,  949,  950,  1000, 951,  953,  954,  985,  955,  956,  957,  958,  959,  960,  961,  962,  996,
    964,  1003, 965,  966,  967,  1004, 767,  968,  969,  970,  1013, 1019, 971,  972,  973,  1020, 1022, 1024, 1025, 1026, 1027, 975,  1028, 1029,
    1030, 976,  977,  978,  1031, 1032, 1034, 979,  980,  981,  982,  983,  1035, 984,  986,  987,  988,  989,  990,  991,  992,  993,  1036, 994,
    995,  997,  1007, 1011, 1008, 1009, 1017, 1033, 1037, 1038, 1039, 1040, 1041, 1042, 1043, 1044, 1045, 1046, 1047, 1048, 1049, 1050, 1051, 1052,
    1053, 1054, 1056, 1057, 1055, 1058, 1059, 1060, 1061, 1062, 1063, 1064, 1065, 1066, 1067, 1068, 1069, 1070, 1071, 1072, 1073, 1074, 1075, 1076,
    1078, 1079, 1077, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1089, 1088, 1090, 1091, 1094, 1095, 1096, 1244, 1098, 1100, 1105, 1101, 1115,
    1106, 1126, 1137, 1107, 1108, 1109, 1148, 1159, 1110, 1170, 1191, 1111, 1112, 1113, 1114, 1116, 1117, 1118, 1119, 1120, 1121, 1122, 1123, 1124,
    1125, 1127, 1128, 1129, 1130, 1131, 1132, 1133, 1134, 1135, 1136, 1138, 1139, 1140, 1141, 1142, 1177, 1143, 1144, 1145, 1192, 1146, 1147, 1149,
    1150, 1197, 1196, 1151, 1152, 1153, 1198, 1154, 1155, 1156, 1206, 1157, 1158, 1211, 1160, 1161, 1162, 1163, 1164, 1165, 1166, 1167, 1168, 1169,
    1171, 1204, 1213, 1205, 1172, 1173, 1174, 1185, 1186, 1187, 1188, 1189, 1190, 1194, 1195, 1214, 1217, 1210, 1199, 1201, 1212, 1225, 1224, 1226,
    1227, 1228, 1229, 1230, 1231, 1232, 1239, 1238, 1240, 1241, 1242, 1243, 1247, 1248, 1249, 166,  880,  1245, 216,  882,  741,  1246, 129,  1251,
    1252, 523,  920,  719,  484,  363,  1018, 1014, 534,  682,  916,  1002, 898,  899,  373,  900,  415,  889,  0,    0,    348,  0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    555,  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    582,  0,    0,    0,    584};

static const yytype_int16 yycheck[] = {
    245, 222,  8,   22,   280,  643,  278, 243,  766, 161, 591, 525, 257, 667, 592,  6,    3,    54,  4,   5,    6,   254,  5,   6,    77,  177, 8,
    4,   5,    6,   5,    6,    61,   3,   279,  65,  3,   137, 3,   191, 5,   6,    275,  276,  75,  76,  54,   81,  4,    155, 156,  717, 717, 3,
    77,  717,  4,   74,   6,    65,   200, 717,  54,  203, 645, 717, 3,   15,  722,  723,  155,  156, 83,  717,  8,   219,  0,   20,   222, 22,  69,
    70,  77,   7,   27,   9,    10,   11,  12,   13,  14,  15,  16,  17,  72,  134,  61,   61,   6,   23,  36,   39,  26,   51,  28,   29,  30,  71,
    3,   65,   48,  134,  60,   219,  100, 101,  102, 54,  218, 4,   106, 366, 367,  100,  101,  102, 165, 7,    20,  106,  22,  65,   12,  81,  14,
    27,  16,   222, 383,  134,  75,   158, 165,  901, 155, 156, 37,  155, 156, 135,  136,  137,  130, 77,  102,  103, 737,  65,  135,  136, 137, 406,
    407, 155,  156, 218,  155,  156,  389, 402,  165, 416, 417, 418, 419, 420, 421,  127,  126,  129, 65,  218,  33,  32,   33,  34,   77,  218, 222,
    38,  39,   220, 222,  465,  470,  143, 144,  145, 32,  444, 445, 33,  780, 168,  152,  153,  154, 41,  39,   3,   4,    5,   6,    204, 83,  234,
    449, 450,  451, 452,  453,  454,  455, 456,  457, 458, 459, 460, 461, 462, 216,  218,  33,   216, 217, 3,    218, 223,  224, 216,  188, 133, 217,
    213, 3,    169, 213,  224,  7,    50,  9,    10,  11,  12,  13,  14,  501, 16,   924,  924,  279, 185, 924,  3,   23,   5,   6,    26,  924, 28,
    29,  30,   924, 197,  495,  8,    219, 931,  72,  73,  924, 205, 206, 231, 67,   68,   155,  156, 82,  169,  84,  536,  537, 87,   218, 78,  79,
    80,  519,  86,  545,  3,    547,  5,   6,    526, 100, 101, 102, 189, 8,   6,    106,  193,  86,  217, 196,  220, 77,   222, 222,  61,  32,  33,
    34,  32,   33,  560,  38,   39,   87,  3,    65,  155, 156, 3,   155, 156, 1096, 1097, 561,  135, 136, 137,  138, 194,  140, 10,   142, 83,  144,
    85,  146,  87,  148,  8,    155,  156, 8,    61,  605, 24,  25,  157, 8,   371,  1004, 65,   155, 156, 65,   33,  7,    35,  9,    10,  11,  12,
    13,  14,   15,  16,   17,   155,  156, 179,  405, 181, 23,  183, 172, 26,  906,  28,   29,   30,  159, 345,  155, 156,  222, 184,  554, 222, 155,
    156, 3,    4,   5,    6,    169,  0,   65,   155, 156, 65,  8,   211, 212, 213,  219,  65,   216, 668, 218,  670, 155,  156, 168,  8,   224, 155,
    156, 1191, 382, 1193, 1017, 8,    197, 8,    8,   221, 77,  155, 156, 219, 679,  206,  222,  8,   20,  1209, 22,  1211, 32,  33,   72,  27,  71,
    218, 159,  160, 161,  3,    697,  217, 168,  64,  219, 71,  222, 692, 50,  65,   72,   72,   73,  219, 177,  77,  219,  3,   718,  222, 194, 82,
    65,  84,   166, 167,  219,  763,  220, 171,  65,  219, 65,  65,  3,   3,   4,    5,    6,    100, 101, 102,  512, 219,  14,  106,  529, 127, 3,
    129, 130,  760, 132,  762,  185,  186, 217,  3,   767, 190, 745, 222, 72,  218,  74,   127,  217, 129, 130,  219, 169,  222, 222,  3,   135, 136,
    137, 138,  561, 140,  3,    142,  76,  144,  217, 146, 185, 148, 6,   222, 219,  39,   1194, 222, 42,  43,   157, 3,    197, 47,   48,  173, 174,
    131, 72,   73,  205,  206,  1210, 77,  1212, 83,  3,   85,  82,  87,  84,  222,  179,  218,  181, 131, 183,  54,  152,  153, 1228, 3,   4,   5,
    6,   6,    100, 101,  102,  199,  200, 201,  106, 879, 150, 151, 152, 153, 154,  1247, 222,  223, 158, 1251, 211, 212,  213, 173,  174, 216, 39,
    218, 219,  42,  43,   187,  188,  224, 47,   48,  4,   135, 136, 137, 138, 179,  140,  181,  142, 183, 144,  194, 146,  194, 148,  209, 210, 211,
    212, 213,  214, 215,  120,  157,  122, 123,  219, 194, 126, 222, 72,  73,  208,  209,  210,  211, 212, 213,  214, 215,  82,  219,  84,  692, 222,
    179, 75,   181, 192,  183,  219,  3,   4,    5,   6,   5,   6,   160, 100, 101,  102,  194,  219, 219, 106,  222, 222,  170, 155,  156, 173, 174,
    175, 219,  177, 711,  222,  211,  212, 213,  219, 194, 216, 222, 218, 213, 214,  215,  191,  192, 224, 194,  195, 135,  136, 137,  138, 219, 140,
    21,  142,  64,  144,  72,   146,  66,  148,  44,  45,  3,   89,  90,  91,  92,   93,   157,  3,   96,  97,   219, 72,   73,  222,  61,  103, 104,
    105, 61,   219, 73,   82,   222,  84,  3,    4,   5,   6,   179, 218, 181, 219,  183,  3,    222, 219, 1020, 125, 222,  100, 101,  102, 44,  45,
    3,   106,  3,   4,    5,    6,    55,  56,   57,  58,  59,  60,  3,   62,  63,   1019, 211,  212, 213, 219,  3,   216,  222, 218,  53,  54,  55,
    56,  65,   224, 135,  136,  137,  138, 4,    140, 219, 142, 4,   144, 3,   146,  7,    148,  9,   10,  11,   12,  13,   14,  3,    16,  157, 46,
    47,  48,   49,  219,  23,   219,  222, 26,   222, 28,  29,  30,  3,   219, 4,    219,  222,  72,  222, 1101, 179, 61,   181, 219,  183, 219, 222,
    82,  222,  84,  42,   43,   921,  922, 218,  165, 6,   3,   6,   4,   3,   32,   33,   4,    35,  100, 101,  102, 54,   40,  41,   106, 211, 212,
    213, 4,    194, 216,  77,   218,  89,  90,   91,  92,  93,  224, 3,   96,  97,   52,   3,    67,  73,  133,  103, 104,  105, 3,    77,  61,  135,
    136, 137,  138, 208,  140,  218,  142, 77,   144, 218, 146, 218, 148, 4,   4,    125,  89,   90,  91,  92,   93,  157,  4,   96,   97,  4,   6,
    218, 218,  218, 103,  104,  105,  218, 72,   218, 3,   6,   218, 218, 218, 218,  46,   179,  220, 181, 46,   183, 76,   4,   77,   76,  125, 6,
    4,   68,   219, 72,   219,  4,    54,  218,  3,   3,   218, 218, 72,  218, 218,  169,  4,    218, 77,  218,  218, 211,  212, 213,  218, 4,   216,
    4,   218,  225, 219,  3,    218,  76,  224,  161, 162, 163, 164, 131, 166, 167,  216,  197,  170, 171, 172,  6,   6,    175, 6,    177, 206, 5,
    3,   181,  182, 42,   150,  151,  152, 153,  131, 219, 218, 218, 158, 218, 194,  195,  218,  131, 198, 6,    4,   165,  202, 165,  75,  222, 131,
    150, 151,  152, 153,  218,  155,  128, 3,    158, 150, 151, 152, 153, 133, 3,    158,  219,  158, 150, 151,  152, 153,  222, 75,   4,   222, 158,
    155, 218,  130, 225,  225,  6,    6,   3,    208, 209, 210, 211, 212, 213, 214,  215,  6,    3,   178, 219,  4,   4,    4,   222,  218, 218, 31,
    218, 4,    6,   218,  208,  209,  210, 211,  212, 213, 214, 215, 218, 208, 209,  210,  211,  212, 213, 214,  215, 219,  208, 209,  210, 211, 212,
    213, 214,  215, 88,   89,   90,   91,  92,   93,  94,  95,  96,  97,  98,  99,   100,  101,  102, 103, 104,  6,   106,  107, 108,  109, 110, 111,
    218, 218,  114, 218,  218,  117,  118, 119,  120, 121, 122, 123, 124, 89,  90,   91,   92,   93,  218, 218,  96,  97,   4,   3,    219, 6,   4,
    103, 104,  105, 89,   90,   91,   92,  93,   222, 77,  96,  97,  219, 219, 6,    5,    49,   103, 104, 105,  46,  6,    125, 208,  222, 6,   89,
    90,  91,   92,  93,   130,  128,  96,  97,   132, 218, 165, 219, 125, 103, 104,  105,  138,  139, 140, 141,  142, 143,  144, 145,  146, 147, 148,
    149, 133,  216, 4,    222,  56,   219, 218,  125, 219, 6,   6,   3,   56,  222,  51,   222,  219, 222, 91,   91,  222,  91,  222,  222, 222, 222,
    222, 222,  133, 222,  222,  222,  222, 222,  222, 222, 91,  222, 222, 133, 222,  222,  222,  91,  222, 222,  222, 222,  222, 222,  222, 222, 91,
    222, 4,    222, 222,  222,  3,    65,  222,  222, 222, 6,   52,  222, 222, 222,  53,   6,    6,   6,   6,    6,   222,  6,   6,    6,   222, 222,
    222, 6,    6,   6,    222,  222,  222, 222,  222, 6,   222, 222, 222, 222, 222,  222,  222,  222, 222, 6,    222, 222,  222, 222,  219, 222, 222,
    218, 222,  6,   6,    6,    6,    6,   6,    6,   222, 6,   6,   6,   6,   6,    6,    6,    6,   6,   6,    6,   6,    222, 6,    6,   6,   6,
    6,   6,    6,   6,    222,  6,    6,   6,    6,   6,   6,   6,   6,   6,   6,    6,    6,    222, 6,   6,    6,   6,    6,   6,    6,   6,   6,
    222, 4,    4,   4,    4,    4,    3,   219,  6,   219, 61,  6,   219, 6,   6,    219,  219,  219, 6,   6,    219, 6,    4,   219,  219, 219, 219,
    219, 219,  219, 219,  219,  219,  219, 219,  219, 219, 219, 219, 219, 219, 219,  219,  219,  219, 219, 219,  219, 219,  219, 219,  219, 165, 219,
    219, 219,  4,   219,  219,  219,  219, 6,    48,  219, 219, 219, 6,   219, 219,  219,  6,    219, 219, 4,    219, 219,  219, 219,  219, 219, 219,
    219, 219,  219, 219,  165,  48,   218, 222,  222, 222, 219, 219, 219, 219, 219,  219,  219,  219, 4,   6,    219, 222,  222, 219,  219, 218, 6,
    47,  219,  219, 39,   39,   4,    219, 218,  39,  39,  39,  39,  219, 3,   3,    65,   734,  218, 132, 736,  647, 218,  22,  219,  219, 384, 786,
    605, 347,  218, 924,  919,  408,  546, 783,  885, 764, 764, 229, 764, 264, 752,  -1,   -1,   190, -1,  -1,   -1,  -1,   -1,  -1,   -1,  -1,  -1,
    -1,  -1,   -1,  -1,   -1,   -1,   442, -1,   -1,  -1,  -1,  -1,  -1,  -1,  -1,   -1,   -1,   -1,  -1,  -1,   -1,  -1,   -1,  -1,   -1,  -1,  -1,
    -1,  -1,   -1,  -1,   -1,   467,  -1,  -1,   -1,  469};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] = {
    0,   7,   9,   10,  11,  12,  13,  14,  15,  16,  17,  23,  26,  28,  29,  30,  77,  169, 185, 197, 205, 206, 218, 227, 228, 229, 231, 243, 244,
    246, 247, 250, 251, 252, 253, 254, 278, 283, 284, 285, 286, 287, 288, 289, 290, 32,  33,  34,  38,  39,  194, 36,  32,  33,  34,  38,  39,  194,
    3,   276, 75,  276, 166, 167, 171, 3,   230, 231, 243, 244, 247, 250, 251, 252, 283, 284, 285, 286, 287, 3,   32,  33,  35,  40,  41,  161, 162,
    163, 164, 166, 167, 170, 171, 172, 175, 177, 181, 182, 194, 195, 198, 202, 33,  33,  39,  159, 160, 161, 177, 3,   3,   276, 3,   279, 280, 172,
    7,   12,  14,  16,  169, 189, 193, 196, 32,  33,  33,  50,  159, 253, 254, 0,   221, 339, 20,  22,  27,  272, 8,   255, 257, 71,  338, 338, 338,
    338, 338, 340, 3,   276, 71,  337, 337, 337, 337, 337, 3,   220, 14,  276, 75,  76,  218, 3,   3,   3,   230, 218, 3,   276, 6,   204, 173, 174,
    173, 174, 3,   76,  6,   3,   199, 200, 201, 200, 203, 276, 276, 3,   61,  54,  222, 6,   194, 194, 185, 186, 190, 160, 170, 173, 174, 175, 177,
    191, 192, 194, 195, 194, 4,   192, 75,  194, 194, 276, 219, 219, 229, 21,  218, 256, 257, 64,  264, 66,  258, 72,  3,   276, 276, 276, 3,   61,
    61,  218, 245, 73,  3,   276, 276, 276, 3,   3,   3,   248, 249, 65,  269, 4,   336, 336, 3,   4,   5,   6,   72,  73,  82,  84,  100, 101, 102,
    106, 135, 136, 137, 138, 140, 142, 144, 146, 148, 157, 179, 181, 183, 211, 212, 213, 216, 218, 224, 291, 293, 294, 295, 296, 297, 298, 299, 300,
    301, 304, 305, 306, 307, 308, 310, 311, 312, 313, 314, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 328, 329, 330, 331, 332, 333, 3,   5,
    6,   61,  168, 3,   5,   6,   61,  168, 3,   5,   6,   61,  168, 219, 39,  42,  43,  47,  48,  3,   3,   336, 4,   10,  24,  25,  61,  276, 218,
    280, 336, 4,   165, 6,   3,   6,   4,   3,   4,   54,  4,   194, 3,   3,   256, 257, 291, 52,  67,  262, 73,  133, 54,  218, 245, 276, 32,  33,
    50,  3,   242, 37,  254, 61,  208, 222, 269, 294, 77,  77,  218, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 72,  73,  295, 218,
    218, 87,  294, 309, 4,   4,   4,   4,   6,   333, 218, 218, 218, 218, 218, 218, 218, 120, 122, 123, 126, 218, 218, 295, 295, 5,   6,   217, 314,
    326, 327, 254, 294, 223, 313, 219, 222, 54,  155, 156, 72,  74,  131, 150, 151, 152, 153, 154, 158, 208, 209, 210, 211, 212, 213, 214, 215, 220,
    217, 222, 217, 222, 217, 222, 222, 223, 217, 222, 217, 222, 3,   6,   46,  46,  76,  276, 77,  341, 255, 4,   39,  48,  6,   76,  187, 188, 4,
    219, 219, 81,  265, 259, 260, 294, 294, 68,  263, 4,   252, 3,   127, 129, 232, 234, 235, 241, 54,  218, 345, 3,   3,   219, 222, 218, 292, 276,
    294, 249, 218, 218, 64,  219, 291, 218, 72,  254, 294, 294, 309, 83,  85,  87,  294, 294, 294, 294, 294, 294, 4,   218, 218, 218, 218, 4,   4,
    225, 219, 219, 76,  293, 3,   294, 294, 74,  158, 218, 72,  130, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 3,   213,
    314, 6,   326, 6,   327, 313, 6,   5,   42,  44,  45,  218, 218, 3,   218, 219, 6,   32,  41,  4,   165, 165, 291, 75,  266, 222, 69,  70,  261,
    294, 133, 88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 106, 107, 108, 109, 110, 111, 114, 117, 118, 119,
    120, 121, 122, 123, 124, 237, 128, 218, 219, 222, 252, 3,   133, 3,   291, 222, 67,  68,  78,  79,  80,  184, 334, 335, 334, 291, 219, 254, 219,
    54,  86,  83,  85,  294, 294, 219, 219, 219, 219, 219, 219, 75,  294, 3,   312, 294, 4,   222, 268, 219, 222, 5,   6,   336, 218, 295, 254, 291,
    130, 155, 225, 225, 6,   6,   242, 233, 235, 3,   343, 344, 6,   4,   4,   218, 273, 274, 275, 276, 281, 178, 267, 260, 4,   218, 218, 218, 218,
    218, 218, 218, 218, 218, 72,  127, 129, 130, 238, 239, 341, 218, 242, 31,  342, 234, 219, 4,   219, 218, 6,   6,   4,   3,   6,   219, 222, 219,
    219, 219, 237, 294, 294, 83,  86,  295, 222, 222, 222, 222, 4,   65,  219, 4,   77,  254, 291, 219, 219, 295, 49,  46,  219, 219, 222, 208, 219,
    222, 6,   252, 222, 55,  57,  58,  59,  60,  62,  63,  282, 3,   54,  277, 296, 297, 298, 299, 300, 301, 302, 303, 269, 6,   237, 236, 237, 89,
    90,  91,  92,  93,  96,  97,  103, 104, 105, 125, 89,  90,  91,  92,  93,  96,  97,  103, 104, 105, 125, 89,  90,  91,  92,  93,  96,  97,  103,
    104, 105, 125, 89,  90,  91,  92,  93,  96,  97,  103, 104, 105, 125, 89,  90,  91,  92,  93,  96,  97,  103, 104, 105, 125, 89,  90,  91,  92,
    93,  96,  97,  103, 104, 105, 125, 130, 128, 132, 239, 240, 240, 242, 219, 218, 133, 165, 291, 335, 219, 83,  294, 219, 294, 314, 216, 315, 318,
    323, 328, 4,   222, 268, 294, 219, 218, 219, 219, 6,   6,   235, 3,   4,   5,   6,   344, 33,  35,  219, 274, 56,  56,  3,   222, 51,  271, 219,
    222, 219, 219, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 91,  222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 91,  222, 222, 222, 222,
    222, 222, 222, 222, 222, 222, 91,  222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 91,  222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 91,
    222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 91,  222, 313, 133, 133, 219, 343, 4,   3,   219, 222, 222, 222, 222, 268, 219, 334, 6,   277,
    275, 275, 218, 302, 52,  53,  270, 6,   237, 6,   6,   6,   6,   6,   6,   6,   6,   6,   222, 6,   6,   6,   6,   6,   6,   6,   6,   6,   6,
    222, 6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   222, 6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   222, 6,   6,   6,   6,   6,   6,
    6,   6,   6,   6,   222, 6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   222, 6,   4,   4,   219, 341, 4,   4,   4,   4,   219, 219, 6,   61,
    242, 291, 294, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 6,   219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 6,   219, 219, 219, 219,
    219, 219, 219, 219, 219, 219, 6,   219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 6,   219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 6,
    219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 6,   219, 222, 222, 222, 268, 268, 165, 39,  42,  43,  47,  48,  294, 219, 219, 219, 219, 219,
    219, 219, 4,   4,   6,   219, 219, 48,  6,   6,   222, 268, 222, 268, 341, 165, 218, 6,   44,  45,  6,   219, 4,   219, 48,  4,   42,  43,  6,
    268, 341, 268, 134, 165, 341, 218, 219, 6,   47,  219, 219, 39,  39,  4,   134, 165, 341, 134, 165, 218, 219, 39,  39,  39,  39,  3,   218, 218,
    219, 3,   3,   341, 219, 219, 341};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] = {
    0,   226, 227, 228, 228, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 230, 230, 230, 230, 230, 230, 230, 230,
    230, 230, 230, 230, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 232, 232, 233, 233, 234, 234, 235, 235, 235, 235, 236, 236, 237, 237, 237,
    237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237,
    237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237,
    237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237,
    237, 237, 237, 237, 238, 238, 239, 239, 239, 239, 240, 240, 241, 241, 242, 242, 243, 244, 244, 245, 245, 246, 246, 247, 248, 248, 249, 250, 250,
    250, 250, 250, 251, 251, 251, 252, 252, 252, 252, 253, 253, 254, 255, 256, 256, 257, 258, 258, 259, 259, 260, 261, 261, 261, 262, 262, 263, 263,
    264, 264, 265, 265, 266, 266, 267, 267, 268, 268, 269, 269, 270, 270, 271, 271, 272, 272, 272, 272, 273, 273, 274, 274, 275, 275, 276, 276, 277,
    277, 277, 277, 278, 278, 279, 279, 280, 281, 281, 282, 282, 282, 282, 282, 282, 282, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283,
    283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283,
    284, 284, 284, 284, 285, 285, 286, 286, 286, 286, 286, 286, 286, 286, 286, 286, 286, 286, 286, 286, 286, 286, 286, 286, 286, 286, 286, 286, 286,
    286, 286, 287, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288,
    288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 289, 289, 289, 290, 290, 291, 291, 292, 292, 293, 293, 294, 294, 294, 294, 294, 295, 295, 295,
    295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 296, 296, 296, 297, 297, 297, 297, 298, 298, 298, 298, 299, 299, 299, 299, 300, 300, 301, 301,
    302, 302, 302, 302, 302, 302, 303, 303, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304,
    304, 304, 304, 304, 304, 304, 304, 304, 305, 305, 306, 307, 307, 308, 308, 308, 308, 309, 309, 310, 311, 311, 311, 311, 312, 312, 312, 312, 313,
    313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 314, 314, 314, 314, 315, 315, 315, 316, 317, 317, 318, 318, 319, 320, 320, 321, 322,
    322, 323, 324, 324, 325, 325, 326, 327, 328, 328, 329, 330, 330, 331, 332, 332, 333, 333, 333, 333, 333, 333, 333, 333, 333, 333, 333, 333, 334,
    334, 335, 335, 335, 335, 335, 335, 336, 337, 337, 338, 338, 339, 339, 340, 340, 341, 341, 342, 342, 343, 343, 344, 344, 344, 344, 344, 345, 345};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] = {
    0,  2,  2, 1, 3, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1,  1,  1,  1,  1,  1,  6,  4,  4,  8,  6,  10, 8,
    7,  6,  8, 1, 3, 1,  3,  1,  1,  4,  4,  6,  6,  1,  3,  1,  1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  6,  4,
    1,  6,  6, 6, 6, 6,  6,  6,  6,  6,  6,  7,  6,  6,  6,  6,  6, 6, 6,  6, 6, 6, 7, 6, 6, 6, 6, 6,  6,  6,  6,  6,  6,  7,  6,  6,  6,  6,  6,  6,
    6,  6,  6, 6, 7, 6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  6, 6, 6,  6, 6, 6, 6, 6, 6, 6, 7, 4,  4,  1,  2,  2,  1,  1,  2,  2,  0,  5,  4,  1,
    3,  4,  6, 5, 3, 0,  3,  2,  5,  1,  3,  3,  4,  4,  4,  4,  6, 8, 11, 8, 1, 1, 3, 3, 3, 3, 2, 4,  3,  3,  9,  3,  0,  1,  3,  2,  1,  1,  0,  2,
    0,  2,  0, 1, 0, 2,  0,  2,  0,  2,  0,  3,  0,  2,  0,  2,  0, 3, 0,  1, 2, 1, 1, 1, 3, 1, 1, 2,  4,  1,  3,  2,  1,  5,  0,  2,  0,  1,  3,  5,
    4,  6,  1, 1, 1, 1,  1,  1,  0,  2,  2,  2,  2,  3,  2,  2,  2, 2, 3,  2, 3, 3, 3, 3, 4, 4, 3, 3,  4,  4,  5,  6,  7,  9,  4,  5,  7,  9,  2,  2,
    4,  3,  3, 4, 2, 3,  3,  4,  2,  3,  2,  2,  2,  2,  2,  5,  2, 4, 4,  4, 4, 4, 4, 4, 4, 4, 4, 4,  4,  4,  4,  4,  4,  6,  6,  5,  3,  4,  4,  2,
    5,  3,  6, 7, 9, 10, 12, 12, 13, 14, 15, 16, 12, 13, 15, 16, 3, 4, 5,  6, 3, 3, 4, 3, 3, 4, 4, 6,  5,  3,  4,  3,  4,  3,  3,  5,  7,  7,  6,  8,
    8,  2,  3, 1, 3, 3,  5,  3,  1,  1,  1,  1,  1,  1,  3,  3,  1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 14, 19, 16, 20, 16, 15, 13, 18, 14, 13, 11, 8,  10,
    13, 15, 5, 7, 4, 6,  1,  1,  1,  1,  1,  1,  1,  3,  3,  4,  5, 4, 4,  4, 4, 4, 4, 4, 3, 2, 2, 2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
    6,  3,  4, 3, 3, 5,  5,  6,  4,  6,  3,  5,  4,  5,  6,  4,  5, 5, 6,  1, 3, 1, 3, 1, 1, 1, 1, 1,  2,  2,  2,  2,  2,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1, 2, 2, 3,  1,  1,  2,  2,  3,  2,  2,  3,  2,  2,  2, 2, 3,  3, 3, 1, 1, 2, 2, 3, 2, 2,  3,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    2,  1,  3, 2, 2, 1,  2,  2,  2,  1,  2,  0,  3,  0,  1,  0,  2, 0, 4,  0, 4, 0, 1, 3, 1, 3, 3, 3,  3,  6,  3};

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
#line 329 "parser.y"
        {
            free(((*yyvaluep).str_value));
        }
#line 2490 "parser.cpp"
        break;

        case YYSYMBOL_STRING: /* STRING  */
#line 329 "parser.y"
        {
            free(((*yyvaluep).str_value));
        }
#line 2498 "parser.cpp"
        break;

        case YYSYMBOL_statement_list: /* statement_list  */
#line 246 "parser.y"
        {
            fprintf(stderr, "destroy statement array\n");
            if ((((*yyvaluep).stmt_array)) != nullptr) {
                for (auto ptr : *(((*yyvaluep).stmt_array))) {
                    delete ptr;
                }
                delete (((*yyvaluep).stmt_array));
            }
        }
#line 2512 "parser.cpp"
        break;

        case YYSYMBOL_table_element_array: /* table_element_array  */
#line 226 "parser.y"
        {
            fprintf(stderr, "destroy table element array\n");
            if ((((*yyvaluep).table_element_array_t)) != nullptr) {
                for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
                    delete ptr;
                }
                delete (((*yyvaluep).table_element_array_t));
            }
        }
#line 2526 "parser.cpp"
        break;

        case YYSYMBOL_column_def_array: /* column_def_array  */
#line 236 "parser.y"
        {
            fprintf(stderr, "destroy column def array\n");
            if ((((*yyvaluep).column_def_array_t)) != nullptr) {
                for (auto ptr : *(((*yyvaluep).column_def_array_t))) {
                    delete ptr;
                }
                delete (((*yyvaluep).column_def_array_t));
            }
        }
#line 2540 "parser.cpp"
        break;

        case YYSYMBOL_column_type_array: /* column_type_array  */
#line 190 "parser.y"
        {
            fprintf(stderr, "destroy column_type_array\n");
            delete (((*yyvaluep).column_type_array_t));
        }
#line 2549 "parser.cpp"
        break;

        case YYSYMBOL_column_type: /* column_type  */
#line 185 "parser.y"
        {
            fprintf(stderr, "destroy column_type\n");
            delete (((*yyvaluep).column_type_t));
        }
#line 2558 "parser.cpp"
        break;

        case YYSYMBOL_column_constraints: /* column_constraints  */
#line 322 "parser.y"
        {
            fprintf(stderr, "destroy constraints\n");
            if ((((*yyvaluep).column_constraints_t)) != nullptr) {
                delete (((*yyvaluep).column_constraints_t));
            }
        }
#line 2569 "parser.cpp"
        break;

        case YYSYMBOL_default_expr: /* default_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 2577 "parser.cpp"
        break;

        case YYSYMBOL_identifier_array: /* identifier_array  */
#line 333 "parser.y"
        {
            fprintf(stderr, "destroy identifier array\n");
            delete (((*yyvaluep).identifier_array_t));
        }
#line 2586 "parser.cpp"
        break;

        case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 333 "parser.y"
        {
            fprintf(stderr, "destroy identifier array\n");
            delete (((*yyvaluep).identifier_array_t));
        }
#line 2595 "parser.cpp"
        break;

        case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 293 "parser.y"
        {
            fprintf(stderr, "destroy update expr array\n");
            if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
                for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
                    delete ptr;
                }
                delete (((*yyvaluep).update_expr_array_t));
            }
        }
#line 2609 "parser.cpp"
        break;

        case YYSYMBOL_update_expr: /* update_expr  */
#line 286 "parser.y"
        {
            fprintf(stderr, "destroy update expr\n");
            if (((*yyvaluep).update_expr_t) != nullptr) {
                delete ((*yyvaluep).update_expr_t);
            }
        }
#line 2620 "parser.cpp"
        break;

        case YYSYMBOL_select_statement: /* select_statement  */
#line 368 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2630 "parser.cpp"
        break;

        case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 368 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2640 "parser.cpp"
        break;

        case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 368 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2650 "parser.cpp"
        break;

        case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 368 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2660 "parser.cpp"
        break;

        case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 368 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2670 "parser.cpp"
        break;

        case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 368 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2680 "parser.cpp"
        break;

        case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 276 "parser.y"
        {
            fprintf(stderr, "destroy order by expr list\n");
            if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
                for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
                    delete ptr;
                }
                delete (((*yyvaluep).order_by_expr_list_t));
            }
        }
#line 2694 "parser.cpp"
        break;

        case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 276 "parser.y"
        {
            fprintf(stderr, "destroy order by expr list\n");
            if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
                for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
                    delete ptr;
                }
                delete (((*yyvaluep).order_by_expr_list_t));
            }
        }
#line 2708 "parser.cpp"
        break;

        case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 356 "parser.y"
        {
            fprintf(stderr, "destroy order by expr\n");
            delete ((*yyvaluep).order_by_expr_t)->expr_;
            delete ((*yyvaluep).order_by_expr_t);
        }
#line 2718 "parser.cpp"
        break;

        case YYSYMBOL_limit_expr: /* limit_expr  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2726 "parser.cpp"
        break;

        case YYSYMBOL_offset_expr: /* offset_expr  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2734 "parser.cpp"
        break;

        case YYSYMBOL_highlight_clause: /* highlight_clause  */
#line 256 "parser.y"
        {
            fprintf(stderr, "destroy expression array\n");
            if ((((*yyvaluep).expr_array_t)) != nullptr) {
                for (auto ptr : *(((*yyvaluep).expr_array_t))) {
                    delete ptr;
                }
                delete (((*yyvaluep).expr_array_t));
            }
        }
#line 2748 "parser.cpp"
        break;

        case YYSYMBOL_from_clause: /* from_clause  */
#line 351 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2757 "parser.cpp"
        break;

        case YYSYMBOL_search_clause: /* search_clause  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2765 "parser.cpp"
        break;

        case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2773 "parser.cpp"
        break;

        case YYSYMBOL_where_clause: /* where_clause  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2781 "parser.cpp"
        break;

        case YYSYMBOL_having_clause: /* having_clause  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2789 "parser.cpp"
        break;

        case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 256 "parser.y"
        {
            fprintf(stderr, "destroy expression array\n");
            if ((((*yyvaluep).expr_array_t)) != nullptr) {
                for (auto ptr : *(((*yyvaluep).expr_array_t))) {
                    delete ptr;
                }
                delete (((*yyvaluep).expr_array_t));
            }
        }
#line 2803 "parser.cpp"
        break;

        case YYSYMBOL_table_reference: /* table_reference  */
#line 351 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2812 "parser.cpp"
        break;

        case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 351 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2821 "parser.cpp"
        break;

        case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 351 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2830 "parser.cpp"
        break;

        case YYSYMBOL_table_name: /* table_name  */
#line 313 "parser.y"
        {
            fprintf(stderr, "destroy table table_name\n");
            if ((((*yyvaluep).table_name_t)) != nullptr) {
                free(((*yyvaluep).table_name_t)->schema_name_ptr_);
                free(((*yyvaluep).table_name_t)->table_name_ptr_);
                delete (((*yyvaluep).table_name_t));
            }
        }
#line 2843 "parser.cpp"
        break;

        case YYSYMBOL_table_alias: /* table_alias  */
#line 346 "parser.y"
        {
            fprintf(stderr, "destroy table alias\n");
            delete (((*yyvaluep).table_alias_t));
        }
#line 2852 "parser.cpp"
        break;

        case YYSYMBOL_with_clause: /* with_clause  */
#line 303 "parser.y"
        {
            fprintf(stderr, "destroy with expr list\n");
            if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
                for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
                    delete ptr;
                }
                delete (((*yyvaluep).with_expr_list_t));
            }
        }
#line 2866 "parser.cpp"
        break;

        case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 303 "parser.y"
        {
            fprintf(stderr, "destroy with expr list\n");
            if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
                for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
                    delete ptr;
                }
                delete (((*yyvaluep).with_expr_list_t));
            }
        }
#line 2880 "parser.cpp"
        break;

        case YYSYMBOL_with_expr: /* with_expr  */
#line 362 "parser.y"
        {
            fprintf(stderr, "destroy with expr\n");
            delete ((*yyvaluep).with_expr_t)->select_;
            delete ((*yyvaluep).with_expr_t);
        }
#line 2890 "parser.cpp"
        break;

        case YYSYMBOL_join_clause: /* join_clause  */
#line 351 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2899 "parser.cpp"
        break;

        case YYSYMBOL_expr_array: /* expr_array  */
#line 256 "parser.y"
        {
            fprintf(stderr, "destroy expression array\n");
            if ((((*yyvaluep).expr_array_t)) != nullptr) {
                for (auto ptr : *(((*yyvaluep).expr_array_t))) {
                    delete ptr;
                }
                delete (((*yyvaluep).expr_array_t));
            }
        }
#line 2913 "parser.cpp"
        break;

        case YYSYMBOL_insert_row_list: /* insert_row_list  */
#line 266 "parser.y"
        {
            fprintf(stderr, "destroy insert row list\n");
            if ((((*yyvaluep).insert_row_list_t)) != nullptr) {
                for (auto ptr : *(((*yyvaluep).insert_row_list_t))) {
                    delete ptr;
                }
                delete (((*yyvaluep).insert_row_list_t));
            }
        }
#line 2927 "parser.cpp"
        break;

        case YYSYMBOL_expr_alias: /* expr_alias  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2935 "parser.cpp"
        break;

        case YYSYMBOL_expr: /* expr  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2943 "parser.cpp"
        break;

        case YYSYMBOL_operand: /* operand  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2951 "parser.cpp"
        break;

        case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2959 "parser.cpp"
        break;

        case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2967 "parser.cpp"
        break;

        case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2975 "parser.cpp"
        break;

        case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2983 "parser.cpp"
        break;

        case YYSYMBOL_query_expr: /* query_expr  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2991 "parser.cpp"
        break;

        case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2999 "parser.cpp"
        break;

        case YYSYMBOL_sub_search: /* sub_search  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3007 "parser.cpp"
        break;

        case YYSYMBOL_sub_search_array: /* sub_search_array  */
#line 256 "parser.y"
        {
            fprintf(stderr, "destroy expression array\n");
            if ((((*yyvaluep).expr_array_t)) != nullptr) {
                for (auto ptr : *(((*yyvaluep).expr_array_t))) {
                    delete ptr;
                }
                delete (((*yyvaluep).expr_array_t));
            }
        }
#line 3021 "parser.cpp"
        break;

        case YYSYMBOL_function_expr: /* function_expr  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3029 "parser.cpp"
        break;

        case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3037 "parser.cpp"
        break;

        case YYSYMBOL_between_expr: /* between_expr  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3045 "parser.cpp"
        break;

        case YYSYMBOL_in_expr: /* in_expr  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3053 "parser.cpp"
        break;

        case YYSYMBOL_case_expr: /* case_expr  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3061 "parser.cpp"
        break;

        case YYSYMBOL_case_check_array: /* case_check_array  */
#line 374 "parser.y"
        {
            fprintf(stderr, "destroy case check array\n");
            if (((*yyvaluep).case_check_array_t) != nullptr) {
                for (auto ptr : *(((*yyvaluep).case_check_array_t))) {
                    delete ptr;
                }
            }
        }
#line 3074 "parser.cpp"
        break;

        case YYSYMBOL_cast_expr: /* cast_expr  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3082 "parser.cpp"
        break;

        case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3090 "parser.cpp"
        break;

        case YYSYMBOL_column_expr: /* column_expr  */
#line 338 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3098 "parser.cpp"
        break;

        case YYSYMBOL_constant_expr: /* constant_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3106 "parser.cpp"
        break;

        case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3114 "parser.cpp"
        break;

        case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3122 "parser.cpp"
        break;

        case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3130 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3138 "parser.cpp"
        break;

        case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3146 "parser.cpp"
        break;

        case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3154 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3162 "parser.cpp"
        break;

        case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3170 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3178 "parser.cpp"
        break;

        case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3186 "parser.cpp"
        break;

        case YYSYMBOL_curly_brackets_expr: /* curly_brackets_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3194 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_curly_brackets_expr: /* unclosed_curly_brackets_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3202 "parser.cpp"
        break;

        case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 383 "parser.y"
        {
            delete (((*yyvaluep).int_sparse_ele_t));
        }
#line 3210 "parser.cpp"
        break;

        case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 387 "parser.y"
        {
            delete (((*yyvaluep).float_sparse_ele_t));
        }
#line 3218 "parser.cpp"
        break;

        case YYSYMBOL_array_expr: /* array_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3226 "parser.cpp"
        break;

        case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3234 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3242 "parser.cpp"
        break;

        case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3250 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3258 "parser.cpp"
        break;

        case YYSYMBOL_interval_expr: /* interval_expr  */
#line 342 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3266 "parser.cpp"
        break;

        case YYSYMBOL_file_path: /* file_path  */
#line 329 "parser.y"
        {
            free(((*yyvaluep).str_value));
        }
#line 3274 "parser.cpp"
        break;

        case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 219 "parser.y"
        {
            fprintf(stderr, "destroy if not exists info\n");
            if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
                delete (((*yyvaluep).if_not_exists_info_t));
            }
        }
#line 3285 "parser.cpp"
        break;

        case YYSYMBOL_with_index_param_list: /* with_index_param_list  */
#line 202 "parser.y"
        {
            fprintf(stderr, "destroy create index param list\n");
            if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
                for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
                    delete ptr;
                }
                delete (((*yyvaluep).with_index_param_list_t));
            }
        }
#line 3299 "parser.cpp"
        break;

        case YYSYMBOL_optional_table_properties_list: /* optional_table_properties_list  */
#line 202 "parser.y"
        {
            fprintf(stderr, "destroy create index param list\n");
            if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
                for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
                    delete ptr;
                }
                delete (((*yyvaluep).with_index_param_list_t));
            }
        }
#line 3313 "parser.cpp"
        break;

        case YYSYMBOL_index_info: /* index_info  */
#line 195 "parser.y"
        {
            fprintf(stderr, "destroy index info\n");
            if ((((*yyvaluep).index_info_t)) != nullptr) {
                delete (((*yyvaluep).index_info_t));
            }
        }
#line 3324 "parser.cpp"
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
#line 87 "parser.y"
    {
        // Initialize
        yylloc.first_column = 0;
        yylloc.last_column = 0;
        yylloc.first_line = 0;
        yylloc.last_line = 0;
        yylloc.total_column = 0;
        yylloc.string_length = 0;
    }

#line 3432 "parser.cpp"

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
#line 519 "parser.y"
        {
            result->statements_ptr_ = (yyvsp[-1].stmt_array);
        }
#line 3647 "parser.cpp"
        break;

        case 3: /* statement_list: statement  */
#line 523 "parser.y"
        {
            (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
            yylloc.string_length = 0;
            (yyval.stmt_array) = new std::vector<infinity::BaseStatement *>();
            (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
        }
#line 3658 "parser.cpp"
        break;

        case 4: /* statement_list: statement_list ';' statement  */
#line 529 "parser.y"
        {
            (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
            yylloc.string_length = 0;
            (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
            (yyval.stmt_array) = (yyvsp[-2].stmt_array);
        }
#line 3669 "parser.cpp"
        break;

        case 5: /* statement: create_statement  */
#line 536 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].create_stmt);
        }
#line 3675 "parser.cpp"
        break;

        case 6: /* statement: drop_statement  */
#line 537 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].drop_stmt);
        }
#line 3681 "parser.cpp"
        break;

        case 7: /* statement: copy_statement  */
#line 538 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].copy_stmt);
        }
#line 3687 "parser.cpp"
        break;

        case 8: /* statement: show_statement  */
#line 539 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].show_stmt);
        }
#line 3693 "parser.cpp"
        break;

        case 9: /* statement: select_statement  */
#line 540 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].select_stmt);
        }
#line 3699 "parser.cpp"
        break;

        case 10: /* statement: delete_statement  */
#line 541 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].delete_stmt);
        }
#line 3705 "parser.cpp"
        break;

        case 11: /* statement: update_statement  */
#line 542 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].update_stmt);
        }
#line 3711 "parser.cpp"
        break;

        case 12: /* statement: insert_statement  */
#line 543 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].insert_stmt);
        }
#line 3717 "parser.cpp"
        break;

        case 13: /* statement: explain_statement  */
#line 544 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].explain_stmt);
        }
#line 3723 "parser.cpp"
        break;

        case 14: /* statement: flush_statement  */
#line 545 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].flush_stmt);
        }
#line 3729 "parser.cpp"
        break;

        case 15: /* statement: optimize_statement  */
#line 546 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].optimize_stmt);
        }
#line 3735 "parser.cpp"
        break;

        case 16: /* statement: command_statement  */
#line 547 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].command_stmt);
        }
#line 3741 "parser.cpp"
        break;

        case 17: /* statement: compact_statement  */
#line 548 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].compact_stmt);
        }
#line 3747 "parser.cpp"
        break;

        case 18: /* statement: admin_statement  */
#line 549 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].admin_stmt);
        }
#line 3753 "parser.cpp"
        break;

        case 19: /* statement: alter_statement  */
#line 550 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].alter_stmt);
        }
#line 3759 "parser.cpp"
        break;

        case 20: /* statement: check_statement  */
#line 551 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].check_stmt);
        }
#line 3765 "parser.cpp"
        break;

        case 21: /* explainable_statement: create_statement  */
#line 553 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].create_stmt);
        }
#line 3771 "parser.cpp"
        break;

        case 22: /* explainable_statement: drop_statement  */
#line 554 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].drop_stmt);
        }
#line 3777 "parser.cpp"
        break;

        case 23: /* explainable_statement: copy_statement  */
#line 555 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].copy_stmt);
        }
#line 3783 "parser.cpp"
        break;

        case 24: /* explainable_statement: show_statement  */
#line 556 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].show_stmt);
        }
#line 3789 "parser.cpp"
        break;

        case 25: /* explainable_statement: select_statement  */
#line 557 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].select_stmt);
        }
#line 3795 "parser.cpp"
        break;

        case 26: /* explainable_statement: delete_statement  */
#line 558 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].delete_stmt);
        }
#line 3801 "parser.cpp"
        break;

        case 27: /* explainable_statement: update_statement  */
#line 559 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].update_stmt);
        }
#line 3807 "parser.cpp"
        break;

        case 28: /* explainable_statement: insert_statement  */
#line 560 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].insert_stmt);
        }
#line 3813 "parser.cpp"
        break;

        case 29: /* explainable_statement: flush_statement  */
#line 561 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].flush_stmt);
        }
#line 3819 "parser.cpp"
        break;

        case 30: /* explainable_statement: optimize_statement  */
#line 562 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].optimize_stmt);
        }
#line 3825 "parser.cpp"
        break;

        case 31: /* explainable_statement: command_statement  */
#line 563 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].command_stmt);
        }
#line 3831 "parser.cpp"
        break;

        case 32: /* explainable_statement: compact_statement  */
#line 564 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].compact_stmt);
        }
#line 3837 "parser.cpp"
        break;

        case 33: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER COMMENT STRING  */
#line 571 "parser.y"
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
#line 3859 "parser.cpp"
        break;

        case 34: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 588 "parser.y"
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
#line 3879 "parser.cpp"
        break;

        case 35: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 605 "parser.y"
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
#line 3897 "parser.cpp"
        break;

        case 36: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 621 "parser.y"
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
#line 3930 "parser.cpp"
        break;

        case 37: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 650 "parser.y"
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
#line 3950 "parser.cpp"
        break;

        case 38: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list COMMENT STRING
                  */
#line 665 "parser.y"
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
#line 3986 "parser.cpp"
        break;

        case 39: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement COMMENT STRING  */
#line 697 "parser.y"
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
#line 4008 "parser.cpp"
        break;

        case 40: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 715 "parser.y"
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
#line 4029 "parser.cpp"
        break;

        case 41: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info  */
#line 733 "parser.y"
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
#line 4062 "parser.cpp"
        break;

        case 42: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info COMMENT STRING  */
#line 761 "parser.y"
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
#line 4097 "parser.cpp"
        break;

        case 43: /* table_element_array: table_element  */
#line 792 "parser.y"
        {
            (yyval.table_element_array_t) = new std::vector<infinity::TableElement *>();
            (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
        }
#line 4106 "parser.cpp"
        break;

        case 44: /* table_element_array: table_element_array ',' table_element  */
#line 796 "parser.y"
        {
            (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
            (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
        }
#line 4115 "parser.cpp"
        break;

        case 45: /* column_def_array: table_column  */
#line 801 "parser.y"
        {
            (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef *>();
            (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
        }
#line 4124 "parser.cpp"
        break;

        case 46: /* column_def_array: column_def_array ',' table_column  */
#line 805 "parser.y"
        {
            (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
            (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
        }
#line 4133 "parser.cpp"
        break;

        case 47: /* table_element: table_column  */
#line 811 "parser.y"
        {
            (yyval.table_element_t) = (yyvsp[0].table_column_t);
        }
#line 4141 "parser.cpp"
        break;

        case 48: /* table_element: table_constraint  */
#line 814 "parser.y"
        {
            (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
        }
#line 4149 "parser.cpp"
        break;

        case 49: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 821 "parser.y"
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
#line 4174 "parser.cpp"
        break;

        case 50: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 841 "parser.y"
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
#line 4201 "parser.cpp"
        break;

        case 51: /* table_column: IDENTIFIER column_type with_index_param_list default_expr COMMENT STRING  */
#line 863 "parser.y"
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
#line 4230 "parser.cpp"
        break;

        case 52: /* table_column: IDENTIFIER column_type column_constraints default_expr COMMENT STRING  */
#line 887 "parser.y"
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
#line 4260 "parser.cpp"
        break;

        case 53: /* column_type_array: column_type  */
#line 913 "parser.y"
        {
            (yyval.column_type_array_t) = new std::vector<std::unique_ptr<infinity::ColumnType>>();
            (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
        }
#line 4269 "parser.cpp"
        break;

        case 54: /* column_type_array: column_type_array ',' column_type  */
#line 917 "parser.y"
        {
            (yyval.column_type_array_t) = (yyvsp[-2].column_type_array_t);
            (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
        }
#line 4278 "parser.cpp"
        break;

        case 55: /* column_type: BOOLEAN  */
#line 923 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4284 "parser.cpp"
        break;

        case 56: /* column_type: TINYINT  */
#line 924 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4290 "parser.cpp"
        break;

        case 57: /* column_type: SMALLINT  */
#line 925 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4296 "parser.cpp"
        break;

        case 58: /* column_type: INTEGER  */
#line 926 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4302 "parser.cpp"
        break;

        case 59: /* column_type: INT  */
#line 927 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4308 "parser.cpp"
        break;

        case 60: /* column_type: BIGINT  */
#line 928 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4314 "parser.cpp"
        break;

        case 61: /* column_type: HUGEINT  */
#line 929 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4320 "parser.cpp"
        break;

        case 62: /* column_type: FLOAT  */
#line 930 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4326 "parser.cpp"
        break;

        case 63: /* column_type: REAL  */
#line 931 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4332 "parser.cpp"
        break;

        case 64: /* column_type: DOUBLE  */
#line 932 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4338 "parser.cpp"
        break;

        case 65: /* column_type: FLOAT16  */
#line 933 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4344 "parser.cpp"
        break;

        case 66: /* column_type: BFLOAT16  */
#line 934 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4350 "parser.cpp"
        break;

        case 67: /* column_type: DATE  */
#line 935 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4356 "parser.cpp"
        break;

        case 68: /* column_type: TIME  */
#line 936 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4362 "parser.cpp"
        break;

        case 69: /* column_type: DATETIME  */
#line 937 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4368 "parser.cpp"
        break;

        case 70: /* column_type: TIMESTAMP  */
#line 938 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4374 "parser.cpp"
        break;

        case 71: /* column_type: UUID  */
#line 939 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4380 "parser.cpp"
        break;

        case 72: /* column_type: POINT  */
#line 940 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4386 "parser.cpp"
        break;

        case 73: /* column_type: LINE  */
#line 941 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4392 "parser.cpp"
        break;

        case 74: /* column_type: LSEG  */
#line 942 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4398 "parser.cpp"
        break;

        case 75: /* column_type: BOX  */
#line 943 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4404 "parser.cpp"
        break;

        case 76: /* column_type: CIRCLE  */
#line 946 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4410 "parser.cpp"
        break;

        case 77: /* column_type: VARCHAR  */
#line 948 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4416 "parser.cpp"
        break;

        case 78: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 949 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal,
                                                             0,
                                                             (yyvsp[-3].long_value),
                                                             (yyvsp[-1].long_value),
                                                             infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4422 "parser.cpp"
        break;

        case 79: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 950 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4428 "parser.cpp"
        break;

        case 80: /* column_type: DECIMAL  */
#line 951 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4434 "parser.cpp"
        break;

        case 81: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 954 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4440 "parser.cpp"
        break;

        case 82: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 955 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4446 "parser.cpp"
        break;

        case 83: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 956 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4452 "parser.cpp"
        break;

        case 84: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 957 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4458 "parser.cpp"
        break;

        case 85: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 958 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4464 "parser.cpp"
        break;

        case 86: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 959 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4470 "parser.cpp"
        break;

        case 87: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 960 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4476 "parser.cpp"
        break;

        case 88: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 961 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4482 "parser.cpp"
        break;

        case 89: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 962 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4488 "parser.cpp"
        break;

        case 90: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 963 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4494 "parser.cpp"
        break;

        case 91: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 964 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4500 "parser.cpp"
        break;

        case 92: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 965 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4506 "parser.cpp"
        break;

        case 93: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 966 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4512 "parser.cpp"
        break;

        case 94: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 967 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4518 "parser.cpp"
        break;

        case 95: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 968 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4524 "parser.cpp"
        break;

        case 96: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 969 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4530 "parser.cpp"
        break;

        case 97: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 970 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4536 "parser.cpp"
        break;

        case 98: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 971 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4542 "parser.cpp"
        break;

        case 99: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 972 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4548 "parser.cpp"
        break;

        case 100: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 973 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector,
                                                             (yyvsp[-1].long_value),
                                                             0,
                                                             0,
                                                             infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4554 "parser.cpp"
        break;

        case 101: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 974 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector,
                                                             (yyvsp[-1].long_value),
                                                             0,
                                                             0,
                                                             infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4560 "parser.cpp"
        break;

        case 102: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 975 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4566 "parser.cpp"
        break;

        case 103: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 976 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4572 "parser.cpp"
        break;

        case 104: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 977 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4578 "parser.cpp"
        break;

        case 105: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 978 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4584 "parser.cpp"
        break;

        case 106: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 979 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4590 "parser.cpp"
        break;

        case 107: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 980 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4596 "parser.cpp"
        break;

        case 108: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 981 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4602 "parser.cpp"
        break;

        case 109: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 982 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4608 "parser.cpp"
        break;

        case 110: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 983 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4614 "parser.cpp"
        break;

        case 111: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 984 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4620 "parser.cpp"
        break;

        case 112: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 985 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4626 "parser.cpp"
        break;

        case 113: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 986 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4632 "parser.cpp"
        break;

        case 114: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 987 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4638 "parser.cpp"
        break;

        case 115: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 988 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4644 "parser.cpp"
        break;

        case 116: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 989 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4650 "parser.cpp"
        break;

        case 117: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 990 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4656 "parser.cpp"
        break;

        case 118: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 991 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4662 "parser.cpp"
        break;

        case 119: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 992 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4668 "parser.cpp"
        break;

        case 120: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 993 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4674 "parser.cpp"
        break;

        case 121: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 994 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4680 "parser.cpp"
        break;

        case 122: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 995 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray,
                                                             (yyvsp[-1].long_value),
                                                             0,
                                                             0,
                                                             infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4686 "parser.cpp"
        break;

        case 123: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 996 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray,
                                                             (yyvsp[-1].long_value),
                                                             0,
                                                             0,
                                                             infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4692 "parser.cpp"
        break;

        case 124: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 997 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4698 "parser.cpp"
        break;

        case 125: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 998 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4704 "parser.cpp"
        break;

        case 126: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 999 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4710 "parser.cpp"
        break;

        case 127: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1000 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4716 "parser.cpp"
        break;

        case 128: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1001 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4722 "parser.cpp"
        break;

        case 129: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1002 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4728 "parser.cpp"
        break;

        case 130: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1003 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4734 "parser.cpp"
        break;

        case 131: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1004 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4740 "parser.cpp"
        break;

        case 132: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1005 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4746 "parser.cpp"
        break;

        case 133: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1006 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4752 "parser.cpp"
        break;

        case 134: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1007 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4758 "parser.cpp"
        break;

        case 135: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1008 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4764 "parser.cpp"
        break;

        case 136: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1009 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4770 "parser.cpp"
        break;

        case 137: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4776 "parser.cpp"
        break;

        case 138: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1011 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4782 "parser.cpp"
        break;

        case 139: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1012 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4788 "parser.cpp"
        break;

        case 140: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1013 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4794 "parser.cpp"
        break;

        case 141: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1014 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4800 "parser.cpp"
        break;

        case 142: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4806 "parser.cpp"
        break;

        case 143: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4812 "parser.cpp"
        break;

        case 144: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1017 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4818 "parser.cpp"
        break;

        case 145: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1018 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4824 "parser.cpp"
        break;

        case 146: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1019 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4830 "parser.cpp"
        break;

        case 147: /* column_type: ARRAY '(' column_type ')'  */
#line 1020 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kArray, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
            (yyval.column_type_t)->element_types_.emplace_back((yyvsp[-1].column_type_t));
        }
#line 4839 "parser.cpp"
        break;

        case 148: /* column_type: TUPLE '(' column_type_array ')'  */
#line 1024 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTuple, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
            (yyval.column_type_t)->element_types_ = std::move(*((yyvsp[-1].column_type_array_t)));
            delete (yyvsp[-1].column_type_array_t);
        }
#line 4849 "parser.cpp"
        break;

        case 149: /* column_constraints: column_constraint  */
#line 1047 "parser.y"
        {
            (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
            (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
        }
#line 4858 "parser.cpp"
        break;

        case 150: /* column_constraints: column_constraints column_constraint  */
#line 1051 "parser.y"
        {
            if ((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
                yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
                delete (yyvsp[-1].column_constraints_t);
                YYERROR;
            }
            (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
            (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
        }
#line 4872 "parser.cpp"
        break;

        case 151: /* column_constraint: PRIMARY KEY  */
#line 1061 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
        }
#line 4880 "parser.cpp"
        break;

        case 152: /* column_constraint: UNIQUE  */
#line 1064 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
        }
#line 4888 "parser.cpp"
        break;

        case 153: /* column_constraint: NULLABLE  */
#line 1067 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
        }
#line 4896 "parser.cpp"
        break;

        case 154: /* column_constraint: NOT NULLABLE  */
#line 1070 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
        }
#line 4904 "parser.cpp"
        break;

        case 155: /* default_expr: DEFAULT constant_expr  */
#line 1074 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 4912 "parser.cpp"
        break;

        case 156: /* default_expr: %empty  */
#line 1077 "parser.y"
        {
            (yyval.const_expr_t) = nullptr;
        }
#line 4920 "parser.cpp"
        break;

        case 157: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1082 "parser.y"
        {
            (yyval.table_constraint_t) = new infinity::TableConstraint();
            (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
            (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
        }
#line 4930 "parser.cpp"
        break;

        case 158: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1087 "parser.y"
        {
            (yyval.table_constraint_t) = new infinity::TableConstraint();
            (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
            (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
        }
#line 4940 "parser.cpp"
        break;

        case 159: /* identifier_array: IDENTIFIER  */
#line 1094 "parser.y"
        {
            (yyval.identifier_array_t) = new std::vector<std::string>();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 4951 "parser.cpp"
        break;

        case 160: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1100 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
            (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
        }
#line 4962 "parser.cpp"
        break;

        case 161: /* delete_statement: DELETE FROM table_name where_clause  */
#line 1110 "parser.y"
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
#line 4979 "parser.cpp"
        break;

        case 162: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES insert_row_list  */
#line 1126 "parser.y"
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
#line 5022 "parser.cpp"
        break;

        case 163: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1164 "parser.y"
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
#line 5042 "parser.cpp"
        break;

        case 164: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1180 "parser.y"
        {
            (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
        }
#line 5050 "parser.cpp"
        break;

        case 165: /* optional_identifier_array: %empty  */
#line 1183 "parser.y"
        {
            (yyval.identifier_array_t) = nullptr;
        }
#line 5058 "parser.cpp"
        break;

        case 166: /* explain_statement: EXPLAIN IDENTIFIER explainable_statement  */
#line 1190 "parser.y"
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
#line 5081 "parser.cpp"
        break;

        case 167: /* explain_statement: EXPLAIN explainable_statement  */
#line 1207 "parser.y"
        {
            (yyval.explain_stmt) = new infinity::ExplainStatement();
            (yyval.explain_stmt)->type_ = infinity::ExplainType::kPhysical;
            (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
        }
#line 5091 "parser.cpp"
        break;

        case 168: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1216 "parser.y"
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
#line 5108 "parser.cpp"
        break;

        case 169: /* update_expr_array: update_expr  */
#line 1229 "parser.y"
        {
            (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr *>();
            (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
        }
#line 5117 "parser.cpp"
        break;

        case 170: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1233 "parser.y"
        {
            (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
            (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
        }
#line 5126 "parser.cpp"
        break;

        case 171: /* update_expr: IDENTIFIER '=' expr  */
#line 1238 "parser.y"
        {
            (yyval.update_expr_t) = new infinity::UpdateExpr();
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));
            (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
        }
#line 5138 "parser.cpp"
        break;

        case 172: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
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
#line 5154 "parser.cpp"
        break;

        case 173: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1264 "parser.y"
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
#line 5172 "parser.cpp"
        break;

        case 174: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1279 "parser.y"
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
#line 5190 "parser.cpp"
        break;

        case 175: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1294 "parser.y"
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
#line 5208 "parser.cpp"
        break;

        case 176: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1309 "parser.y"
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
#line 5231 "parser.cpp"
        break;

        case 177: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1332 "parser.y"
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
#line 5289 "parser.cpp"
        break;

        case 178: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1385 "parser.y"
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
#line 5349 "parser.cpp"
        break;

        case 179: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1440 "parser.y"
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
#line 5401 "parser.cpp"
        break;

        case 180: /* select_statement: select_without_paren  */
#line 1491 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[0].select_stmt);
        }
#line 5409 "parser.cpp"
        break;

        case 181: /* select_statement: select_with_paren  */
#line 1494 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[0].select_stmt);
        }
#line 5417 "parser.cpp"
        break;

        case 182: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1497 "parser.y"
        {
            infinity::SelectStatement *node = (yyvsp[-2].select_stmt);
            while (node->nested_select_ != nullptr) {
                node = node->nested_select_;
            }
            node->set_op_ = (yyvsp[-1].set_operator_t);
            node->nested_select_ = (yyvsp[0].select_stmt);
            (yyval.select_stmt) = (yyvsp[-2].select_stmt);
        }
#line 5431 "parser.cpp"
        break;

        case 183: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1506 "parser.y"
        {
            infinity::SelectStatement *node = (yyvsp[-2].select_stmt);
            while (node->nested_select_ != nullptr) {
                node = node->nested_select_;
            }
            node->set_op_ = (yyvsp[-1].set_operator_t);
            node->nested_select_ = (yyvsp[0].select_stmt);
            (yyval.select_stmt) = (yyvsp[-2].select_stmt);
        }
#line 5445 "parser.cpp"
        break;

        case 184: /* select_with_paren: '(' select_without_paren ')'  */
#line 1516 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5453 "parser.cpp"
        break;

        case 185: /* select_with_paren: '(' select_with_paren ')'  */
#line 1519 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5461 "parser.cpp"
        break;

        case 186: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1523 "parser.y"
        {
            (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
            (yyval.select_stmt) = (yyvsp[0].select_stmt);
        }
#line 5470 "parser.cpp"
        break;

        case 187: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1528 "parser.y"
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
#line 5501 "parser.cpp"
        break;

        case 188: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1555 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5509 "parser.cpp"
        break;

        case 189: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1558 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5517 "parser.cpp"
        break;

        case 190: /* select_clause_without_modifier: SELECT distinct expr_array highlight_clause from_clause search_clause where_clause
                     group_by_clause having_clause  */
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

            if ((yyval.select_stmt)->group_by_list_ == nullptr && (yyval.select_stmt)->having_expr_ != nullptr) {
                yyerror(&yyloc, scanner, result, "HAVING clause should follow after GROUP BY clause");
                YYERROR;
            }
        }
#line 5538 "parser.cpp"
        break;

        case 191: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1580 "parser.y"
        {
            (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
        }
#line 5546 "parser.cpp"
        break;

        case 192: /* order_by_clause: %empty  */
#line 1583 "parser.y"
        {
            (yyval.order_by_expr_list_t) = nullptr;
        }
#line 5554 "parser.cpp"
        break;

        case 193: /* order_by_expr_list: order_by_expr  */
#line 1587 "parser.y"
        {
            (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr *>();
            (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
        }
#line 5563 "parser.cpp"
        break;

        case 194: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1591 "parser.y"
        {
            (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
            (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
        }
#line 5572 "parser.cpp"
        break;

        case 195: /* order_by_expr: expr order_by_type  */
#line 1596 "parser.y"
        {
            (yyval.order_by_expr_t) = new infinity::OrderByExpr();
            (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
            (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
        }
#line 5582 "parser.cpp"
        break;

        case 196: /* order_by_type: ASC  */
#line 1602 "parser.y"
        {
            (yyval.order_by_type_t) = infinity::kAsc;
        }
#line 5590 "parser.cpp"
        break;

        case 197: /* order_by_type: DESC  */
#line 1605 "parser.y"
        {
            (yyval.order_by_type_t) = infinity::kDesc;
        }
#line 5598 "parser.cpp"
        break;

        case 198: /* order_by_type: %empty  */
#line 1608 "parser.y"
        {
            (yyval.order_by_type_t) = infinity::kAsc;
        }
#line 5606 "parser.cpp"
        break;

        case 199: /* limit_expr: LIMIT expr  */
#line 1612 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5614 "parser.cpp"
        break;

        case 200: /* limit_expr: %empty  */
#line 1616 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5620 "parser.cpp"
        break;

        case 201: /* offset_expr: OFFSET expr  */
#line 1618 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5628 "parser.cpp"
        break;

        case 202: /* offset_expr: %empty  */
#line 1622 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5634 "parser.cpp"
        break;

        case 203: /* distinct: DISTINCT  */
#line 1624 "parser.y"
        {
            (yyval.bool_value) = true;
        }
#line 5642 "parser.cpp"
        break;

        case 204: /* distinct: %empty  */
#line 1627 "parser.y"
        {
            (yyval.bool_value) = false;
        }
#line 5650 "parser.cpp"
        break;

        case 205: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1631 "parser.y"
        {
            (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
        }
#line 5658 "parser.cpp"
        break;

        case 206: /* highlight_clause: %empty  */
#line 1634 "parser.y"
        {
            (yyval.expr_array_t) = nullptr;
        }
#line 5666 "parser.cpp"
        break;

        case 207: /* from_clause: FROM table_reference  */
#line 1638 "parser.y"
        {
            (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
        }
#line 5674 "parser.cpp"
        break;

        case 208: /* from_clause: %empty  */
#line 1641 "parser.y"
        {
            (yyval.table_reference_t) = nullptr;
        }
#line 5682 "parser.cpp"
        break;

        case 209: /* search_clause: SEARCH sub_search_array  */
#line 1645 "parser.y"
        {
            infinity::SearchExpr *search_expr = new infinity::SearchExpr();
            search_expr->SetExprs((yyvsp[0].expr_array_t));
            (yyval.expr_t) = search_expr;
        }
#line 5692 "parser.cpp"
        break;

        case 210: /* search_clause: %empty  */
#line 1650 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5700 "parser.cpp"
        break;

        case 211: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1654 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5708 "parser.cpp"
        break;

        case 212: /* optional_search_filter_expr: %empty  */
#line 1657 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5716 "parser.cpp"
        break;

        case 213: /* where_clause: WHERE expr  */
#line 1661 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5724 "parser.cpp"
        break;

        case 214: /* where_clause: %empty  */
#line 1664 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5732 "parser.cpp"
        break;

        case 215: /* having_clause: HAVING expr  */
#line 1668 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5740 "parser.cpp"
        break;

        case 216: /* having_clause: %empty  */
#line 1671 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5748 "parser.cpp"
        break;

        case 217: /* group_by_clause: GROUP BY expr_array  */
#line 1675 "parser.y"
        {
            (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
        }
#line 5756 "parser.cpp"
        break;

        case 218: /* group_by_clause: %empty  */
#line 1678 "parser.y"
        {
            (yyval.expr_array_t) = nullptr;
        }
#line 5764 "parser.cpp"
        break;

        case 219: /* set_operator: UNION  */
#line 1682 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
        }
#line 5772 "parser.cpp"
        break;

        case 220: /* set_operator: UNION ALL  */
#line 1685 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
        }
#line 5780 "parser.cpp"
        break;

        case 221: /* set_operator: INTERSECT  */
#line 1688 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
        }
#line 5788 "parser.cpp"
        break;

        case 222: /* set_operator: EXCEPT  */
#line 1691 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
        }
#line 5796 "parser.cpp"
        break;

        case 223: /* table_reference: table_reference_unit  */
#line 1699 "parser.y"
        {
            (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
        }
#line 5804 "parser.cpp"
        break;

        case 224: /* table_reference: table_reference ',' table_reference_unit  */
#line 1702 "parser.y"
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
#line 5822 "parser.cpp"
        break;

        case 227: /* table_reference_name: table_name table_alias  */
#line 1719 "parser.y"
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
#line 5840 "parser.cpp"
        break;

        case 228: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1733 "parser.y"
        {
            infinity::SubqueryReference *subquery_reference = new infinity::SubqueryReference();
            subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
            subquery_reference->alias_ = (yyvsp[0].table_alias_t);
            (yyval.table_reference_t) = subquery_reference;
        }
#line 5851 "parser.cpp"
        break;

        case 229: /* table_name: IDENTIFIER  */
#line 1742 "parser.y"
        {
            (yyval.table_name_t) = new infinity::TableName();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
        }
#line 5861 "parser.cpp"
        break;

        case 230: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1747 "parser.y"
        {
            (yyval.table_name_t) = new infinity::TableName();
            ParserHelper::ToLower((yyvsp[-2].str_value));
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
            (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
        }
#line 5873 "parser.cpp"
        break;

        case 231: /* table_alias: AS IDENTIFIER  */
#line 1756 "parser.y"
        {
            (yyval.table_alias_t) = new infinity::TableAlias();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
        }
#line 5883 "parser.cpp"
        break;

        case 232: /* table_alias: IDENTIFIER  */
#line 1761 "parser.y"
        {
            (yyval.table_alias_t) = new infinity::TableAlias();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
        }
#line 5893 "parser.cpp"
        break;

        case 233: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1766 "parser.y"
        {
            (yyval.table_alias_t) = new infinity::TableAlias();
            ParserHelper::ToLower((yyvsp[-3].str_value));
            (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
            (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
        }
#line 5904 "parser.cpp"
        break;

        case 234: /* table_alias: %empty  */
#line 1772 "parser.y"
        {
            (yyval.table_alias_t) = nullptr;
        }
#line 5912 "parser.cpp"
        break;

        case 235: /* with_clause: WITH with_expr_list  */
#line 1779 "parser.y"
        {
            (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
        }
#line 5920 "parser.cpp"
        break;

        case 236: /* with_clause: %empty  */
#line 1782 "parser.y"
        {
            (yyval.with_expr_list_t) = nullptr;
        }
#line 5928 "parser.cpp"
        break;

        case 237: /* with_expr_list: with_expr  */
#line 1786 "parser.y"
        {
            (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr *>();
            (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
        }
#line 5937 "parser.cpp"
        break;

        case 238: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1789 "parser.y"
        {
            (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
            (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
        }
#line 5946 "parser.cpp"
        break;

        case 239: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1794 "parser.y"
        {
            (yyval.with_expr_t) = new infinity::WithExpr();
            ParserHelper::ToLower((yyvsp[-4].str_value));
            (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
            free((yyvsp[-4].str_value));
            (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
        }
#line 5958 "parser.cpp"
        break;

        case 240: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1806 "parser.y"
        {
            infinity::JoinReference *join_reference = new infinity::JoinReference();
            join_reference->left_ = (yyvsp[-3].table_reference_t);
            join_reference->right_ = (yyvsp[0].table_reference_t);
            join_reference->join_type_ = infinity::JoinType::kNatural;
            (yyval.table_reference_t) = join_reference;
        }
#line 5970 "parser.cpp"
        break;

        case 241: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1813 "parser.y"
        {
            infinity::JoinReference *join_reference = new infinity::JoinReference();
            join_reference->left_ = (yyvsp[-5].table_reference_t);
            join_reference->right_ = (yyvsp[-2].table_reference_t);
            join_reference->join_type_ = (yyvsp[-4].join_type_t);
            join_reference->condition_ = (yyvsp[0].expr_t);
            (yyval.table_reference_t) = join_reference;
        }
#line 5983 "parser.cpp"
        break;

        case 242: /* join_type: INNER  */
#line 1827 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kInner;
        }
#line 5991 "parser.cpp"
        break;

        case 243: /* join_type: LEFT  */
#line 1830 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kLeft;
        }
#line 5999 "parser.cpp"
        break;

        case 244: /* join_type: RIGHT  */
#line 1833 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kRight;
        }
#line 6007 "parser.cpp"
        break;

        case 245: /* join_type: OUTER  */
#line 1836 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kFull;
        }
#line 6015 "parser.cpp"
        break;

        case 246: /* join_type: FULL  */
#line 1839 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kFull;
        }
#line 6023 "parser.cpp"
        break;

        case 247: /* join_type: CROSS  */
#line 1842 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kCross;
        }
#line 6031 "parser.cpp"
        break;

        case 248: /* join_type: %empty  */
#line 1845 "parser.y"
        {
        }
#line 6038 "parser.cpp"
        break;

        case 249: /* show_statement: SHOW DATABASES  */
#line 1851 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
        }
#line 6047 "parser.cpp"
        break;

        case 250: /* show_statement: SHOW TABLES  */
#line 1855 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
        }
#line 6056 "parser.cpp"
        break;

        case 251: /* show_statement: SHOW TASKS  */
#line 1859 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTasks;
        }
#line 6065 "parser.cpp"
        break;

        case 252: /* show_statement: SHOW CONFIGS  */
#line 1863 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
        }
#line 6074 "parser.cpp"
        break;

        case 253: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1867 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 6086 "parser.cpp"
        break;

        case 254: /* show_statement: SHOW PROFILES  */
#line 1874 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
        }
#line 6095 "parser.cpp"
        break;

        case 255: /* show_statement: SHOW BUFFER  */
#line 1878 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
        }
#line 6104 "parser.cpp"
        break;

        case 256: /* show_statement: SHOW MEMINDEX  */
#line 1882 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
        }
#line 6113 "parser.cpp"
        break;

        case 257: /* show_statement: SHOW QUERIES  */
#line 1886 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
        }
#line 6122 "parser.cpp"
        break;

        case 258: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1890 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
            (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
        }
#line 6132 "parser.cpp"
        break;

        case 259: /* show_statement: SHOW TRANSACTIONS  */
#line 1895 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
        }
#line 6141 "parser.cpp"
        break;

        case 260: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1899 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
            (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
        }
#line 6151 "parser.cpp"
        break;

        case 261: /* show_statement: SHOW TRANSACTION HISTORY  */
#line 1904 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactionHistory;
        }
#line 6160 "parser.cpp"
        break;

        case 262: /* show_statement: SHOW SESSION VARIABLES  */
#line 1908 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
        }
#line 6169 "parser.cpp"
        break;

        case 263: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1912 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
        }
#line 6178 "parser.cpp"
        break;

        case 264: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1916 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
            (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 6189 "parser.cpp"
        break;

        case 265: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1922 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
            (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 6200 "parser.cpp"
        break;

        case 266: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1928 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
            (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 6211 "parser.cpp"
        break;

        case 267: /* show_statement: SHOW TABLE table_name  */
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
#line 6227 "parser.cpp"
        break;

        case 268: /* show_statement: SHOW TABLE table_name COLUMNS  */
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
#line 6243 "parser.cpp"
        break;

        case 269: /* show_statement: SHOW TABLE table_name SEGMENTS  */
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
#line 6259 "parser.cpp"
        break;

        case 270: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
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
#line 6276 "parser.cpp"
        break;

        case 271: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
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
#line 6293 "parser.cpp"
        break;

        case 272: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
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
#line 6311 "parser.cpp"
        break;

        case 273: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
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
#line 6330 "parser.cpp"
        break;

        case 274: /* show_statement: SHOW TABLE table_name INDEXES  */
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
#line 6346 "parser.cpp"
        break;

        case 275: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
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

            (yyval.show_stmt)->index_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 6365 "parser.cpp"
        break;

        case 276: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 2043 "parser.y"
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

            (yyval.show_stmt)->index_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));

            (yyval.show_stmt)->segment_id_ = (yyvsp[0].long_value);
        }
#line 6386 "parser.cpp"
        break;

        case 277: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 2059 "parser.y"
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

            (yyval.show_stmt)->index_name_ = (yyvsp[-4].str_value);
            free((yyvsp[-4].str_value));

            (yyval.show_stmt)->segment_id_ = (yyvsp[-2].long_value);
            (yyval.show_stmt)->chunk_id_ = (yyvsp[0].long_value);
        }
#line 6408 "parser.cpp"
        break;

        case 278: /* show_statement: SHOW LOGS  */
#line 2076 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
        }
#line 6417 "parser.cpp"
        break;

        case 279: /* show_statement: SHOW CATALOG  */
#line 2080 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalog;
        }
#line 6426 "parser.cpp"
        break;

        case 280: /* show_statement: SHOW CATALOG TO file_path  */
#line 2084 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogToFile;
            (yyval.show_stmt)->file_path_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 6437 "parser.cpp"
        break;

        case 281: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2090 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
        }
#line 6446 "parser.cpp"
        break;

        case 282: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2094 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
        }
#line 6455 "parser.cpp"
        break;

        case 283: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2098 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
            (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 6466 "parser.cpp"
        break;

        case 284: /* show_statement: SHOW MEMORY  */
#line 2104 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
        }
#line 6475 "parser.cpp"
        break;

        case 285: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2108 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
        }
#line 6484 "parser.cpp"
        break;

        case 286: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2112 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
        }
#line 6493 "parser.cpp"
        break;

        case 287: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2116 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
            (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));
        }
#line 6504 "parser.cpp"
        break;

        case 288: /* show_statement: SHOW SNAPSHOTS  */
#line 2122 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListSnapshots;
        }
#line 6513 "parser.cpp"
        break;

        case 289: /* show_statement: SHOW SNAPSHOT IDENTIFIER  */
#line 2126 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowSnapshot;
            (yyval.show_stmt)->snapshot_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 6524 "parser.cpp"
        break;

        case 290: /* flush_statement: FLUSH DATA  */
#line 2136 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
        }
#line 6533 "parser.cpp"
        break;

        case 291: /* flush_statement: FLUSH LOG  */
#line 2140 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
        }
#line 6542 "parser.cpp"
        break;

        case 292: /* flush_statement: FLUSH BUFFER  */
#line 2144 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
        }
#line 6551 "parser.cpp"
        break;

        case 293: /* flush_statement: FLUSH CATALOG  */
#line 2148 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kCatalog;
        }
#line 6560 "parser.cpp"
        break;

        case 294: /* optimize_statement: OPTIMIZE table_name  */
#line 2156 "parser.y"
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
#line 6575 "parser.cpp"
        break;

        case 295: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 2167 "parser.y"
        {
            (yyval.optimize_stmt) = new infinity::OptimizeStatement();
            if ((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
                (yyval.optimize_stmt)->schema_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
                free((yyvsp[-1].table_name_t)->schema_name_ptr_);
            }
            (yyval.optimize_stmt)->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
            free((yyvsp[-1].table_name_t)->table_name_ptr_);
            delete (yyvsp[-1].table_name_t);

            (yyval.optimize_stmt)->index_name_ = (yyvsp[-3].str_value);
            free((yyvsp[-3].str_value));

            for (auto *&index_param : *(yyvsp[0].with_index_param_list_t)) {
                (yyval.optimize_stmt)->opt_params_.emplace_back(std::unique_ptr<infinity::InitParameter>(index_param));
                index_param = nullptr;
            }
            delete (yyvsp[0].with_index_param_list_t);
        }
#line 6599 "parser.cpp"
        break;

        case 296: /* command_statement: USE IDENTIFIER  */
#line 2190 "parser.y"
        {
            (yyval.command_stmt) = new infinity::CommandStatement();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 6610 "parser.cpp"
        break;

        case 297: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2196 "parser.y"
        {
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
            free((yyvsp[0].str_value));
        }
#line 6620 "parser.cpp"
        break;

        case 298: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2201 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
            free((yyvsp[-1].str_value));
        }
#line 6631 "parser.cpp"
        break;

        case 299: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2207 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
            free((yyvsp[-1].str_value));
        }
#line 6642 "parser.cpp"
        break;

        case 300: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2213 "parser.y"
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
#line 6655 "parser.cpp"
        break;

        case 301: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2221 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession,
                                                                                     infinity::SetVarType::kInteger,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].long_value));
            free((yyvsp[-1].str_value));
        }
#line 6666 "parser.cpp"
        break;

        case 302: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2227 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession,
                                                                                     infinity::SetVarType::kDouble,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].double_value));
            free((yyvsp[-1].str_value));
        }
#line 6677 "parser.cpp"
        break;

        case 303: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2233 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
            free((yyvsp[-1].str_value));
        }
#line 6688 "parser.cpp"
        break;

        case 304: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2239 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
            free((yyvsp[-1].str_value));
        }
#line 6699 "parser.cpp"
        break;

        case 305: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2245 "parser.y"
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
#line 6712 "parser.cpp"
        break;

        case 306: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2253 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal,
                                                                                     infinity::SetVarType::kInteger,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].long_value));
            free((yyvsp[-1].str_value));
        }
#line 6723 "parser.cpp"
        break;

        case 307: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2259 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal,
                                                                                     infinity::SetVarType::kDouble,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].double_value));
            free((yyvsp[-1].str_value));
        }
#line 6734 "parser.cpp"
        break;

        case 308: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2265 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
            free((yyvsp[-1].str_value));
        }
#line 6745 "parser.cpp"
        break;

        case 309: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2271 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
            free((yyvsp[-1].str_value));
        }
#line 6756 "parser.cpp"
        break;

        case 310: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2277 "parser.y"
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
#line 6769 "parser.cpp"
        break;

        case 311: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2285 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig,
                                                                                     infinity::SetVarType::kInteger,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].long_value));
            free((yyvsp[-1].str_value));
        }
#line 6780 "parser.cpp"
        break;

        case 312: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2291 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig,
                                                                                     infinity::SetVarType::kDouble,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].double_value));
            free((yyvsp[-1].str_value));
        }
#line 6791 "parser.cpp"
        break;

        case 313: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON TABLE IDENTIFIER  */
#line 2297 "parser.y"
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
#line 6804 "parser.cpp"
        break;

        case 314: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON DATABASE IDENTIFIER  */
#line 2305 "parser.y"
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
#line 6817 "parser.cpp"
        break;

        case 315: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON SYSTEM  */
#line 2313 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SnapshotCmd>((yyvsp[-2].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kSystem);
            free((yyvsp[-2].str_value));
        }
#line 6828 "parser.cpp"
        break;

        case 316: /* command_statement: DROP SNAPSHOT IDENTIFIER  */
#line 2319 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kDrop, infinity::SnapshotScope::kIgnore);
            free((yyvsp[0].str_value));
        }
#line 6839 "parser.cpp"
        break;

        case 317: /* command_statement: RESTORE DATABASE SNAPSHOT IDENTIFIER  */
#line 2325 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kDatabase);
            free((yyvsp[0].str_value));
        }
#line 6850 "parser.cpp"
        break;

        case 318: /* command_statement: RESTORE TABLE SNAPSHOT IDENTIFIER  */
#line 2331 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kTable);
            free((yyvsp[0].str_value));
        }
#line 6861 "parser.cpp"
        break;

        case 319: /* command_statement: CLEAN DATA  */
#line 2337 "parser.y"
        {
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::CleanupCmd>();
        }
#line 6870 "parser.cpp"
        break;

        case 320: /* command_statement: DUMP INDEX IDENTIFIER ON table_name  */
#line 2341 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
            ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
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
#line 6888 "parser.cpp"
        break;

        case 321: /* compact_statement: COMPACT TABLE table_name  */
#line 2355 "parser.y"
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
#line 6905 "parser.cpp"
        break;

        case 322: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2368 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
            (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
            (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
        }
#line 6916 "parser.cpp"
        break;

        case 323: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2374 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
            (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
            (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
            (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
        }
#line 6928 "parser.cpp"
        break;

        case 324: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2381 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
            (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
            (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
            (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
            (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
        }
#line 6941 "parser.cpp"
        break;

        case 325: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2389 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
            (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
            (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
            (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
            (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
            (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
        }
#line 6955 "parser.cpp"
        break;

        case 326: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2398 "parser.y"
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
#line 6970 "parser.cpp"
        break;

        case 327: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2408 "parser.y"
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
#line 6985 "parser.cpp"
        break;

        case 328: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT
                     LONG_VALUE  */
#line 2418 "parser.y"
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
#line 7001 "parser.cpp"
        break;

        case 329: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT
                     LONG_VALUE BLOCKS  */
#line 2429 "parser.y"
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
#line 7017 "parser.cpp"
        break;

        case 330: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT
                     LONG_VALUE BLOCK LONG_VALUE  */
#line 2440 "parser.y"
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
#line 7034 "parser.cpp"
        break;

        case 331: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT
                     LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2452 "parser.y"
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
#line 7051 "parser.cpp"
        break;

        case 332: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2464 "parser.y"
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
#line 7066 "parser.cpp"
        break;

        case 333: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX
                     LONG_VALUE  */
#line 2474 "parser.y"
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
#line 7082 "parser.cpp"
        break;

        case 334: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX
                     LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2485 "parser.y"
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
#line 7099 "parser.cpp"
        break;

        case 335: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX
                     LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2497 "parser.y"
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
#line 7117 "parser.cpp"
        break;

        case 336: /* admin_statement: ADMIN SHOW LOGS  */
#line 2510 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
        }
#line 7126 "parser.cpp"
        break;

        case 337: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2514 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
            (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
        }
#line 7136 "parser.cpp"
        break;

        case 338: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2519 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
            (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
        }
#line 7146 "parser.cpp"
        break;

        case 339: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2524 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
            (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
            (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
        }
#line 7157 "parser.cpp"
        break;

        case 340: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2530 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
        }
#line 7166 "parser.cpp"
        break;

        case 341: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2534 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
        }
#line 7175 "parser.cpp"
        break;

        case 342: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2538 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
            (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7186 "parser.cpp"
        break;

        case 343: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2544 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
        }
#line 7195 "parser.cpp"
        break;

        case 344: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2548 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
        }
#line 7204 "parser.cpp"
        break;

        case 345: /* admin_statement: ADMIN SHOW SNAPSHOT IDENTIFIER  */
#line 2552 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSnapshot;
            (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7215 "parser.cpp"
        break;

        case 346: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2558 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
            (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7226 "parser.cpp"
        break;

        case 347: /* admin_statement: ADMIN EXPORT SNAPSHOT STRING TO STRING  */
#line 2564 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kExportSnapshot;
            (yyval.admin_stmt)->snapshot_name_ = (yyvsp[-2].str_value);
            (yyval.admin_stmt)->export_path_ = (yyvsp[0].str_value);
            free((yyvsp[-2].str_value));
            free((yyvsp[0].str_value));
        }
#line 7239 "parser.cpp"
        break;

        case 348: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2572 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
            (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7250 "parser.cpp"
        break;

        case 349: /* admin_statement: ADMIN SHOW NODES  */
#line 2578 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
        }
#line 7259 "parser.cpp"
        break;

        case 350: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2582 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
            (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7270 "parser.cpp"
        break;

        case 351: /* admin_statement: ADMIN SHOW NODE  */
#line 2588 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
        }
#line 7279 "parser.cpp"
        break;

        case 352: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2592 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
            (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7290 "parser.cpp"
        break;

        case 353: /* admin_statement: ADMIN SET ADMIN  */
#line 2598 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
            (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
        }
#line 7300 "parser.cpp"
        break;

        case 354: /* admin_statement: ADMIN SET STANDALONE  */
#line 2603 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
            (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
        }
#line 7310 "parser.cpp"
        break;

        case 355: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2608 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
            (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLeader;
            (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7322 "parser.cpp"
        break;

        case 356: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2615 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
            (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kFollower;
            (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
            (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
            free((yyvsp[-4].str_value));
            free((yyvsp[0].str_value));
        }
#line 7336 "parser.cpp"
        break;

        case 357: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2624 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
            (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLearner;
            (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
            (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
            free((yyvsp[-4].str_value));
            free((yyvsp[0].str_value));
        }
#line 7350 "parser.cpp"
        break;

        case 358: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2634 "parser.y"
        {
            auto *ret = new infinity::RenameTableStatement((yyvsp[-3].table_name_t)->schema_name_ptr_, (yyvsp[-3].table_name_t)->table_name_ptr_);
            (yyval.alter_stmt) = ret;
            ret->new_table_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
            free((yyvsp[-3].table_name_t)->schema_name_ptr_);
            free((yyvsp[-3].table_name_t)->table_name_ptr_);
            delete (yyvsp[-3].table_name_t);
        }
#line 7364 "parser.cpp"
        break;

        case 359: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2643 "parser.y"
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
#line 7381 "parser.cpp"
        break;

        case 360: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2655 "parser.y"
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
#line 7397 "parser.cpp"
        break;

        case 361: /* check_statement: CHECK SYSTEM  */
#line 2667 "parser.y"
        {
            (yyval.check_stmt) = new infinity::CheckStatement();
            (yyval.check_stmt)->check_type_ = infinity::CheckStmtType::kSystem;
        }
#line 7406 "parser.cpp"
        break;

        case 362: /* check_statement: CHECK TABLE table_name  */
#line 2671 "parser.y"
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
#line 7422 "parser.cpp"
        break;

        case 363: /* expr_array: expr_alias  */
#line 2687 "parser.y"
        {
            (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr *>();
            (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
        }
#line 7431 "parser.cpp"
        break;

        case 364: /* expr_array: expr_array ',' expr_alias  */
#line 2691 "parser.y"
        {
            (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
        }
#line 7440 "parser.cpp"
        break;

        case 365: /* insert_row_list: '(' expr_array ')'  */
#line 2696 "parser.y"
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
#line 7455 "parser.cpp"
        break;

        case 366: /* insert_row_list: insert_row_list ',' '(' expr_array ')'  */
#line 2706 "parser.y"
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
#line 7470 "parser.cpp"
        break;

        case 367: /* expr_alias: expr AS IDENTIFIER  */
#line 2728 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[-2].expr_t);
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7481 "parser.cpp"
        break;

        case 368: /* expr_alias: expr  */
#line 2734 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 7489 "parser.cpp"
        break;

        case 374: /* operand: '(' expr ')'  */
#line 2744 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[-1].expr_t);
        }
#line 7497 "parser.cpp"
        break;

        case 375: /* operand: '(' select_without_paren ')'  */
#line 2747 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 7508 "parser.cpp"
        break;

        case 376: /* operand: constant_expr  */
#line 2753 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].const_expr_t);
        }
#line 7516 "parser.cpp"
        break;

        case 387: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING
                     optional_search_filter_expr ')'  */
#line 2769 "parser.y"
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
#line 7536 "parser.cpp"
        break;

        case 388: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING
                     optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')'  */
#line 2785 "parser.y"
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
            match_tensor_expr->index_name_ = (yyvsp[-1].str_value);
            (yyval.expr_t) = match_tensor_expr.release();
        }
#line 7557 "parser.cpp"
        break;

        case 389: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING
                     optional_search_filter_expr ')' IGNORE INDEX  */
#line 2802 "parser.y"
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
#line 7578 "parser.cpp"
        break;

        case 390: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING
                     INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2820 "parser.y"
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
#line 7639 "parser.cpp"
        break;

        case 391: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE
                     INDEX  */
#line 2877 "parser.y"
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
#line 7693 "parser.cpp"
        break;

        case 392: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')'
                     with_index_param_list  */
#line 2927 "parser.y"
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
#line 7751 "parser.cpp"
        break;

        case 393: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2981 "parser.y"
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
#line 7810 "parser.cpp"
        break;

        case 394: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')'
                     USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 3039 "parser.y"
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

            match_sparse_expr->index_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));
        }
#line 7838 "parser.cpp"
        break;

        case 395: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')'
                     IGNORE INDEX  */
#line 3063 "parser.y"
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
#line 7865 "parser.cpp"
        break;

        case 396: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')'
                     with_index_param_list  */
#line 3086 "parser.y"
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
#line 7890 "parser.cpp"
        break;

        case 397: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')'
                     with_index_param_list  */
#line 3107 "parser.y"
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
#line 7915 "parser.cpp"
        break;

        case 398: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3128 "parser.y"
        {
            infinity::MatchExpr *match_text_expr = new infinity::MatchExpr();
            match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
            match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
            match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
            free((yyvsp[-4].str_value));
            free((yyvsp[-2].str_value));
            (yyval.expr_t) = match_text_expr;
        }
#line 7929 "parser.cpp"
        break;

        case 399: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3137 "parser.y"
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
#line 7945 "parser.cpp"
        break;

        case 400: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3148 "parser.y"
        {
            infinity::MatchExpr *match_text_expr = new infinity::MatchExpr();
            match_text_expr->fields_ = std::string((yyvsp[-9].str_value));
            match_text_expr->matching_text_ = std::string((yyvsp[-7].str_value));
            match_text_expr->filter_expr_.reset((yyvsp[-6].expr_t));
            match_text_expr->index_names_ = std::string((yyvsp[-1].str_value));
            free((yyvsp[-9].str_value));
            free((yyvsp[-7].str_value));
            free((yyvsp[-1].str_value));
            (yyval.expr_t) = match_text_expr;
        }
#line 7961 "parser.cpp"
        break;

        case 401: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3159 "parser.y"
        {
            infinity::MatchExpr *match_text_expr = new infinity::MatchExpr();
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
#line 7979 "parser.cpp"
        break;

        case 402: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 3173 "parser.y"
        {
            infinity::MatchExpr *match_text_expr = new infinity::MatchExpr();
            match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
            match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
            free((yyvsp[-2].str_value));
            (yyval.expr_t) = match_text_expr;
        }
#line 7991 "parser.cpp"
        break;

        case 403: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3180 "parser.y"
        {
            infinity::MatchExpr *match_text_expr = new infinity::MatchExpr();
            match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
            match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
            match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
            free((yyvsp[-4].str_value));
            free((yyvsp[-2].str_value));
            (yyval.expr_t) = match_text_expr;
        }
#line 8005 "parser.cpp"
        break;

        case 404: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3190 "parser.y"
        {
            infinity::FusionExpr *fusion_expr = new infinity::FusionExpr();
            fusion_expr->method_ = std::string((yyvsp[-1].str_value));
            free((yyvsp[-1].str_value));
            (yyval.expr_t) = fusion_expr;
        }
#line 8016 "parser.cpp"
        break;

        case 405: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 3196 "parser.y"
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
#line 8032 "parser.cpp"
        break;

        case 406: /* sub_search: match_vector_expr  */
#line 3208 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8040 "parser.cpp"
        break;

        case 407: /* sub_search: match_text_expr  */
#line 3211 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8048 "parser.cpp"
        break;

        case 408: /* sub_search: match_tensor_expr  */
#line 3214 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8056 "parser.cpp"
        break;

        case 409: /* sub_search: match_sparse_expr  */
#line 3217 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8064 "parser.cpp"
        break;

        case 410: /* sub_search: query_expr  */
#line 3220 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8072 "parser.cpp"
        break;

        case 411: /* sub_search: fusion_expr  */
#line 3223 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8080 "parser.cpp"
        break;

        case 412: /* sub_search_array: sub_search  */
#line 3227 "parser.y"
        {
            (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr *>();
            (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
        }
#line 8089 "parser.cpp"
        break;

        case 413: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3231 "parser.y"
        {
            (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
        }
#line 8098 "parser.cpp"
        break;

        case 414: /* function_expr: IDENTIFIER '(' ')'  */
#line 3236 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            ParserHelper::ToLower((yyvsp[-2].str_value));
            func_expr->func_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));
            func_expr->arguments_ = nullptr;
            (yyval.expr_t) = func_expr;
        }
#line 8111 "parser.cpp"
        break;

        case 415: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3244 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            ParserHelper::ToLower((yyvsp[-3].str_value));
            func_expr->func_name_ = (yyvsp[-3].str_value);
            free((yyvsp[-3].str_value));
            func_expr->arguments_ = (yyvsp[-1].expr_array_t);
            (yyval.expr_t) = func_expr;
        }
#line 8124 "parser.cpp"
        break;

        case 416: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 3252 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            ParserHelper::ToLower((yyvsp[-4].str_value));
            func_expr->func_name_ = (yyvsp[-4].str_value);
            free((yyvsp[-4].str_value));
            func_expr->arguments_ = (yyvsp[-1].expr_array_t);
            func_expr->distinct_ = true;
            (yyval.expr_t) = func_expr;
        }
#line 8138 "parser.cpp"
        break;

        case 417: /* function_expr: YEAR '(' expr ')'  */
#line 3261 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "year";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8150 "parser.cpp"
        break;

        case 418: /* function_expr: MONTH '(' expr ')'  */
#line 3268 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "month";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8162 "parser.cpp"
        break;

        case 419: /* function_expr: DAY '(' expr ')'  */
#line 3275 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "day";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8174 "parser.cpp"
        break;

        case 420: /* function_expr: HOUR '(' expr ')'  */
#line 3282 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "hour";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8186 "parser.cpp"
        break;

        case 421: /* function_expr: MINUTE '(' expr ')'  */
#line 3289 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "minute";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8198 "parser.cpp"
        break;

        case 422: /* function_expr: SECOND '(' expr ')'  */
#line 3296 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "second";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8210 "parser.cpp"
        break;

        case 423: /* function_expr: operand IS NOT NULLABLE  */
#line 3303 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "is_not_null";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8222 "parser.cpp"
        break;

        case 424: /* function_expr: operand IS NULLABLE  */
#line 3310 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "is_null";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8234 "parser.cpp"
        break;

        case 425: /* function_expr: NOT operand  */
#line 3317 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "not";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8246 "parser.cpp"
        break;

        case 426: /* function_expr: '-' operand  */
#line 3324 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "-";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8258 "parser.cpp"
        break;

        case 427: /* function_expr: '+' operand  */
#line 3331 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "+";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8270 "parser.cpp"
        break;

        case 428: /* function_expr: operand '-' operand  */
#line 3338 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "-";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8283 "parser.cpp"
        break;

        case 429: /* function_expr: operand '+' operand  */
#line 3346 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "+";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8296 "parser.cpp"
        break;

        case 430: /* function_expr: operand '*' operand  */
#line 3354 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "*";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8309 "parser.cpp"
        break;

        case 431: /* function_expr: operand '/' operand  */
#line 3362 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "/";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8322 "parser.cpp"
        break;

        case 432: /* function_expr: operand '%' operand  */
#line 3370 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "%";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8335 "parser.cpp"
        break;

        case 433: /* function_expr: operand '=' operand  */
#line 3378 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8348 "parser.cpp"
        break;

        case 434: /* function_expr: operand EQUAL operand  */
#line 3386 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8361 "parser.cpp"
        break;

        case 435: /* function_expr: operand NOT_EQ operand  */
#line 3394 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "<>";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8374 "parser.cpp"
        break;

        case 436: /* function_expr: operand '<' operand  */
#line 3402 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "<";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8387 "parser.cpp"
        break;

        case 437: /* function_expr: operand '>' operand  */
#line 3410 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = ">";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8400 "parser.cpp"
        break;

        case 438: /* function_expr: operand LESS_EQ operand  */
#line 3418 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "<=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8413 "parser.cpp"
        break;

        case 439: /* function_expr: operand GREATER_EQ operand  */
#line 3426 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = ">=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8426 "parser.cpp"
        break;

        case 440: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3434 "parser.y"
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
#line 8461 "parser.cpp"
        break;

        case 441: /* function_expr: operand LIKE operand  */
#line 3464 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "like";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8474 "parser.cpp"
        break;

        case 442: /* function_expr: operand NOT LIKE operand  */
#line 3472 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "not_like";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8487 "parser.cpp"
        break;

        case 443: /* conjunction_expr: expr AND expr  */
#line 3481 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "and";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8500 "parser.cpp"
        break;

        case 444: /* conjunction_expr: expr OR expr  */
#line 3489 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "or";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8513 "parser.cpp"
        break;

        case 445: /* between_expr: operand BETWEEN operand AND operand  */
#line 3498 "parser.y"
        {
            infinity::BetweenExpr *between_expr = new infinity::BetweenExpr();
            between_expr->value_ = (yyvsp[-4].expr_t);
            between_expr->lower_bound_ = (yyvsp[-2].expr_t);
            between_expr->upper_bound_ = (yyvsp[0].expr_t);
            (yyval.expr_t) = between_expr;
        }
#line 8525 "parser.cpp"
        break;

        case 446: /* in_expr: operand IN '(' expr_array ')'  */
#line 3506 "parser.y"
        {
            infinity::InExpr *in_expr = new infinity::InExpr(true);
            in_expr->left_ = (yyvsp[-4].expr_t);
            in_expr->arguments_ = (yyvsp[-1].expr_array_t);
            (yyval.expr_t) = in_expr;
        }
#line 8536 "parser.cpp"
        break;

        case 447: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3512 "parser.y"
        {
            infinity::InExpr *in_expr = new infinity::InExpr(false);
            in_expr->left_ = (yyvsp[-5].expr_t);
            in_expr->arguments_ = (yyvsp[-1].expr_array_t);
            (yyval.expr_t) = in_expr;
        }
#line 8547 "parser.cpp"
        break;

        case 448: /* case_expr: CASE expr case_check_array END  */
#line 3519 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->expr_ = (yyvsp[-2].expr_t);
            case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
            (yyval.expr_t) = case_expr;
        }
#line 8558 "parser.cpp"
        break;

        case 449: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3525 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->expr_ = (yyvsp[-4].expr_t);
            case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
            case_expr->else_expr_ = (yyvsp[-1].expr_t);
            (yyval.expr_t) = case_expr;
        }
#line 8570 "parser.cpp"
        break;

        case 450: /* case_expr: CASE case_check_array END  */
#line 3532 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
            (yyval.expr_t) = case_expr;
        }
#line 8580 "parser.cpp"
        break;

        case 451: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3537 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
            case_expr->else_expr_ = (yyvsp[-1].expr_t);
            (yyval.expr_t) = case_expr;
        }
#line 8591 "parser.cpp"
        break;

        case 452: /* case_check_array: WHEN expr THEN expr  */
#line 3544 "parser.y"
        {
            (yyval.case_check_array_t) = new std::vector<infinity::WhenThen *>();
            infinity::WhenThen *when_then_ptr = new infinity::WhenThen();
            when_then_ptr->when_ = (yyvsp[-2].expr_t);
            when_then_ptr->then_ = (yyvsp[0].expr_t);
            (yyval.case_check_array_t)->emplace_back(when_then_ptr);
        }
#line 8603 "parser.cpp"
        break;

        case 453: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3551 "parser.y"
        {
            infinity::WhenThen *when_then_ptr = new infinity::WhenThen();
            when_then_ptr->when_ = (yyvsp[-2].expr_t);
            when_then_ptr->then_ = (yyvsp[0].expr_t);
            (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
            (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
        }
#line 8615 "parser.cpp"
        break;

        case 454: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3559 "parser.y"
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
#line 8632 "parser.cpp"
        break;

        case 455: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3572 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8643 "parser.cpp"
        break;

        case 456: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3578 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8654 "parser.cpp"
        break;

        case 457: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3584 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
            subquery_expr->left_ = (yyvsp[-4].expr_t);
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8666 "parser.cpp"
        break;

        case 458: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3591 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
            subquery_expr->left_ = (yyvsp[-5].expr_t);
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8678 "parser.cpp"
        break;

        case 459: /* column_expr: IDENTIFIER  */
#line 3599 "parser.y"
        {
            infinity::ColumnExpr *column_expr = new infinity::ColumnExpr();
            ParserHelper::ToLower((yyvsp[0].str_value));
            column_expr->names_.emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
            (yyval.expr_t) = column_expr;
        }
#line 8690 "parser.cpp"
        break;

        case 460: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3606 "parser.y"
        {
            infinity::ColumnExpr *column_expr = (infinity::ColumnExpr *)(yyvsp[-2].expr_t);
            ParserHelper::ToLower((yyvsp[0].str_value));
            column_expr->names_.emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
            (yyval.expr_t) = column_expr;
        }
#line 8702 "parser.cpp"
        break;

        case 461: /* column_expr: '*'  */
#line 3613 "parser.y"
        {
            infinity::ColumnExpr *column_expr = new infinity::ColumnExpr();
            column_expr->star_ = true;
            (yyval.expr_t) = column_expr;
        }
#line 8712 "parser.cpp"
        break;

        case 462: /* column_expr: column_expr '.' '*'  */
#line 3618 "parser.y"
        {
            infinity::ColumnExpr *column_expr = (infinity::ColumnExpr *)(yyvsp[-2].expr_t);
            if (column_expr->star_) {
                yyerror(&yyloc, scanner, result, "Invalid column expression format");
                YYERROR;
            }
            column_expr->star_ = true;
            (yyval.expr_t) = column_expr;
        }
#line 8726 "parser.cpp"
        break;

        case 463: /* constant_expr: STRING  */
#line 3628 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
            const_expr->str_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8736 "parser.cpp"
        break;

        case 464: /* constant_expr: TRUE  */
#line 3633 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
            const_expr->bool_value_ = true;
            (yyval.const_expr_t) = const_expr;
        }
#line 8746 "parser.cpp"
        break;

        case 465: /* constant_expr: FALSE  */
#line 3638 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
            const_expr->bool_value_ = false;
            (yyval.const_expr_t) = const_expr;
        }
#line 8756 "parser.cpp"
        break;

        case 466: /* constant_expr: DOUBLE_VALUE  */
#line 3643 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
            const_expr->double_value_ = (yyvsp[0].double_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8766 "parser.cpp"
        break;

        case 467: /* constant_expr: LONG_VALUE  */
#line 3648 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
            const_expr->integer_value_ = (yyvsp[0].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8776 "parser.cpp"
        break;

        case 468: /* constant_expr: DATE STRING  */
#line 3653 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8786 "parser.cpp"
        break;

        case 469: /* constant_expr: TIME STRING  */
#line 3658 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8796 "parser.cpp"
        break;

        case 470: /* constant_expr: DATETIME STRING  */
#line 3663 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8806 "parser.cpp"
        break;

        case 471: /* constant_expr: TIMESTAMP STRING  */
#line 3668 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8816 "parser.cpp"
        break;

        case 472: /* constant_expr: INTERVAL interval_expr  */
#line 3673 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 8824 "parser.cpp"
        break;

        case 473: /* constant_expr: interval_expr  */
#line 3676 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 8832 "parser.cpp"
        break;

        case 474: /* constant_expr: common_array_expr  */
#line 3679 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 8840 "parser.cpp"
        break;

        case 475: /* constant_expr: curly_brackets_expr  */
#line 3682 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 8848 "parser.cpp"
        break;

        case 476: /* common_array_expr: array_expr  */
#line 3686 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 8856 "parser.cpp"
        break;

        case 477: /* common_array_expr: subarray_array_expr  */
#line 3689 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 8864 "parser.cpp"
        break;

        case 478: /* common_array_expr: sparse_array_expr  */
#line 3692 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 8872 "parser.cpp"
        break;

        case 479: /* common_array_expr: empty_array_expr  */
#line 3695 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 8880 "parser.cpp"
        break;

        case 480: /* common_sparse_array_expr: sparse_array_expr  */
#line 3699 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 8888 "parser.cpp"
        break;

        case 481: /* common_sparse_array_expr: array_expr  */
#line 3702 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 8896 "parser.cpp"
        break;

        case 482: /* common_sparse_array_expr: empty_array_expr  */
#line 3705 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 8904 "parser.cpp"
        break;

        case 483: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3709 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 8912 "parser.cpp"
        break;

        case 484: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3713 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
            const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
            (yyval.const_expr_t) = const_expr;
        }
#line 8922 "parser.cpp"
        break;

        case 485: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3718 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 8931 "parser.cpp"
        break;

        case 486: /* sparse_array_expr: long_sparse_array_expr  */
#line 3723 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 8939 "parser.cpp"
        break;

        case 487: /* sparse_array_expr: double_sparse_array_expr  */
#line 3726 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 8947 "parser.cpp"
        break;

        case 488: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3730 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 8955 "parser.cpp"
        break;

        case 489: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3734 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
            const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
            const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
            delete (yyvsp[0].int_sparse_ele_t);
            (yyval.const_expr_t) = const_expr;
        }
#line 8967 "parser.cpp"
        break;

        case 490: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3741 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
            (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
            delete (yyvsp[0].int_sparse_ele_t);
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 8978 "parser.cpp"
        break;

        case 491: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3748 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 8986 "parser.cpp"
        break;

        case 492: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3752 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
            const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
            const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
            delete (yyvsp[0].float_sparse_ele_t);
            (yyval.const_expr_t) = const_expr;
        }
#line 8998 "parser.cpp"
        break;

        case 493: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3759 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
            (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
            delete (yyvsp[0].float_sparse_ele_t);
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9009 "parser.cpp"
        break;

        case 494: /* empty_array_expr: '[' ']'  */
#line 3766 "parser.y"
        {
            (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
        }
#line 9017 "parser.cpp"
        break;

        case 495: /* curly_brackets_expr: unclosed_curly_brackets_expr '}'  */
#line 3770 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9025 "parser.cpp"
        break;

        case 496: /* curly_brackets_expr: '{' '}'  */
#line 3773 "parser.y"
        {
            (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
        }
#line 9033 "parser.cpp"
        break;

        case 497: /* unclosed_curly_brackets_expr: '{' constant_expr  */
#line 3777 "parser.y"
        {
            (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
            (yyval.const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
        }
#line 9042 "parser.cpp"
        break;

        case 498: /* unclosed_curly_brackets_expr: unclosed_curly_brackets_expr ',' constant_expr  */
#line 3781 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9051 "parser.cpp"
        break;

        case 499: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3786 "parser.y"
        {
            (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
        }
#line 9059 "parser.cpp"
        break;

        case 500: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3790 "parser.y"
        {
            (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
        }
#line 9067 "parser.cpp"
        break;

        case 501: /* array_expr: long_array_expr  */
#line 3794 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9075 "parser.cpp"
        break;

        case 502: /* array_expr: double_array_expr  */
#line 3797 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9083 "parser.cpp"
        break;

        case 503: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3801 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9091 "parser.cpp"
        break;

        case 504: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3805 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
            const_expr->long_array_.emplace_back((yyvsp[0].long_value));
            (yyval.const_expr_t) = const_expr;
        }
#line 9101 "parser.cpp"
        break;

        case 505: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3810 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9110 "parser.cpp"
        break;

        case 506: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3815 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9118 "parser.cpp"
        break;

        case 507: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3819 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
            const_expr->double_array_.emplace_back((yyvsp[0].double_value));
            (yyval.const_expr_t) = const_expr;
        }
#line 9128 "parser.cpp"
        break;

        case 508: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3824 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9137 "parser.cpp"
        break;

        case 509: /* interval_expr: LONG_VALUE SECONDS  */
#line 3829 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kSecond;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9148 "parser.cpp"
        break;

        case 510: /* interval_expr: LONG_VALUE SECOND  */
#line 3835 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kSecond;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9159 "parser.cpp"
        break;

        case 511: /* interval_expr: LONG_VALUE MINUTES  */
#line 3841 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMinute;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9170 "parser.cpp"
        break;

        case 512: /* interval_expr: LONG_VALUE MINUTE  */
#line 3847 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMinute;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9181 "parser.cpp"
        break;

        case 513: /* interval_expr: LONG_VALUE HOURS  */
#line 3853 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kHour;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9192 "parser.cpp"
        break;

        case 514: /* interval_expr: LONG_VALUE HOUR  */
#line 3859 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kHour;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9203 "parser.cpp"
        break;

        case 515: /* interval_expr: LONG_VALUE DAYS  */
#line 3865 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kDay;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9214 "parser.cpp"
        break;

        case 516: /* interval_expr: LONG_VALUE DAY  */
#line 3871 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kDay;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9225 "parser.cpp"
        break;

        case 517: /* interval_expr: LONG_VALUE MONTHS  */
#line 3877 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMonth;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9236 "parser.cpp"
        break;

        case 518: /* interval_expr: LONG_VALUE MONTH  */
#line 3883 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMonth;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9247 "parser.cpp"
        break;

        case 519: /* interval_expr: LONG_VALUE YEARS  */
#line 3889 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kYear;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9258 "parser.cpp"
        break;

        case 520: /* interval_expr: LONG_VALUE YEAR  */
#line 3895 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kYear;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9269 "parser.cpp"
        break;

        case 521: /* copy_option_list: copy_option  */
#line 3906 "parser.y"
        {
            (yyval.copy_option_array) = new std::vector<infinity::CopyOption *>();
            (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
        }
#line 9278 "parser.cpp"
        break;

        case 522: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3910 "parser.y"
        {
            (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
            (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
        }
#line 9287 "parser.cpp"
        break;

        case 523: /* copy_option: FORMAT IDENTIFIER  */
#line 3915 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kFormat;
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
#line 9323 "parser.cpp"
        break;

        case 524: /* copy_option: DELIMITER STRING  */
#line 3946 "parser.y"
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
#line 9338 "parser.cpp"
        break;

        case 525: /* copy_option: HEADER  */
#line 3956 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
            (yyval.copy_option_t)->header_ = true;
        }
#line 9348 "parser.cpp"
        break;

        case 526: /* copy_option: OFFSET LONG_VALUE  */
#line 3961 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
            (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
        }
#line 9358 "parser.cpp"
        break;

        case 527: /* copy_option: LIMIT LONG_VALUE  */
#line 3966 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
            (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
        }
#line 9368 "parser.cpp"
        break;

        case 528: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3971 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
            (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
        }
#line 9378 "parser.cpp"
        break;

        case 529: /* file_path: STRING  */
#line 3977 "parser.y"
        {
            (yyval.str_value) = (yyvsp[0].str_value);
        }
#line 9386 "parser.cpp"
        break;

        case 530: /* if_exists: IF EXISTS  */
#line 3981 "parser.y"
        {
            (yyval.bool_value) = true;
        }
#line 9392 "parser.cpp"
        break;

        case 531: /* if_exists: %empty  */
#line 3982 "parser.y"
        {
            (yyval.bool_value) = false;
        }
#line 9398 "parser.cpp"
        break;

        case 532: /* if_not_exists: IF NOT EXISTS  */
#line 3984 "parser.y"
        {
            (yyval.bool_value) = true;
        }
#line 9404 "parser.cpp"
        break;

        case 533: /* if_not_exists: %empty  */
#line 3985 "parser.y"
        {
            (yyval.bool_value) = false;
        }
#line 9410 "parser.cpp"
        break;

        case 536: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 4000 "parser.y"
        {
            (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
            (yyval.if_not_exists_info_t)->exists_ = true;
            (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 9423 "parser.cpp"
        break;

        case 537: /* if_not_exists_info: %empty  */
#line 4008 "parser.y"
        {
            (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
        }
#line 9431 "parser.cpp"
        break;

        case 538: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 4012 "parser.y"
        {
            (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
        }
#line 9439 "parser.cpp"
        break;

        case 539: /* with_index_param_list: %empty  */
#line 4015 "parser.y"
        {
            (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter *>();
        }
#line 9447 "parser.cpp"
        break;

        case 540: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 4019 "parser.y"
        {
            (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
        }
#line 9455 "parser.cpp"
        break;

        case 541: /* optional_table_properties_list: %empty  */
#line 4022 "parser.y"
        {
            (yyval.with_index_param_list_t) = nullptr;
        }
#line 9463 "parser.cpp"
        break;

        case 542: /* index_param_list: index_param  */
#line 4026 "parser.y"
        {
            (yyval.index_param_list_t) = new std::vector<infinity::InitParameter *>();
            (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
        }
#line 9472 "parser.cpp"
        break;

        case 543: /* index_param_list: index_param_list ',' index_param  */
#line 4030 "parser.y"
        {
            (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
            (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
        }
#line 9481 "parser.cpp"
        break;

        case 544: /* index_param: IDENTIFIER  */
#line 4035 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 9492 "parser.cpp"
        break;

        case 545: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 4041 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));

            (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 9507 "parser.cpp"
        break;

        case 546: /* index_param: IDENTIFIER '=' STRING  */
#line 4051 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));

            (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 9522 "parser.cpp"
        break;

        case 547: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 4061 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));

            (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
        }
#line 9535 "parser.cpp"
        break;

        case 548: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 4069 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));

            (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
        }
#line 9548 "parser.cpp"
        break;

        case 549: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 4080 "parser.y"
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
            (yyval.index_info_t)->column_name_ = (yyvsp[-4].str_value);
            (yyval.index_info_t)->index_param_list_ = (yyvsp[0].with_index_param_list_t);
            free((yyvsp[-4].str_value));
        }
#line 9584 "parser.cpp"
        break;

        case 550: /* index_info: '(' IDENTIFIER ')'  */
#line 4111 "parser.y"
        {
            (yyval.index_info_t) = new infinity::IndexInfo();
            (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
            (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
            free((yyvsp[-1].str_value));
        }
#line 9595 "parser.cpp"
        break;

#line 9599 "parser.cpp"

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

#line 4118 "parser.y"

void yyerror(YYLTYPE *llocp, void *lexer, infinity::ParserResult *result, const char *msg) {
    if (result->IsError())
        return;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
    fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}
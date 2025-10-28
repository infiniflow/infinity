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
    YYSYMBOL_INTEGER = 90,                               /* INTEGER  */
    YYSYMBOL_INT = 91,                                   /* INT  */
    YYSYMBOL_TINYINT = 92,                               /* TINYINT  */
    YYSYMBOL_SMALLINT = 93,                              /* SMALLINT  */
    YYSYMBOL_BIGINT = 94,                                /* BIGINT  */
    YYSYMBOL_HUGEINT = 95,                               /* HUGEINT  */
    YYSYMBOL_VARCHAR = 96,                               /* VARCHAR  */
    YYSYMBOL_FLOAT = 97,                                 /* FLOAT  */
    YYSYMBOL_DOUBLE = 98,                                /* DOUBLE  */
    YYSYMBOL_REAL = 99,                                  /* REAL  */
    YYSYMBOL_DECIMAL = 100,                              /* DECIMAL  */
    YYSYMBOL_DATE = 101,                                 /* DATE  */
    YYSYMBOL_TIME = 102,                                 /* TIME  */
    YYSYMBOL_DATETIME = 103,                             /* DATETIME  */
    YYSYMBOL_FLOAT16 = 104,                              /* FLOAT16  */
    YYSYMBOL_BFLOAT16 = 105,                             /* BFLOAT16  */
    YYSYMBOL_UNSIGNED = 106,                             /* UNSIGNED  */
    YYSYMBOL_TIMESTAMP = 107,                            /* TIMESTAMP  */
    YYSYMBOL_UUID = 108,                                 /* UUID  */
    YYSYMBOL_POINT = 109,                                /* POINT  */
    YYSYMBOL_LINE = 110,                                 /* LINE  */
    YYSYMBOL_LSEG = 111,                                 /* LSEG  */
    YYSYMBOL_BOX = 112,                                  /* BOX  */
    YYSYMBOL_PATH = 113,                                 /* PATH  */
    YYSYMBOL_POLYGON = 114,                              /* POLYGON  */
    YYSYMBOL_CIRCLE = 115,                               /* CIRCLE  */
    YYSYMBOL_BLOB = 116,                                 /* BLOB  */
    YYSYMBOL_BITMAP = 117,                               /* BITMAP  */
    YYSYMBOL_ARRAY = 118,                                /* ARRAY  */
    YYSYMBOL_TUPLE = 119,                                /* TUPLE  */
    YYSYMBOL_EMBEDDING = 120,                            /* EMBEDDING  */
    YYSYMBOL_VECTOR = 121,                               /* VECTOR  */
    YYSYMBOL_MULTIVECTOR = 122,                          /* MULTIVECTOR  */
    YYSYMBOL_TENSOR = 123,                               /* TENSOR  */
    YYSYMBOL_SPARSE = 124,                               /* SPARSE  */
    YYSYMBOL_TENSORARRAY = 125,                          /* TENSORARRAY  */
    YYSYMBOL_BIT = 126,                                  /* BIT  */
    YYSYMBOL_TEXT = 127,                                 /* TEXT  */
    YYSYMBOL_PRIMARY = 128,                              /* PRIMARY  */
    YYSYMBOL_KEY = 129,                                  /* KEY  */
    YYSYMBOL_UNIQUE = 130,                               /* UNIQUE  */
    YYSYMBOL_NULLABLE = 131,                             /* NULLABLE  */
    YYSYMBOL_IS = 132,                                   /* IS  */
    YYSYMBOL_DEFAULT = 133,                              /* DEFAULT  */
    YYSYMBOL_COMMENT = 134,                              /* COMMENT  */
    YYSYMBOL_IGNORE = 135,                               /* IGNORE  */
    YYSYMBOL_TRUE = 136,                                 /* TRUE  */
    YYSYMBOL_FALSE = 137,                                /* FALSE  */
    YYSYMBOL_INTERVAL = 138,                             /* INTERVAL  */
    YYSYMBOL_SECOND = 139,                               /* SECOND  */
    YYSYMBOL_SECONDS = 140,                              /* SECONDS  */
    YYSYMBOL_MINUTE = 141,                               /* MINUTE  */
    YYSYMBOL_MINUTES = 142,                              /* MINUTES  */
    YYSYMBOL_HOUR = 143,                                 /* HOUR  */
    YYSYMBOL_HOURS = 144,                                /* HOURS  */
    YYSYMBOL_DAY = 145,                                  /* DAY  */
    YYSYMBOL_DAYS = 146,                                 /* DAYS  */
    YYSYMBOL_MONTH = 147,                                /* MONTH  */
    YYSYMBOL_MONTHS = 148,                               /* MONTHS  */
    YYSYMBOL_YEAR = 149,                                 /* YEAR  */
    YYSYMBOL_YEARS = 150,                                /* YEARS  */
    YYSYMBOL_EQUAL = 151,                                /* EQUAL  */
    YYSYMBOL_NOT_EQ = 152,                               /* NOT_EQ  */
    YYSYMBOL_LESS_EQ = 153,                              /* LESS_EQ  */
    YYSYMBOL_GREATER_EQ = 154,                           /* GREATER_EQ  */
    YYSYMBOL_BETWEEN = 155,                              /* BETWEEN  */
    YYSYMBOL_AND = 156,                                  /* AND  */
    YYSYMBOL_OR = 157,                                   /* OR  */
    YYSYMBOL_EXTRACT = 158,                              /* EXTRACT  */
    YYSYMBOL_LIKE = 159,                                 /* LIKE  */
    YYSYMBOL_DATA = 160,                                 /* DATA  */
    YYSYMBOL_LOG = 161,                                  /* LOG  */
    YYSYMBOL_BUFFER = 162,                               /* BUFFER  */
    YYSYMBOL_TRANSACTIONS = 163,                         /* TRANSACTIONS  */
    YYSYMBOL_TRANSACTION = 164,                          /* TRANSACTION  */
    YYSYMBOL_MEMINDEX = 165,                             /* MEMINDEX  */
    YYSYMBOL_USING = 166,                                /* USING  */
    YYSYMBOL_SESSION = 167,                              /* SESSION  */
    YYSYMBOL_GLOBAL = 168,                               /* GLOBAL  */
    YYSYMBOL_OFF = 169,                                  /* OFF  */
    YYSYMBOL_EXPORT = 170,                               /* EXPORT  */
    YYSYMBOL_CONFIGS = 171,                              /* CONFIGS  */
    YYSYMBOL_CONFIG = 172,                               /* CONFIG  */
    YYSYMBOL_PROFILES = 173,                             /* PROFILES  */
    YYSYMBOL_VARIABLES = 174,                            /* VARIABLES  */
    YYSYMBOL_VARIABLE = 175,                             /* VARIABLE  */
    YYSYMBOL_LOGS = 176,                                 /* LOGS  */
    YYSYMBOL_CATALOGS = 177,                             /* CATALOGS  */
    YYSYMBOL_CATALOG = 178,                              /* CATALOG  */
    YYSYMBOL_SEARCH = 179,                               /* SEARCH  */
    YYSYMBOL_MATCH = 180,                                /* MATCH  */
    YYSYMBOL_MAXSIM = 181,                               /* MAXSIM  */
    YYSYMBOL_QUERY = 182,                                /* QUERY  */
    YYSYMBOL_QUERIES = 183,                              /* QUERIES  */
    YYSYMBOL_FUSION = 184,                               /* FUSION  */
    YYSYMBOL_ROWLIMIT = 185,                             /* ROWLIMIT  */
    YYSYMBOL_ADMIN = 186,                                /* ADMIN  */
    YYSYMBOL_LEADER = 187,                               /* LEADER  */
    YYSYMBOL_FOLLOWER = 188,                             /* FOLLOWER  */
    YYSYMBOL_LEARNER = 189,                              /* LEARNER  */
    YYSYMBOL_CONNECT = 190,                              /* CONNECT  */
    YYSYMBOL_STANDALONE = 191,                           /* STANDALONE  */
    YYSYMBOL_NODES = 192,                                /* NODES  */
    YYSYMBOL_NODE = 193,                                 /* NODE  */
    YYSYMBOL_REMOVE = 194,                               /* REMOVE  */
    YYSYMBOL_SNAPSHOT = 195,                             /* SNAPSHOT  */
    YYSYMBOL_SNAPSHOTS = 196,                            /* SNAPSHOTS  */
    YYSYMBOL_RECOVER = 197,                              /* RECOVER  */
    YYSYMBOL_RESTORE = 198,                              /* RESTORE  */
    YYSYMBOL_CACHES = 199,                               /* CACHES  */
    YYSYMBOL_CACHE = 200,                                /* CACHE  */
    YYSYMBOL_PERSISTENCE = 201,                          /* PERSISTENCE  */
    YYSYMBOL_OBJECT = 202,                               /* OBJECT  */
    YYSYMBOL_OBJECTS = 203,                              /* OBJECTS  */
    YYSYMBOL_FILES = 204,                                /* FILES  */
    YYSYMBOL_MEMORY = 205,                               /* MEMORY  */
    YYSYMBOL_ALLOCATION = 206,                           /* ALLOCATION  */
    YYSYMBOL_HISTORY = 207,                              /* HISTORY  */
    YYSYMBOL_CHECK = 208,                                /* CHECK  */
    YYSYMBOL_CLEAN = 209,                                /* CLEAN  */
    YYSYMBOL_CHECKPOINT = 210,                           /* CHECKPOINT  */
    YYSYMBOL_IMPORT = 211,                               /* IMPORT  */
    YYSYMBOL_NUMBER = 212,                               /* NUMBER  */
    YYSYMBOL_213_ = 213,                                 /* '='  */
    YYSYMBOL_214_ = 214,                                 /* '<'  */
    YYSYMBOL_215_ = 215,                                 /* '>'  */
    YYSYMBOL_216_ = 216,                                 /* '+'  */
    YYSYMBOL_217_ = 217,                                 /* '-'  */
    YYSYMBOL_218_ = 218,                                 /* '*'  */
    YYSYMBOL_219_ = 219,                                 /* '/'  */
    YYSYMBOL_220_ = 220,                                 /* '%'  */
    YYSYMBOL_221_ = 221,                                 /* '['  */
    YYSYMBOL_222_ = 222,                                 /* ']'  */
    YYSYMBOL_223_ = 223,                                 /* '('  */
    YYSYMBOL_224_ = 224,                                 /* ')'  */
    YYSYMBOL_225_ = 225,                                 /* '.'  */
    YYSYMBOL_226_ = 226,                                 /* ';'  */
    YYSYMBOL_227_ = 227,                                 /* ','  */
    YYSYMBOL_228_ = 228,                                 /* '}'  */
    YYSYMBOL_229_ = 229,                                 /* '{'  */
    YYSYMBOL_230_ = 230,                                 /* ':'  */
    YYSYMBOL_YYACCEPT = 231,                             /* $accept  */
    YYSYMBOL_input_pattern = 232,                        /* input_pattern  */
    YYSYMBOL_statement_list = 233,                       /* statement_list  */
    YYSYMBOL_statement = 234,                            /* statement  */
    YYSYMBOL_explainable_statement = 235,                /* explainable_statement  */
    YYSYMBOL_create_statement = 236,                     /* create_statement  */
    YYSYMBOL_table_element_array = 237,                  /* table_element_array  */
    YYSYMBOL_column_def_array = 238,                     /* column_def_array  */
    YYSYMBOL_table_element = 239,                        /* table_element  */
    YYSYMBOL_table_column = 240,                         /* table_column  */
    YYSYMBOL_column_type_array = 241,                    /* column_type_array  */
    YYSYMBOL_column_type = 242,                          /* column_type  */
    YYSYMBOL_column_constraints = 243,                   /* column_constraints  */
    YYSYMBOL_column_constraint = 244,                    /* column_constraint  */
    YYSYMBOL_default_expr = 245,                         /* default_expr  */
    YYSYMBOL_table_constraint = 246,                     /* table_constraint  */
    YYSYMBOL_identifier_array = 247,                     /* identifier_array  */
    YYSYMBOL_delete_statement = 248,                     /* delete_statement  */
    YYSYMBOL_insert_statement = 249,                     /* insert_statement  */
    YYSYMBOL_optional_identifier_array = 250,            /* optional_identifier_array  */
    YYSYMBOL_explain_statement = 251,                    /* explain_statement  */
    YYSYMBOL_update_statement = 252,                     /* update_statement  */
    YYSYMBOL_update_expr_array = 253,                    /* update_expr_array  */
    YYSYMBOL_update_expr = 254,                          /* update_expr  */
    YYSYMBOL_drop_statement = 255,                       /* drop_statement  */
    YYSYMBOL_copy_statement = 256,                       /* copy_statement  */
    YYSYMBOL_select_statement = 257,                     /* select_statement  */
    YYSYMBOL_select_with_paren = 258,                    /* select_with_paren  */
    YYSYMBOL_select_without_paren = 259,                 /* select_without_paren  */
    YYSYMBOL_select_clause_with_modifier = 260,          /* select_clause_with_modifier  */
    YYSYMBOL_select_clause_without_modifier_paren = 261, /* select_clause_without_modifier_paren  */
    YYSYMBOL_select_clause_without_modifier = 262,       /* select_clause_without_modifier  */
    YYSYMBOL_order_by_clause = 263,                      /* order_by_clause  */
    YYSYMBOL_order_by_expr_list = 264,                   /* order_by_expr_list  */
    YYSYMBOL_order_by_expr = 265,                        /* order_by_expr  */
    YYSYMBOL_order_by_type = 266,                        /* order_by_type  */
    YYSYMBOL_limit_expr = 267,                           /* limit_expr  */
    YYSYMBOL_offset_expr = 268,                          /* offset_expr  */
    YYSYMBOL_distinct = 269,                             /* distinct  */
    YYSYMBOL_highlight_clause = 270,                     /* highlight_clause  */
    YYSYMBOL_from_clause = 271,                          /* from_clause  */
    YYSYMBOL_search_clause = 272,                        /* search_clause  */
    YYSYMBOL_optional_search_filter_expr = 273,          /* optional_search_filter_expr  */
    YYSYMBOL_where_clause = 274,                         /* where_clause  */
    YYSYMBOL_having_clause = 275,                        /* having_clause  */
    YYSYMBOL_group_by_clause = 276,                      /* group_by_clause  */
    YYSYMBOL_set_operator = 277,                         /* set_operator  */
    YYSYMBOL_table_reference = 278,                      /* table_reference  */
    YYSYMBOL_table_reference_unit = 279,                 /* table_reference_unit  */
    YYSYMBOL_table_reference_name = 280,                 /* table_reference_name  */
    YYSYMBOL_table_name = 281,                           /* table_name  */
    YYSYMBOL_table_alias = 282,                          /* table_alias  */
    YYSYMBOL_with_clause = 283,                          /* with_clause  */
    YYSYMBOL_with_expr_list = 284,                       /* with_expr_list  */
    YYSYMBOL_with_expr = 285,                            /* with_expr  */
    YYSYMBOL_join_clause = 286,                          /* join_clause  */
    YYSYMBOL_join_type = 287,                            /* join_type  */
    YYSYMBOL_show_statement = 288,                       /* show_statement  */
    YYSYMBOL_flush_statement = 289,                      /* flush_statement  */
    YYSYMBOL_optimize_statement = 290,                   /* optimize_statement  */
    YYSYMBOL_command_statement = 291,                    /* command_statement  */
    YYSYMBOL_compact_statement = 292,                    /* compact_statement  */
    YYSYMBOL_admin_statement = 293,                      /* admin_statement  */
    YYSYMBOL_alter_statement = 294,                      /* alter_statement  */
    YYSYMBOL_check_statement = 295,                      /* check_statement  */
    YYSYMBOL_expr_array = 296,                           /* expr_array  */
    YYSYMBOL_insert_row_list = 297,                      /* insert_row_list  */
    YYSYMBOL_expr_alias = 298,                           /* expr_alias  */
    YYSYMBOL_expr = 299,                                 /* expr  */
    YYSYMBOL_operand = 300,                              /* operand  */
    YYSYMBOL_match_tensor_expr = 301,                    /* match_tensor_expr  */
    YYSYMBOL_match_vector_expr = 302,                    /* match_vector_expr  */
    YYSYMBOL_match_sparse_expr = 303,                    /* match_sparse_expr  */
    YYSYMBOL_match_text_expr = 304,                      /* match_text_expr  */
    YYSYMBOL_query_expr = 305,                           /* query_expr  */
    YYSYMBOL_fusion_expr = 306,                          /* fusion_expr  */
    YYSYMBOL_sub_search = 307,                           /* sub_search  */
    YYSYMBOL_sub_search_array = 308,                     /* sub_search_array  */
    YYSYMBOL_function_expr = 309,                        /* function_expr  */
    YYSYMBOL_conjunction_expr = 310,                     /* conjunction_expr  */
    YYSYMBOL_between_expr = 311,                         /* between_expr  */
    YYSYMBOL_in_expr = 312,                              /* in_expr  */
    YYSYMBOL_case_expr = 313,                            /* case_expr  */
    YYSYMBOL_case_check_array = 314,                     /* case_check_array  */
    YYSYMBOL_cast_expr = 315,                            /* cast_expr  */
    YYSYMBOL_subquery_expr = 316,                        /* subquery_expr  */
    YYSYMBOL_column_expr = 317,                          /* column_expr  */
    YYSYMBOL_constant_expr = 318,                        /* constant_expr  */
    YYSYMBOL_common_array_expr = 319,                    /* common_array_expr  */
    YYSYMBOL_common_sparse_array_expr = 320,             /* common_sparse_array_expr  */
    YYSYMBOL_subarray_array_expr = 321,                  /* subarray_array_expr  */
    YYSYMBOL_unclosed_subarray_array_expr = 322,         /* unclosed_subarray_array_expr  */
    YYSYMBOL_sparse_array_expr = 323,                    /* sparse_array_expr  */
    YYSYMBOL_long_sparse_array_expr = 324,               /* long_sparse_array_expr  */
    YYSYMBOL_unclosed_long_sparse_array_expr = 325,      /* unclosed_long_sparse_array_expr  */
    YYSYMBOL_double_sparse_array_expr = 326,             /* double_sparse_array_expr  */
    YYSYMBOL_unclosed_double_sparse_array_expr = 327,    /* unclosed_double_sparse_array_expr  */
    YYSYMBOL_empty_array_expr = 328,                     /* empty_array_expr  */
    YYSYMBOL_curly_brackets_expr = 329,                  /* curly_brackets_expr  */
    YYSYMBOL_unclosed_curly_brackets_expr = 330,         /* unclosed_curly_brackets_expr  */
    YYSYMBOL_int_sparse_ele = 331,                       /* int_sparse_ele  */
    YYSYMBOL_float_sparse_ele = 332,                     /* float_sparse_ele  */
    YYSYMBOL_array_expr = 333,                           /* array_expr  */
    YYSYMBOL_long_array_expr = 334,                      /* long_array_expr  */
    YYSYMBOL_unclosed_long_array_expr = 335,             /* unclosed_long_array_expr  */
    YYSYMBOL_double_array_expr = 336,                    /* double_array_expr  */
    YYSYMBOL_unclosed_double_array_expr = 337,           /* unclosed_double_array_expr  */
    YYSYMBOL_interval_expr = 338,                        /* interval_expr  */
    YYSYMBOL_copy_option_list = 339,                     /* copy_option_list  */
    YYSYMBOL_copy_option = 340,                          /* copy_option  */
    YYSYMBOL_file_path = 341,                            /* file_path  */
    YYSYMBOL_if_exists = 342,                            /* if_exists  */
    YYSYMBOL_if_not_exists = 343,                        /* if_not_exists  */
    YYSYMBOL_semicolon = 344,                            /* semicolon  */
    YYSYMBOL_if_not_exists_info = 345,                   /* if_not_exists_info  */
    YYSYMBOL_with_index_param_list = 346,                /* with_index_param_list  */
    YYSYMBOL_optional_table_properties_list = 347,       /* optional_table_properties_list  */
    YYSYMBOL_index_param_list = 348,                     /* index_param_list  */
    YYSYMBOL_index_param = 349,                          /* index_param  */
    YYSYMBOL_index_info = 350                            /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;

/* Unqualified %code blocks.  */
#line 98 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 477 "parser.cpp"

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
#define YYFINAL 138
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST 1586

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS 231
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS 120
/* YYNRULES -- Number of rules.  */
#define YYNRULES 565
/* YYNSTATES -- Number of states.  */
#define YYNSTATES 1273

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK 467

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX) (0 <= (YYX) && (YYX) <= YYMAXUTOK ? YY_CAST(yysymbol_kind_t, yytranslate[YYX]) : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] = {
    0,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   220, 2,   2,   223, 224, 218, 216, 227, 217, 225, 219, 2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   230, 226, 214, 213, 215, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   221, 2,   222, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   229, 2,   228, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
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
    193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] = {
    0,    520,  520,  524,  530,  537,  538,  539,  540,  541,  542,  543,  544,  545,  546,  547,  548,  549,  550,  551,  552,  554,  555,  556,
    557,  558,  559,  560,  561,  562,  563,  564,  565,  572,  589,  606,  622,  651,  666,  698,  716,  734,  762,  793,  797,  802,  806,  812,
    815,  822,  842,  864,  888,  914,  918,  924,  925,  926,  927,  928,  929,  930,  931,  932,  933,  934,  935,  936,  937,  938,  939,  940,
    941,  942,  943,  944,  947,  949,  950,  951,  952,  955,  956,  957,  958,  959,  960,  961,  962,  963,  964,  965,  966,  967,  968,  969,
    970,  971,  972,  973,  974,  975,  976,  977,  978,  979,  980,  981,  982,  983,  984,  985,  986,  987,  988,  989,  990,  991,  992,  993,
    994,  995,  996,  997,  998,  999,  1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1010, 1011, 1012, 1013, 1014, 1015, 1016, 1017,
    1018, 1019, 1020, 1021, 1025, 1048, 1052, 1062, 1065, 1068, 1071, 1075, 1078, 1083, 1088, 1095, 1101, 1111, 1127, 1165, 1181, 1184, 1191, 1208,
    1217, 1230, 1234, 1239, 1252, 1265, 1280, 1295, 1310, 1333, 1386, 1441, 1492, 1495, 1498, 1507, 1517, 1520, 1524, 1529, 1556, 1559, 1564, 1581,
    1584, 1588, 1592, 1597, 1603, 1606, 1609, 1613, 1617, 1619, 1623, 1625, 1628, 1632, 1635, 1639, 1642, 1646, 1651, 1655, 1658, 1662, 1665, 1669,
    1672, 1676, 1679, 1683, 1686, 1689, 1692, 1700, 1703, 1718, 1718, 1720, 1734, 1743, 1748, 1757, 1762, 1767, 1773, 1780, 1783, 1787, 1790, 1795,
    1807, 1814, 1828, 1831, 1834, 1837, 1840, 1843, 1846, 1852, 1856, 1860, 1864, 1868, 1875, 1879, 1883, 1887, 1891, 1896, 1900, 1905, 1909, 1913,
    1917, 1923, 1929, 1936, 1947, 1958, 1969, 1981, 1993, 2006, 2020, 2031, 2046, 2063, 2080, 2098, 2102, 2106, 2113, 2119, 2123, 2127, 2133, 2137,
    2141, 2145, 2152, 2156, 2163, 2167, 2171, 2175, 2180, 2184, 2189, 2194, 2198, 2203, 2207, 2211, 2216, 2225, 2229, 2233, 2237, 2245, 2259, 2265,
    2270, 2276, 2282, 2290, 2296, 2302, 2308, 2314, 2322, 2328, 2334, 2340, 2346, 2354, 2360, 2366, 2374, 2382, 2388, 2394, 2400, 2406, 2410, 2422,
    2435, 2441, 2448, 2456, 2465, 2475, 2485, 2496, 2507, 2519, 2531, 2541, 2552, 2564, 2577, 2581, 2586, 2591, 2597, 2601, 2605, 2612, 2616, 2620,
    2627, 2633, 2641, 2647, 2651, 2657, 2661, 2667, 2672, 2677, 2684, 2693, 2703, 2713, 2725, 2736, 2755, 2759, 2775, 2779, 2784, 2794, 2816, 2822,
    2826, 2827, 2828, 2829, 2830, 2832, 2835, 2841, 2844, 2845, 2846, 2847, 2848, 2849, 2850, 2851, 2852, 2853, 2857, 2873, 2891, 2909, 2967, 3017,
    3071, 3129, 3154, 3177, 3198, 3219, 3228, 3239, 3250, 3264, 3271, 3281, 3287, 3299, 3302, 3305, 3308, 3311, 3314, 3318, 3322, 3327, 3335, 3343,
    3352, 3359, 3366, 3373, 3380, 3387, 3394, 3401, 3408, 3415, 3422, 3429, 3437, 3445, 3453, 3461, 3469, 3477, 3485, 3493, 3501, 3509, 3517, 3525,
    3555, 3563, 3572, 3580, 3589, 3597, 3603, 3610, 3616, 3623, 3628, 3635, 3642, 3650, 3663, 3669, 3675, 3682, 3690, 3697, 3704, 3709, 3719, 3724,
    3729, 3734, 3739, 3744, 3749, 3754, 3759, 3764, 3767, 3770, 3773, 3777, 3780, 3783, 3786, 3790, 3793, 3796, 3800, 3804, 3809, 3814, 3817, 3821,
    3825, 3832, 3839, 3843, 3850, 3857, 3861, 3864, 3868, 3872, 3877, 3881, 3885, 3888, 3892, 3896, 3901, 3906, 3910, 3915, 3920, 3926, 3932, 3938,
    3944, 3950, 3956, 3962, 3968, 3974, 3980, 3986, 3997, 4001, 4006, 4038, 4048, 4053, 4058, 4063, 4069, 4073, 4074, 4076, 4077, 4079, 4080, 4092,
    4100, 4104, 4107, 4111, 4114, 4118, 4122, 4127, 4133, 4143, 4153, 4161, 4172, 4204};
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

#define YYPACT_NINF (-775)

#define yypact_value_is_default(Yyn) ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-553)

#define yytable_value_is_error(Yyn) ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] = {
    1026, 90,   45,   337,  88,   42,   88,   317,  241,  929,  51,   126,  131,  309,  188,  88,   195,  32,   120,  345,  294,  65,   -34,  232,
    106,  -775, -775, -775, -775, -775, -775, -775, -775, 452,  -775, -775, 269,  -775, -775, -775, -775, -775, -775, -775, -775, 225,  225,  225,
    225,  36,   298,  88,   334,  334,  334,  334,  334,  343,  189,  348,  88,   -41,  422,  430,  437,  324,  -775, -775, -775, -775, -775, -775,
    -775, 452,  -775, -775, -775, -775, -775, 166,  395,  417,  490,  88,   -775, -775, -775, -775, -775, 19,   -775, 303,  328,  -775, 514,  -775,
    -775, 53,   404,  -775, 550,  -775, -775, -775, 332,  270,  60,   95,   -775, 545,  88,   88,   566,  -775, -775, -775, -775, -775, -775, 541,
    271,  -775, 605,  424,  426,  329,  472,  447,  619,  432,  556,  456,  458,  88,   -775, -775, 393,  434,  -775, 1002, -775, 641,  -775, -775,
    6,    612,  -775, 616,  618,  682,  88,   88,   88,   698,  643,  649,  492,  639,  714,  88,   88,   88,   723,  -775, 736,  743,  686,  744,
    744,  676,  77,   94,   107,  -775, 547,  625,  627,  -775, 449,  -775, -775, -775, 759,  -775, 770,  -775, -775, 744,  -775, -775, 771,  -775,
    -775, -775, -775, -775, 645,  -775, 650,  88,   391,  -775, 712,  559,  195,  744,  -775, 780,  -775, 620,  -775, 779,  -775, -775, 787,  -775,
    785,  -775, 789,  792,  -775, 793,  741,  794,  604,  798,  799,  -775, -775, -775, -775, -775, 6,    -775, -775, -775, 676,  751,  738,  731,
    673,  -2,   -775, 492,  -775, 88,   275,  805,  18,   -775, -775, -775, -775, -775, 747,  -775, 603,  -40,  -775, 676,  -775, -775, 740,  746,
    614,  -775, -775, 1021, 737,  626,  628,  477,  831,  832,  837,  838,  -775, -775, 844,  629,  630,  632,  634,  636,  640,  642,  425,  644,
    646,  842,  842,  -775, 37,   571,  13,   164,  -775, -4,   613,  -775, -775, -775, -775, -775, -775, -775, -775, -775, -775, -775, -775, -775,
    637,  -775, -775, -775, -39,  -775, -775, 102,  -775, 185,  -775, -775, 443,  -775, -775, 196,  -775, 235,  -775, -775, -775, -775, -775, -775,
    -775, -775, -775, -775, -775, -775, -775, -775, -775, -775, -775, -775, -775, 840,  848,  -775, -775, -775, -775, -775, -775, -775, -775, -775,
    786,  820,  822,  795,  88,   269,  -775, -775, -775, 866,  274,  -775, 865,  -775, -775, 800,  342,  -775, 875,  -775, -775, 657,  661,  -37,
    676,  676,  814,  -775, 883,  -34,  43,   833,  666,  887,  888,  -775, -775, 339,  675,  -775, 88,   676,  743,  -775, 354,  677,  678,  283,
    -775, -775, -775, -775, -775, -775, -775, -775, -775, -775, -775, -775, 842,  679,  1000, 818,  676,  676,  135,  439,  -775, -775, -775, -775,
    1021, -775, 676,  676,  676,  676,  676,  676,  899,  681,  683,  685,  687,  903,  905,  363,  363,  -775, 688,  -775, -775, -775, -775, 689,
    -93,  -775, -775, 834,  676,  909,  676,  676,  -48,  691,  -12,  842,  842,  842,  842,  842,  842,  842,  842,  842,  842,  842,  842,  842,
    842,  21,   -775, 699,  -775, 910,  -775, 916,  99,   -775, -775, 917,  -775, 919,  884,  631,  705,  -775, 706,  707,  928,  -775, 709,  -775,
    931,  -775, 372,  932,  768,  769,  -775, -775, -775, 676,  862,  713,  -775, 202,  354,  676,  -775, -775, 47,   1161, 810,  718,  340,  -775,
    -775, -775, -34,  939,  812,  -775, -775, -775, 944,  676,  721,  -775, 354,  -775, 7,    7,    676,  -775, 407,  818,  791,  727,  1,    -19,
    470,  -775, 676,  676,  -50,  125,  127,  137,  143,  155,  880,  676,  25,   676,  953,  745,  412,  684,  -775, -775, 744,  -775, -775, -775,
    807,  742,  842,  571,  836,  -775, 963,  963,  228,  228,  930,  963,  963,  228,  228,  363,  363,  -775, -775, -775, -775, -775, -775, 752,
    -775, 754,  -775, -775, -775, -775, 962,  965,  -775, 970,  805,  971,  -775, -775, -775, 969,  -775, -775, 972,  973,  761,  17,   811,  676,
    -775, -775, -775, 354,  982,  -775, -775, -775, -775, -775, -775, -775, -775, -775, -775, -775, 772,  -775, -775, -775, -775, -775, -775, -775,
    -775, -775, -775, -775, -775, 773,  774,  775,  776,  778,  781,  782,  783,  330,  784,  805,  961,  43,   452,  796,  989,  -775, 413,  804,
    988,  1004, 1017, 1005, -775, 1023, 414,  -775, 428,  436,  -775, 821,  -775, 1161, 676,  -775, 676,  5,    109,  -775, -775, -775, -775, -775,
    -775, 842,  -84,  -775, 226,  -79,  817,  75,   823,  -775, 1019, -775, -775, 956,  571,  963,  824,  442,  -775, 842,  1040, 1045, 647,  1007,
    851,  460,  -775, 475,  495,  -775, 1051, -775, -775, -34,  839,  534,  -775, 92,   -775, 388,  686,  -775, -775, 1055, 1161, 1161, 1100, 1197,
    1214, 1237, 1254, 1277, 920,  938,  -775, -775, 266,  -775, 935,  805,  497,  846,  936,  -775, 906,  -775, -775, 676,  -775, -775, -775, -775,
    -775, -775, 7,    -775, -775, -775, 850,  354,  71,   -775, 676,  386,  676,  699,  854,  1072, 852,  676,  -775, 853,  855,  861,  501,  -775,
    -775, 1000, 1081, -775, 1082, 538,  -775, 970,  -775, -775, 971,  524,  38,   17,   1033, -775, -775, -775, -775, -775, -775, 1041, -775, 1096,
    -775, -775, -775, -775, -775, -775, -775, -775, 876,  1054, 503,  885,  507,  -775, 881,  886,  891,  892,  893,  894,  896,  900,  904,  1018,
    908,  914,  915,  946,  947,  948,  957,  958,  959,  960,  1034, 968,  974,  975,  976,  980,  981,  994,  995,  996,  1001, 1044, 1003, 1006,
    1009, 1010, 1011, 1012, 1013, 1014, 1015, 1016, 1063, 1020, 1047, 1048, 1050, 1065, 1066, 1069, 1070, 1071, 1073, 1064, 1083, 1086, 1087, 1088,
    1089, 1090, 1094, 1095, 1097, 1098, 1107, 1099, -775, -775, 99,   -775, 999,  1024, 508,  -775, 970,  1133, 1154, 509,  -775, -775, -775, 354,
    -775, -64,  1105, 16,   1106, -775, -775, -775, 1109, 1123, 985,  354,  -775, 7,    -775, -775, -775, -775, -775, -775, -775, -775, -775, -775,
    1221, -775, 92,   534,  17,   17,   1008, 388,  1176, 1178, -775, 1238, -775, -775, 1161, 1239, 1240, 1242, 1261, 1272, 1293, 1303, 1331, 1332,
    1112, 1343, 1344, 1347, 1348, 1349, 1350, 1351, 1355, 1356, 1358, 1138, 1360, 1366, 1367, 1370, 1371, 1372, 1373, 1378, 1379, 1380, 1160, 1382,
    1383, 1384, 1385, 1386, 1387, 1388, 1389, 1390, 1391, 1171, 1393, 1394, 1395, 1396, 1398, 1399, 1400, 1401, 1402, 1403, 1183, 1405, 1406, 1407,
    1408, 1409, 1410, 1411, 1412, 1413, 1414, 1194, 1416, -775, 1419, 1420, -775, 520,  -775, 786,  -775, 1421, 1422, 1423, 98,   1204, -775, 530,
    1424, -775, -775, 1369, 805,  -775, 676,  676,  -775, 1205, -775, 1208, 1209, 1210, 1211, 1212, 1213, 1215, 1216, 1217, 1432, 1218, 1219, 1220,
    1222, 1223, 1224, 1225, 1226, 1227, 1228, 1439, 1229, 1230, 1231, 1232, 1233, 1234, 1235, 1236, 1241, 1243, 1455, 1244, 1245, 1246, 1247, 1248,
    1249, 1250, 1251, 1252, 1253, 1456, 1255, 1256, 1257, 1258, 1259, 1260, 1262, 1263, 1264, 1265, 1457, 1266, 1267, 1268, 1269, 1270, 1271, 1273,
    1274, 1275, 1276, 1458, 1278, -775, -775, -775, -775, 1279, 1280, 1281, 852,  1300, -775, 466,  676,  536,  761,  354,  -775, -775, -775, -775,
    -775, -775, -775, -775, -775, -775, 1285, -775, -775, -775, -775, -775, -775, -775, -775, -775, -775, 1286, -775, -775, -775, -775, -775, -775,
    -775, -775, -775, -775, 1287, -775, -775, -775, -775, -775, -775, -775, -775, -775, -775, 1288, -775, -775, -775, -775, -775, -775, -775, -775,
    -775, -775, 1289, -775, -775, -775, -775, -775, -775, -775, -775, -775, -775, 1290, -775, 1474, 1481, 255,  1291, 1292, 1448, 1495, 1497, -775,
    -775, -775, 354,  -775, -775, -775, -775, -775, -775, -775, 1294, 1295, 852,  786,  1338, 1282, 1511, 659,  297,  1296, 1514, 1299, -775, 1471,
    1520, 695,  1519, -775, 852,  786,  852,  -45,  1304, 1302, 1522, -775, 1482, 1306, -775, 1307, 1493, 1494, -775, 1530, -775, -775, -775, -26,
    139,  -775, 1312, 1313, 1499, 1500, -775, 1501, 1502, 1533, -775, -775, 1319, -775, 1320, 1321, 1541, 1543, 786,  1323, 1324, -775, 786,  -775,
    -775};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] = {
    236, 0,   0,   0,   0,   0,   0,   0,   236, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   236, 0,   550, 3,   5,   10,  12,
    13,  11,  6,   7,   9,   181, 180, 0,   8,   14,  15,  16,  17,  18,  19,  20,  548, 548, 548, 548, 548, 0,   0,   546, 546, 546, 546, 546, 0,
    229, 0,   0,   0,   0,   0,   0,   236, 167, 21,  26,  28,  27,  22,  23,  25,  24,  29,  30,  31,  32,  0,   294, 299, 0,   0,   250, 251, 249,
    255, 259, 0,   256, 0,   0,   252, 0,   254, 279, 280, 0,   257, 0,   290, 292, 293, 0,   286, 302, 296, 301, 0,   0,   0,   0,   305, 306, 307,
    308, 310, 309, 0,   235, 237, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   376, 333, 0,   0,   1,   236, 2,   219, 221, 222, 0,
    204, 186, 192, 0,   0,   0,   0,   0,   0,   0,   0,   165, 0,   0,   0,   0,   0,   0,   330, 0,   0,   214, 0,   0,   0,   0,   0,   0,   166,
    0,   0,   0,   266, 267, 260, 261, 262, 0,   263, 0,   253, 281, 0,   258, 291, 0,   284, 283, 287, 288, 304, 0,   298, 0,   0,   0,   335, 0,
    0,   0,   0,   357, 0,   367, 0,   368, 0,   354, 355, 0,   350, 0,   363, 365, 0,   358, 0,   0,   0,   0,   0,   0,   377, 185, 184, 4,   220,
    0,   182, 183, 203, 0,   0,   200, 0,   34,  0,   35,  165, 551, 0,   0,   0,   236, 545, 172, 174, 173, 175, 0,   230, 0,   214, 169, 0,   161,
    544, 0,   0,   474, 478, 481, 482, 0,   0,   0,   0,   0,   0,   0,   0,   479, 480, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   476, 0,   236, 0,   0,   378, 383, 384, 398, 396, 399, 397, 400, 401, 393, 388, 387, 386, 394, 395, 385, 392, 391, 489, 492, 0,   493, 501,
    0,   502, 0,   494, 490, 0,   491, 516, 0,   517, 0,   488, 314, 316, 315, 312, 313, 319, 321, 320, 317, 318, 324, 326, 325, 322, 323, 289, 295,
    300, 0,   0,   269, 268, 274, 264, 265, 282, 285, 303, 297, 554, 0,   0,   0,   0,   0,   238, 311, 360, 0,   351, 356, 0,   364, 359, 0,   0,
    366, 0,   331, 332, 0,   0,   206, 0,   0,   202, 547, 0,   236, 0,   0,   0,   0,   0,   329, 159, 0,   0,   163, 0,   0,   0,   168, 213, 0,
    0,   0,   525, 524, 527, 526, 529, 528, 531, 530, 533, 532, 535, 534, 0,   0,   440, 236, 0,   0,   0,   0,   483, 484, 485, 486, 0,   487, 0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   442, 441, 522, 519, 509, 499, 504, 507, 0,   0,   511, 512, 0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   498, 0,   503, 0,   506, 0,   0,   510, 518, 0,   521,
    0,   275, 270, 0,   375, 0,   0,   0,   334, 0,   369, 0,   352, 0,   0,   0,   0,   362, 189, 188, 0,   208, 191, 193, 198, 199, 0,   187, 33,
    37,  0,   0,   0,   0,   43,  47,  48,  236, 0,   41,  328, 327, 164, 0,   0,   162, 176, 171, 170, 0,   0,   0,   429, 0,   236, 0,   0,   0,
    0,   0,   465, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   212, 0,   0,   390, 389, 0,   379, 382, 458, 459, 0,   0,   236,
    0,   439, 449, 450, 453, 454, 0,   456, 448, 451, 452, 444, 443, 445, 446, 447, 475, 477, 500, 0,   505, 0,   508, 513, 520, 523, 0,   0,   271,
    0,   0,   0,   372, 239, 353, 0,   336, 361, 0,   0,   205, 0,   210, 0,   196, 197, 195, 201, 0,   55,  58,  59,  56,  57,  60,  61,  77,  62,
    64,  63,  80,  67,  68,  69,  65,  66,  70,  71,  72,  73,  74,  75,  76,  0,   0,   0,   0,   0,   0,   0,   0,   554, 0,   0,   556, 0,   40,
    0,   0,   160, 0,   0,   0,   0,   0,   0,   540, 0,   0,   536, 0,   0,   430, 0,   470, 0,   0,   463, 0,   0,   0,   437, 436, 435, 434, 433,
    432, 0,   0,   474, 0,   0,   0,   0,   0,   419, 0,   515, 514, 0,   236, 457, 0,   0,   438, 0,   0,   0,   276, 272, 559, 0,   557, 0,   0,
    45,  337, 370, 371, 236, 207, 223, 225, 234, 226, 0,   214, 194, 39,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   152, 153, 156, 149,
    156, 0,   0,   0,   36,  44,  565, 42,  380, 0,   542, 541, 539, 538, 543, 179, 0,   177, 431, 471, 0,   467, 0,   466, 0,   0,   0,   0,   0,
    0,   212, 0,   417, 0,   0,   0,   0,   472, 461, 460, 0,   277, 0,   0,   553, 0,   374, 373, 0,   0,   0,   0,   0,   243, 244, 245, 246, 242,
    247, 0,   232, 0,   227, 423, 421, 424, 422, 425, 426, 427, 209, 218, 0,   0,   0,   53,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   154, 151, 0,
    150, 50,  49,  0,   158, 0,   0,   0,   0,   537, 469, 464, 468, 455, 0,   0,   0,   0,   495, 497, 496, 212, 0,   0,   211, 420, 0,   473, 462,
    278, 273, 560, 561, 563, 562, 558, 46,  0,   338, 234, 224, 0,   0,   231, 0,   0,   216, 79,  0,   147, 148, 0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   155, 0,   0,   157, 0,   38,  554, 381, 0,   0,   0,   0,   0,   418, 0,   339, 228, 240, 0,   0,   428, 0,   0,   190, 0,   54,  0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   52,  51,  555, 564, 0,   0,   212, 212, 413, 178, 0,   0,   0,   217, 215, 78,  84,  85,  82,  83,  86,  87,
    88,  89,  90,  0,   81,  128, 129, 126, 127, 130, 131, 132, 133, 134, 0,   125, 95,  96,  93,  94,  97,  98,  99,  100, 101, 0,   92,  106, 107,
    104, 105, 108, 109, 110, 111, 112, 0,   103, 139, 140, 137, 138, 141, 142, 143, 144, 145, 0,   136, 117, 118, 115, 116, 119, 120, 121, 122, 123,
    0,   114, 0,   0,   0,   0,   0,   0,   0,   0,   341, 340, 346, 241, 233, 91,  135, 102, 113, 146, 124, 212, 0,   212, 554, 414, 0,   347, 342,
    0,   0,   0,   0,   412, 0,   0,   0,   0,   343, 212, 554, 212, 554, 0,   0,   0,   348, 344, 0,   408, 0,   0,   0,   411, 0,   415, 349, 345,
    554, 402, 410, 0,   0,   0,   0,   407, 0,   0,   0,   416, 406, 0,   404, 0,   0,   0,   0,   554, 0,   0,   409, 554, 403, 405};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] = {
    -775, -775, -775, 1415, 1484, 260,  -775, -775, 890,  -580, -775, -670, -775, 801,  797,  -775, -601, 290,  307,  1309, -775, 308,  -775, 1148,
    315,  322,  -6,   1534, -18,  1192, 1326, -97,  -775, -775, 937,  -775, -775, -775, -775, -775, -775, -775, -774, -250, -775, -775, -775, -775,
    756,  -171, 34,   621,  -775, -775, 1359, -775, -775, 335,  366,  384,  387,  401,  -775, -775, -775, -235, -775, 1102, -259, -260, -719, -712,
    -698, -696, -694, -687, 622,  -775, -775, -775, -775, -775, -775, 1140, -775, -775, 1022, -289, -286, -775, -775, -775, 788,  -775, -775, -775,
    -775, 790,  -775, -775, 1079, 1080, 802,  -775, -775, -775, -775, 1297, -532, 803,  -155, 532,  581,  -775, -775, -658, -775, 664,  777,  -775};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] = {0,   23,  24,  25,  66,   26,  526, 724, 527, 528, 828, 661, 752, 753, 900, 529, 397, 27,  28,  248,
                                         29,  30,  257, 258, 31,   32,  33,  34,  35,  146, 233, 147, 238, 515, 516, 626, 386, 520, 236, 514,
                                         622, 736, 704, 260, 1040, 945, 144, 730, 731, 732, 733, 816, 36,  120, 121, 734, 813, 37,  38,  39,
                                         40,  41,  42,  43,  44,   295, 538, 296, 297, 298, 299, 300, 301, 302, 303, 304, 823, 824, 305, 306,
                                         307, 308, 309, 427, 310,  311, 312, 313, 314, 916, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324,
                                         453, 454, 325, 326, 327,  328, 329, 330, 678, 679, 262, 158, 149, 140, 154, 497, 758, 721, 722, 532};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] = {
    404,  383,  73,   754,  137,  458,  452,  403,  422,  723,  922,  680,  426,  263,  145,  774,  817,  265,  266,  267,  58,   449,  450,  818,
    596,  179,  259,  577,  699,  447,  448,  725,  356,  496,  456,  167,  168,  819,  59,   820,  61,   821,  449,  450,  16,   513,  523,  234,
    822,  118,  367,  461,  496,  389,  109,  398,  685,  186,  141,  73,   142,  580,  756,  462,  463,  143,  195,  141,  686,  142,  827,  829,
    462,  463,  143,  672,  673,  462,  463,  784,  331,  51,   332,  333,  110,  156,  674,  675,  676,  777,  1240, 58,   462,  463,  166,  814,
    16,   336,  -552, 337,  338,  197,  1116, 265,  266,  267,  462,  463,  148,  1252, 341,  578,  342,  343,  272,  273,  274,  178,  60,   581,
    275,  1241, 45,   46,   47,   517,  518,  123,  48,   49,   187,  571,  124,  196,  125,  382,  126,  462,  463,  334,  1253, 785,  540,  780,
    200,  201,  1029, 815,  782,  276,  277,  278,  462,  463,  902,  910,  339,  462,  463,  111,  422,  462,  463,  1025, 785,  550,  551,  227,
    198,  344,  112,  524,  546,  525,  691,  556,  557,  558,  559,  560,  561,  628,  169,  482,  241,  242,  243,  402,  483,  22,   460,  117,
    677,  251,  252,  253,  778,  598,  119,  603,  272,  273,  274,  575,  576,  122,  275,  582,  583,  584,  585,  586,  587,  588,  589,  590,
    591,  592,  593,  594,  595,  390,  935,  425,  817,  135,  180,  462,  463,  232,  399,  818,  138,  360,  292,  276,  277,  278,  451,  597,
    729,  457,  294,  291,  65,   819,  335,  820,  1,    821,  2,    3,    4,    5,    6,    7,    822,  9,    292,  451,  627,  1212, 938,  340,
    11,   462,  463,  12,   67,   13,   14,   15,   624,  625,  1255, 455,  345,  145,  620,  392,  1042, 462,  463,  462,  463,  50,   264,  265,
    266,  267,  127,  462,  463,  462,  463,  689,  690,  148,  68,   462,  463,  155,  670,  1228, 698,  1256, 701,  393,  394,  681,  128,  462,
    463,  504,  129,  69,   70,   130,  711,  16,   292,  785,  505,  71,   484,  67,   395,  133,  294,  485,  72,   1,    139,  2,    3,    4,
    5,    6,    7,    748,  9,    1194, 1195, 74,   713,  134,  163,  11,   544,  692,  12,   693,  13,   14,   15,   68,   268,  269,  462,  463,
    466,  694,  165,  785,  517,  1112, 270,  695,  271,  52,   53,   54,   69,   70,   75,   55,   56,   131,  132,  696,  71,   -553, -553, 522,
    272,  273,  274,  72,   459,  174,  275,  460,  76,   1031, 749,  77,   750,  751,  501,  898,  74,   361,  16,   748,  615,  549,  157,  486,
    496,  78,   188,  17,   487,  616,  164,  362,  363,  709,  490,  276,  277,  278,  279,  491,  280,  170,  281,  775,  282,  776,  283,  75,
    284,  171,  539,  1121, 1219, 779,  1221, 19,   172,  285,  -553, -553, 476,  477,  478,  479,  480,  76,   21,   481,  77,   781,  1237, 793,
    1239, 492,  749,  548,  750,  751,  493,  286,  22,   287,  78,   288,  175,  113,  114,  115,  141,  193,  142,  790,  194,  181,  182,  143,
    264,  265,  266,  267,  62,   63,   825,  116,  349,  64,   176,  350,  351,  177,  17,   914,  352,  353,  204,  289,  290,  291,  183,  184,
    292,  1197, 293,  545,  1198, 1199, 462,  463,  294,  1200, 1201, 208,  209,  185,  466,  911,  210,  913,  19,   553,  666,  554,  923,  555,
    907,  683,  508,  509,  57,   21,   190,  191,  192,  467,  468,  469,  470,  930,  931,  932,  933,  472,  441,  22,   442,  443,  268,  269,
    444,  189,  687,  1222, 688,  936,  555,  937,  270,  712,  271,  535,  664,  425,  536,  665,  286,  202,  287,  1238, 288,  1242, 264,  265,
    266,  267,  272,  273,  274,  478,  479,  480,  275,  159,  160,  161,  162,  1254, 806,  -248, 807,  808,  809,  810,  203,  811,  812,  473,
    474,  475,  476,  477,  478,  479,  480,  199,  1269, 1017, 912,  205,  1272, 276,  277,  278,  279,  228,  280,  206,  281,  207,  282,  222,
    283,  223,  284,  150,  151,  152,  153,  682,  224,  211,  460,  285,  705,  762,  769,  706,  460,  770,  221,  212,  268,  269,  213,  214,
    215,  16,   216,  225,  771,  226,  270,  770,  271,  286,  229,  287,  772,  288,  231,  460,  217,  218,  792,  219,  220,  460,  488,  489,
    272,  273,  274,  607,  608,  235,  275,  264,  265,  266,  267,  237,  798,  240,  464,  799,  465,  707,  708,  239,  789,  289,  290,  291,
    794,  795,  292,  800,  293,  244,  536,  1226, 1227, 245,  294,  276,  277,  278,  279,  246,  280,  249,  281,  247,  282,  250,  283,  801,
    284,  903,  802,  804,  536,  927,  254,  946,  460,  285,  947,  949,  1020, 1024, 950,  536,  460,  1234, 1235, 255,  264,  265,  266,  267,
    1111, 466,  256,  799,  261,  268,  269,  286,  259,  287,  1118, 288,  347,  770,  348,  270,  1203, 271,  354,  536,  467,  468,  469,  470,
    471,  1034, 1035, 346,  472,  355,  364,  357,  358,  272,  273,  274,  1123, 359,  365,  275,  368,  370,  369,  289,  290,  291,  371,  372,
    292,  373,  293,  374,  376,  375,  377,  378,  294,  379,  380,  1122, 384,  387,  385,  388,  396,  400,  420,  421,  276,  277,  278,  279,
    401,  280,  405,  281,  270,  282,  271,  283,  406,  284,  473,  474,  475,  476,  477,  478,  479,  480,  285,  428,  429,  407,  272,  273,
    274,  430,  431,  494,  275,  264,  265,  266,  267,  423,  432,  424,  434,  435,  495,  436,  286,  437,  287,  438,  288,  1202, 481,  439,
    496,  440,  498,  445,  499,  446,  503,  506,  500,  276,  277,  278,  279,  507,  280,  510,  281,  511,  282,  519,  283,  512,  284,  521,
    530,  531,  533,  534,  289,  290,  291,  285,  16,   292,  537,  293,  542,  543,  547,  562,  563,  294,  564,  567,  565,  568,  566,  572,
    574,  570,  579,  420,  599,  286,  569,  287,  292,  288,  601,  604,  605,  270,  606,  271,  609,  610,  611,  612,  79,   613,  618,  619,
    617,  614,  621,  662,  623,  663,  667,  272,  273,  274,  668,  669,  671,  275,  578,  684,  80,   289,  290,  291,  697,  702,  292,  81,
    293,  82,   83,   462,  84,   710,  294,  714,  718,  85,   86,   719,  703,  720,  523,  726,  727,  728,  276,  277,  278,  279,  716,  280,
    717,  281,  738,  282,  460,  283,  735,  284,  757,  761,  764,  739,  740,  741,  742,  743,  285,  744,  -549, 548,  745,  746,  747,  755,
    767,  1,    765,  2,    3,    4,    5,    6,    7,    8,    9,    10,   760,  766,  286,  787,  287,  11,   288,  763,  12,   768,  13,   14,
    15,   1,    788,  2,    3,    4,    5,    6,    7,    8,    9,    10,   783,  773,  708,  786,  791,  11,   707,  896,  12,   796,  13,   14,
    15,   803,  289,  290,  291,  826,  466,  292,  797,  293,  805,  897,  898,  904,  905,  294,  906,  548,  909,  915,  920,  924,  925,  921,
    16,   467,  468,  469,  470,  926,  715,  928,  929,  472,  940,  87,   88,   89,   90,   466,  91,   92,   941,  942,  93,   94,   95,   943,
    16,   96,   944,  97,   951,  948,  960,  98,   99,   952,  -553, -553, 469,  470,  953,  954,  955,  956,  -553, 957,  100,  101,  971,  958,
    102,  103,  104,  959,  466,  1018, 105,  961,  982,  1022, 106,  107,  108,  962,  963,  473,  474,  475,  476,  477,  478,  479,  480,  467,
    468,  469,  470,  993,  1004, 1023, 1019, 472,  408,  409,  410,  411,  412,  413,  414,  415,  416,  417,  418,  419,  17,   964,  965,  966,
    -553, 474,  475,  476,  477,  478,  479,  480,  967,  968,  969,  970,  18,   785,  830,  831,  832,  833,  834,  972,  17,   835,  836,  1015,
    19,   973,  974,  975,  837,  838,  839,  976,  977,  1030, 20,   21,   18,   473,  474,  475,  476,  477,  478,  479,  480,  978,  979,  980,
    19,   22,   840,  1032, 981,  1038, 983,  1036, 1039, 984,  20,   21,   985,  986,  987,  988,  989,  990,  991,  992,  1041, 1043, 1044, 994,
    1045, 22,   629,  630,  631,  632,  633,  634,  635,  636,  637,  638,  639,  640,  641,  642,  643,  644,  645,  1046, 646,  647,  648,  649,
    650,  651,  995,  996,  652,  997,  1047, 653,  654,  655,  656,  657,  658,  659,  660,  841,  842,  843,  844,  845,  998,  999,  846,  847,
    1000, 1001, 1002, 1048, 1003, 848,  849,  850,  852,  853,  854,  855,  856,  1049, 1005, 857,  858,  1006, 1007, 1008, 1009, 1010, 859,  860,
    861,  1011, 1012, 851,  1013, 1014, 1016, 863,  864,  865,  866,  867,  1026, 1027, 868,  869,  1028, 1050, 1051, 1052, 862,  870,  871,  872,
    874,  875,  876,  877,  878,  1053, 1054, 879,  880,  1055, 1056, 1057, 1058, 1059, 881,  882,  883,  1060, 1061, 873,  1062, 1063, 1064, 885,
    886,  887,  888,  889,  1065, 1066, 890,  891,  1067, 1068, 1069, 1070, 884,  892,  893,  894,  1071, 1072, 1073, 1074, 1075, 1076, 1077, 1078,
    1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1088, 1089, 895,  1090, 1091, 1092, 1093, 1094, 1095, 1096, 1097, 1098, 1099, 1100, 1101,
    1102, 1103, 1104, 1105, 1106, 1107, 1108, 1109, 1110, 1113, 1114, 1115, 1117, 1124, 1119, 1120, 1125, 1126, 1127, 1128, 1129, 1130, 1134, 1131,
    1132, 1133, 1135, 1136, 1137, 1145, 1138, 1139, 1140, 1141, 1142, 1143, 1144, 1146, 1147, 1148, 1149, 1150, 1151, 1152, 1153, 1156, 1167, 1178,
    1189, 1154, 1196, 1155, 1157, 1158, 1159, 1160, 1161, 1162, 1163, 1164, 1165, 1166, 1210, 1168, 1169, 1170, 1171, 1172, 1173, 1211, 1174, 1175,
    1176, 1177, 1179, 1180, 1181, 1182, 1183, 1184, 1215, 1185, 1186, 1187, 1188, 1216, 1190, 1217, 1223, 1224, 1191, 1192, 1193, 1204, 1205, 1206,
    1207, 1208, 1209, 1213, 1214, 1225, 1230, 1232, 1229, 1218, 1220, 1231, 1233, 1236, 1244, 1243, 1245, 1246, 1247, 1248, 1249, 1250, 1251, 1257,
    1263, 1258, 1259, 1260, 1261, 1262, 1264, 1265, 1267, 1266, 1268, 1270, 1271, 173,  541,  901,  391,  899,  230,  759,  136,  502,  381,  1033,
    737,  939,  573,  366,  600,  1037, 552,  602,  1021, 0,    917,  0,    918,  908,  0,    433,  934,  0,    0,    0,    0,    0,    0,    0,
    919,  0,    700};

static const yytype_int16 yycheck[] = {
    259, 236, 8,   661,  22,   294, 292,  257, 268, 610, 784, 543, 271, 168, 8,    685,  735, 4,    5,    6,   3,    5,   6,    735, 3,    6,   66,
    75,  3,   289, 290,  611,  187, 78,   293, 76,  77,  735, 4,   735, 6,   735,  5,    6,   78,   82,   3,   144,  735, 15,   205, 55,   78,  55,
    3,   37,  55,  4,    20,   65,  22,   73,  663, 156, 157, 27,  6,   20,  87,   22,   740, 741,  156,  157, 27,   68,  69,   156, 157,  4,   3,
    36,  5,   6,   33,   51,   79,  80,   81,  84,  135, 3,   156, 157, 60,  3,    78,   3,   62,   5,    6,   6,    4,   4,    5,   6,    156, 157,
    72,  135, 3,   159,  5,    6,   101,  102, 103, 83,  76,  131, 107, 166, 32,   33,   34,  384,  385,  7,   38,   39,  77,   224, 12,   73,  14,
    232, 16,  156, 157,  62,   166, 66,   401, 227, 110, 111, 920, 55,  227, 136,  137,  138, 156,  157,  755, 84,   62,  156,  157, 33,   420, 156,
    157, 227, 66,  424,  425,  133, 73,   62,  39,  128, 407, 130, 224, 434, 435,  436,  437, 438,  439,  134, 223,  222, 150,  151, 152,  227, 227,
    223, 227, 3,   185,  159,  160, 161,  87,  483, 3,   488, 101, 102, 103, 462,  463,  173, 107,  467,  468, 469,  470, 471,  472, 473,  474, 475,
    476, 477, 478, 479,  480,  223, 802,  88,  943, 160, 207, 156, 157, 223, 248,  943,  0,   199,  221,  136, 137,  138, 222,  218, 223,  228, 229,
    218, 3,   943, 169,  943,  7,   943,  9,   10,  11,  12,  13,  14,  943, 16,   221,  222, 519,  6,    224, 169,  23,  156,  157, 26,   8,   28,
    29,  30,  70,  71,   135,  293, 169,  8,   513, 245, 950, 156, 157, 156, 157,  195,  3,   4,    5,    6,   170,  156, 157,  156, 157,  554, 555,
    72,  8,   156, 157,  3,    537, 6,    563, 166, 565, 32,  33,  544, 190, 156,  157,  39,  194,  8,    8,   197,  578, 78,   221, 66,   48,  8,
    222, 65,  51,  33,   229,  227, 8,    7,   226, 9,   10,  11,  12,  13,  14,   73,   16,  1115, 1116, 8,   579,  51,  3,    23,  65,   224, 26,
    224, 28,  29,  30,   65,   73,  74,   156, 157, 132, 224, 14,  66,  623, 1023, 83,   224, 85,   32,   33,  34,   65,  65,   8,   38,   39,  32,
    33,  224, 65,  153,  154,  389, 101,  102, 103, 65,  224, 223, 107, 227, 8,    925,  128, 8,    130,  131, 364,  133, 65,   10,  78,   73,  32,
    423, 72,  222, 78,   8,    6,   170,  227, 41,  225, 24,  25,  572, 222, 136,  137,  138, 139,  227,  141, 3,    143, 686,  145, 688,  147, 65,
    149, 3,   400, 1036, 1210, 697, 1212, 198, 3,   158, 214, 215, 216, 217, 218,  219,  220, 65,   209,  225, 65,   227, 1228, 715, 1230, 222, 128,
    73,  130, 131, 227,  180,  223, 182,  65,  184, 73,  160, 161, 162, 20,  203,  22,   710, 206,  174,  175, 27,   3,   4,    5,   6,    167, 168,
    736, 178, 39,  172,  73,   42,  43,   3,   170, 781, 47,  48,  227, 216, 217,  218,  174, 175,  221,  39,  223,  224, 42,   43,  156,  157, 229,
    47,  48,  186, 187,  3,    132, 778,  191, 780, 198, 84,  530, 86,  785, 88,   763,  547, 188,  189,  195, 209,  202, 203,  204, 151,  152, 153,
    154, 3,   4,   5,    6,    159, 121,  223, 123, 124, 73,  74,  127, 3,   84,   1213, 86,  33,   88,   35,  83,   579, 85,   224, 224,  88,  227,
    227, 180, 3,   182,  1229, 184, 1231, 3,   4,   5,   6,   101, 102, 103, 218,  219,  220, 107,  53,   54,  55,   56,  1247, 56,  57,   58,  59,
    60,  61,  55,  63,   64,   213, 214,  215, 216, 217, 218, 219, 220, 62,  1266, 898,  224, 6,    1270, 136, 137,  138, 139,  224, 141,  195, 143,
    195, 145, 4,   147,  193,  149, 46,   47,  48,  49,  224, 76,  161, 227, 158,  224,  224, 224,  227,  227, 227,  195, 171,  73,  74,   174, 175,
    176, 78,  178, 195,  224,  195, 83,   227, 85,  180, 224, 182, 224, 184, 21,   227,  192, 193,  224,  195, 196,  227, 227,  228, 101,  102, 103,
    44,  45,  65,  107,  3,    4,   5,    6,   67,  224, 3,   73,  227, 75,  5,    6,    73,  710,  216,  217, 218,  49,  50,   221, 224,  223, 3,
    227, 44,  45,  62,   229,  136, 137,  138, 139, 62,  141, 74,  143, 223, 145,  3,    147, 224,  149,  224, 227,  729, 227,  224, 3,    224, 227,
    158, 227, 224, 224,  224,  227, 227,  227, 42,  43,  3,   3,   4,   5,   6,    224,  132, 3,    227,  4,   73,   74,  180,  66,  182,  224, 184,
    131, 227, 131, 83,   224,  85,  3,    227, 151, 152, 153, 154, 155, 940, 941,  224,  159, 3,    62,   4,   131,  101, 102,  103, 1039, 131, 223,
    107, 4,   6,   166,  216,  217, 218,  3,   6,   221, 4,   223, 3,   55,  4,    4,    195, 229,  3,    3,   1038, 53,  74,   68,  134,  3,   62,
    73,  74,  136, 137,  138,  139, 213,  141, 78,  143, 83,  145, 85,  147, 78,   149,  213, 214,  215,  216, 217,  218, 219,  220, 158,  4,   4,
    223, 101, 102, 103,  4,    4,   3,    107, 3,   4,   5,   6,   223, 6,   223,  223,  223, 6,    223,  180, 223,  182, 223,  184, 1120, 225, 223,
    78,  223, 46,  223,  46,   223, 4,    6,   77,  136, 137, 138, 139, 77,  141,  4,    143, 224,  145,  69,  147,  224, 149,  4,   55,   223, 3,
    3,   216, 217, 218,  158,  78,  221,  223, 223, 223, 223, 223, 4,   223, 229,  223,  4,   223,  4,    223, 77,   3,   224,  223, 73,   6,   180,
    230, 182, 221, 184,  6,    6,   5,    83,  42,  85,  223, 223, 223, 3,   3,    224,  166, 166,  4,    6,   76,   129, 227,  223, 3,    101, 102,
    103, 134, 3,   227,  107,  159, 224,  23,  216, 217, 218, 76,  4,   221, 30,   223,  32,  33,   156,  35,  223,  229, 131,  6,   40,   41,  6,
    227, 3,   3,   6,    4,    4,   136,  137, 138, 139, 230, 141, 230, 143, 4,    145,  227, 147,  179,  149, 31,   4,   6,    223, 223,  223, 223,
    223, 158, 223, 0,    73,   223, 223,  223, 223, 3,   7,   6,   9,   10,  11,   12,   13,  14,   15,   16,  17,   224, 4,    180, 4,    182, 23,
    184, 223, 26,  6,    28,   29,  30,   7,   78,  9,   10,  11,  12,  13,  14,   15,   16,  17,   227,  224, 6,    224, 224,  23,  5,    131, 26,
    46,  28,  29,  30,   6,    216, 217,  218, 6,   132, 221, 213, 223, 227, 129,  133,  223, 134,  229,  166, 73,   224, 221,  4,   224,  223, 227,
    78,  151, 152, 153,  154,  224, 156,  6,   6,   159, 57,  162, 163, 164, 165,  132,  167, 168,  57,   3,   171,  172, 173,  227, 78,   176, 52,
    178, 227, 224, 92,   182,  183, 227,  151, 152, 153, 154, 227, 227, 227, 227,  159,  227, 195,  196,  92,  227,  199, 200,  201, 227,  132, 134,
    205, 227, 92,  4,    209,  210, 211,  227, 227, 213, 214, 215, 216, 217, 218,  219,  220, 151,  152,  153, 154,  92,  92,   3,   134,  159, 139,
    140, 141, 142, 143,  144,  145, 146,  147, 148, 149, 150, 170, 227, 227, 227,  213,  214, 215,  216,  217, 218,  219, 220,  227, 227,  227, 227,
    186, 66,  90,  91,   92,   93,  94,   227, 170, 97,  98,  92,  198, 227, 227,  227,  104, 105,  106,  227, 227,  224, 208,  209, 186,  213, 214,
    215, 216, 217, 218,  219,  220, 227,  227, 227, 198, 223, 126, 6,   227, 53,   227,  223, 54,   227,  208, 209,  227, 227,  227, 227,  227, 227,
    227, 227, 6,   6,    6,    227, 6,    223, 89,  90,  91,  92,  93,  94,  95,   96,   97,  98,   99,   100, 101,  102, 103,  104, 105,  6,   107,
    108, 109, 110, 111,  112,  227, 227,  115, 227, 6,   118, 119, 120, 121, 122,  123,  124, 125,  90,   91,  92,   93,  94,   227, 227,  97,  98,
    227, 227, 227, 6,    227,  104, 105,  106, 90,  91,  92,  93,  94,  6,   227,  97,   98,  227,  227,  227, 227,  227, 104,  105, 106,  227, 227,
    126, 227, 227, 227,  90,   91,  92,   93,  94,  227, 227, 97,  98,  227, 6,    6,    227, 126,  104,  105, 106,  90,  91,   92,  93,   94,  6,
    6,   97,  98,  6,    6,    6,   6,    6,   104, 105, 106, 6,   6,   126, 6,    227,  6,   90,   91,   92,  93,   94,  6,    6,   97,   98,  6,
    6,   6,   6,   126,  104,  105, 106,  6,   6,   6,   227, 6,   6,   6,   6,    6,    6,   6,    6,    6,   6,    227, 6,    6,   6,    6,   126,
    6,   6,   6,   6,    6,    6,   227,  6,   6,   6,   6,   6,   6,   6,   6,    6,    6,   227,  6,    4,   4,    4,   4,    4,   224,  224, 6,
    62,  224, 224, 224,  224,  224, 224,  6,   224, 224, 224, 224, 224, 224, 6,    224,  224, 224,  224,  224, 224,  224, 224,  224, 224,  224, 224,
    224, 224, 224, 6,    6,    6,   6,    224, 166, 224, 224, 224, 224, 224, 224,  224,  224, 224,  224,  224, 4,    224, 224,  224, 224,  224, 224,
    4,   224, 224, 224,  224,  224, 224,  224, 224, 224, 224, 48,  224, 224, 224,  224,  6,   224,  6,    166, 223,  227, 227,  227, 224,  224, 224,
    224, 224, 224, 224,  224,  6,   4,    48,  224, 227, 227, 224, 4,   6,   224,  223,  6,   47,   224,  224, 39,   39,  4,    223, 3,    224, 39,
    39,  39,  39,  223,  223,  3,   224,  3,   224, 224, 65,  402, 754, 243, 752,  139,  665, 22,   365,  232, 938,  623, 805,  460, 204,  485, 943,
    426, 487, 904, -1,   782,  -1,  782,  770, -1,  278, 799, -1,  -1,  -1,  -1,   -1,   -1,  -1,   782,  -1,  564};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] = {
    0,   7,   9,   10,  11,  12,  13,  14,  15,  16,  17,  23,  26,  28,  29,  30,  78,  170, 186, 198, 208, 209, 223, 232, 233, 234, 236, 248, 249,
    251, 252, 255, 256, 257, 258, 259, 283, 288, 289, 290, 291, 292, 293, 294, 295, 32,  33,  34,  38,  39,  195, 36,  32,  33,  34,  38,  39,  195,
    3,   281, 76,  281, 167, 168, 172, 3,   235, 236, 248, 249, 252, 255, 256, 257, 288, 289, 290, 291, 292, 3,   23,  30,  32,  33,  35,  40,  41,
    162, 163, 164, 165, 167, 168, 171, 172, 173, 176, 178, 182, 183, 195, 196, 199, 200, 201, 205, 209, 210, 211, 3,   33,  33,  39,  160, 161, 162,
    178, 3,   281, 3,   284, 285, 173, 7,   12,  14,  16,  170, 190, 194, 197, 32,  33,  33,  51,  160, 258, 259, 0,   226, 344, 20,  22,  27,  277,
    8,   260, 262, 72,  343, 343, 343, 343, 343, 345, 3,   281, 72,  342, 342, 342, 342, 342, 3,   225, 14,  281, 76,  77,  223, 3,   3,   3,   235,
    223, 73,  73,  3,   281, 6,   207, 174, 175, 174, 175, 3,   4,   77,  6,   3,   202, 203, 204, 203, 206, 6,   73,  6,   73,  62,  281, 281, 3,
    55,  227, 6,   195, 195, 186, 187, 191, 161, 171, 174, 175, 176, 178, 192, 193, 195, 196, 195, 4,   193, 76,  195, 195, 281, 224, 224, 234, 21,
    223, 261, 262, 65,  269, 67,  263, 73,  3,   281, 281, 281, 3,   62,  62,  223, 250, 74,  3,   281, 281, 281, 3,   3,   3,   253, 254, 66,  274,
    4,   341, 341, 3,   4,   5,   6,   73,  74,  83,  85,  101, 102, 103, 107, 136, 137, 138, 139, 141, 143, 145, 147, 149, 158, 180, 182, 184, 216,
    217, 218, 221, 223, 229, 296, 298, 299, 300, 301, 302, 303, 304, 305, 306, 309, 310, 311, 312, 313, 315, 316, 317, 318, 319, 321, 322, 323, 324,
    325, 326, 327, 328, 329, 330, 333, 334, 335, 336, 337, 338, 3,   5,   6,   62,  169, 3,   5,   6,   62,  169, 3,   5,   6,   62,  169, 224, 131,
    131, 39,  42,  43,  47,  48,  3,   3,   341, 4,   131, 131, 281, 10,  24,  25,  62,  223, 285, 341, 4,   166, 6,   3,   6,   4,   3,   4,   55,
    4,   195, 3,   3,   261, 262, 296, 53,  68,  267, 74,  134, 55,  223, 250, 281, 32,  33,  51,  3,   247, 37,  259, 62,  213, 227, 274, 299, 78,
    78,  223, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 73,  74,  300, 223, 223, 88,  299, 314, 4,   4,   4,   4,   6,   338, 223,
    223, 223, 223, 223, 223, 223, 121, 123, 124, 127, 223, 223, 300, 300, 5,   6,   222, 319, 331, 332, 259, 299, 228, 318, 224, 227, 55,  156, 157,
    73,  75,  132, 151, 152, 153, 154, 155, 159, 213, 214, 215, 216, 217, 218, 219, 220, 225, 222, 227, 222, 227, 222, 227, 227, 228, 222, 227, 222,
    227, 3,   6,   78,  346, 46,  46,  77,  281, 260, 4,   39,  48,  6,   77,  188, 189, 4,   224, 224, 82,  270, 264, 265, 299, 299, 69,  268, 4,
    257, 3,   128, 130, 237, 239, 240, 246, 55,  223, 350, 3,   3,   224, 227, 223, 297, 281, 299, 254, 223, 223, 65,  224, 296, 223, 73,  259, 299,
    299, 314, 84,  86,  88,  299, 299, 299, 299, 299, 299, 4,   223, 223, 223, 223, 4,   4,   230, 224, 224, 77,  298, 3,   299, 299, 75,  159, 223,
    73,  131, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 3,   218, 319, 6,   331, 6,   332, 318, 6,   5,   42,  44,  45,
    223, 223, 223, 3,   224, 6,   32,  41,  4,   166, 166, 296, 76,  271, 227, 70,  71,  266, 299, 134, 89,  90,  91,  92,  93,  94,  95,  96,  97,
    98,  99,  100, 101, 102, 103, 104, 105, 107, 108, 109, 110, 111, 112, 115, 118, 119, 120, 121, 122, 123, 124, 125, 242, 129, 223, 224, 227, 257,
    3,   134, 3,   296, 227, 68,  69,  79,  80,  81,  185, 339, 340, 339, 296, 224, 259, 224, 55,  87,  84,  86,  299, 299, 224, 224, 224, 224, 224,
    224, 76,  299, 3,   317, 299, 4,   227, 273, 224, 227, 5,   6,   341, 223, 300, 259, 296, 131, 156, 230, 230, 6,   6,   3,   348, 349, 247, 238,
    240, 6,   4,   4,   223, 278, 279, 280, 281, 286, 179, 272, 265, 4,   223, 223, 223, 223, 223, 223, 223, 223, 223, 73,  128, 130, 131, 243, 244,
    346, 223, 247, 31,  347, 239, 224, 4,   224, 223, 6,   6,   4,   3,   6,   224, 227, 224, 224, 224, 242, 299, 299, 84,  87,  300, 227, 227, 227,
    227, 4,   66,  224, 4,   78,  259, 296, 224, 224, 300, 49,  50,  46,  213, 224, 227, 224, 224, 227, 6,   257, 227, 56,  58,  59,  60,  61,  63,
    64,  287, 3,   55,  282, 301, 302, 303, 304, 305, 306, 307, 308, 274, 6,   242, 241, 242, 90,  91,  92,  93,  94,  97,  98,  104, 105, 106, 126,
    90,  91,  92,  93,  94,  97,  98,  104, 105, 106, 126, 90,  91,  92,  93,  94,  97,  98,  104, 105, 106, 126, 90,  91,  92,  93,  94,  97,  98,
    104, 105, 106, 126, 90,  91,  92,  93,  94,  97,  98,  104, 105, 106, 126, 90,  91,  92,  93,  94,  97,  98,  104, 105, 106, 126, 131, 129, 133,
    244, 245, 245, 247, 224, 223, 134, 166, 296, 340, 224, 84,  299, 224, 299, 319, 221, 320, 323, 328, 333, 4,   227, 273, 299, 224, 223, 224, 224,
    6,   6,   3,   4,   5,   6,   349, 240, 33,  35,  224, 279, 57,  57,  3,   227, 52,  276, 224, 227, 224, 224, 227, 227, 227, 227, 227, 227, 227,
    227, 227, 227, 92,  227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 92,  227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 92,  227, 227, 227,
    227, 227, 227, 227, 227, 227, 227, 92,  227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 92,  227, 227, 227, 227, 227, 227, 227, 227, 227, 227,
    92,  227, 318, 134, 134, 224, 348, 4,   3,   224, 227, 227, 227, 227, 273, 224, 339, 6,   282, 280, 280, 223, 307, 53,  54,  275, 6,   242, 6,
    6,   6,   6,   6,   6,   6,   6,   6,   227, 6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   227, 6,   6,   6,   6,   6,   6,   6,   6,   6,
    6,   227, 6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   227, 6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   227, 6,   6,   6,   6,   6,
    6,   6,   6,   6,   6,   227, 6,   4,   4,   224, 346, 4,   4,   4,   4,   224, 224, 6,   62,  247, 296, 299, 224, 224, 224, 224, 224, 224, 224,
    224, 224, 224, 6,   224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 6,   224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 6,   224, 224, 224,
    224, 224, 224, 224, 224, 224, 224, 6,   224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 6,   224, 224, 224, 224, 224, 224, 224, 224, 224, 224,
    6,   224, 227, 227, 227, 273, 273, 166, 39,  42,  43,  47,  48,  299, 224, 224, 224, 224, 224, 224, 224, 4,   4,   6,   224, 224, 48,  6,   6,
    227, 273, 227, 273, 346, 166, 223, 6,   44,  45,  6,   224, 4,   224, 48,  4,   42,  43,  6,   273, 346, 273, 135, 166, 346, 223, 224, 6,   47,
    224, 224, 39,  39,  4,   135, 166, 346, 135, 166, 223, 224, 39,  39,  39,  39,  3,   223, 223, 224, 3,   3,   346, 224, 224, 346};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] = {
    0,   231, 232, 233, 233, 234, 234, 234, 234, 234, 234, 234, 234, 234, 234, 234, 234, 234, 234, 234, 234, 235, 235, 235, 235, 235, 235, 235, 235,
    235, 235, 235, 235, 236, 236, 236, 236, 236, 236, 236, 236, 236, 236, 237, 237, 238, 238, 239, 239, 240, 240, 240, 240, 241, 241, 242, 242, 242,
    242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242,
    242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242,
    242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242,
    242, 242, 242, 242, 243, 243, 244, 244, 244, 244, 245, 245, 246, 246, 247, 247, 248, 249, 249, 250, 250, 251, 251, 252, 253, 253, 254, 255, 255,
    255, 255, 255, 256, 256, 256, 257, 257, 257, 257, 258, 258, 259, 260, 261, 261, 262, 263, 263, 264, 264, 265, 266, 266, 266, 267, 267, 268, 268,
    269, 269, 270, 270, 271, 271, 272, 272, 273, 273, 274, 274, 275, 275, 276, 276, 277, 277, 277, 277, 278, 278, 279, 279, 280, 280, 281, 281, 282,
    282, 282, 282, 283, 283, 284, 284, 285, 286, 286, 287, 287, 287, 287, 287, 287, 287, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288,
    288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288,
    288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 288, 289, 289, 289, 289, 290, 291, 291, 291, 291, 291, 291, 291, 291, 291,
    291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 292, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293,
    293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 294, 294, 294, 294, 295,
    295, 296, 296, 297, 297, 298, 298, 299, 299, 299, 299, 299, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 301, 301, 301, 302,
    302, 302, 302, 303, 303, 303, 303, 304, 304, 304, 304, 305, 305, 306, 306, 307, 307, 307, 307, 307, 307, 308, 308, 309, 309, 309, 309, 309, 309,
    309, 309, 309, 309, 309, 309, 309, 309, 309, 309, 309, 309, 309, 309, 309, 309, 309, 309, 309, 309, 309, 309, 309, 310, 310, 311, 312, 312, 313,
    313, 313, 313, 314, 314, 315, 316, 316, 316, 316, 317, 317, 317, 317, 318, 318, 318, 318, 318, 318, 318, 318, 318, 318, 318, 318, 318, 319, 319,
    319, 319, 320, 320, 320, 321, 322, 322, 323, 323, 324, 325, 325, 326, 327, 327, 328, 329, 329, 330, 330, 331, 332, 333, 333, 334, 335, 335, 336,
    337, 337, 338, 338, 338, 338, 338, 338, 338, 338, 338, 338, 338, 338, 339, 339, 340, 340, 340, 340, 340, 340, 341, 342, 342, 343, 343, 344, 344,
    345, 345, 346, 346, 347, 347, 348, 348, 349, 349, 349, 349, 349, 350, 350};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] = {
    0,  2,  2,  1,  3,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  6, 4,  4,  8,  6,
    10, 8,  7,  6,  8,  1,  3,  1,  3, 1, 1, 4, 4, 6, 6, 1, 3, 1, 1, 1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1,  1,  1,  1,
    1,  1,  6,  4,  1,  6,  6,  6,  6, 6, 6, 6, 6, 6, 6, 7, 6, 6, 6, 6, 6, 6, 6,  6,  6,  6,  7,  6,  6,  6,  6,  6,  6,  6, 6,  6,  6,  7,
    6,  6,  6,  6,  6,  6,  6,  6,  6, 6, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  4, 4,  1,  2,  2,
    1,  1,  2,  2,  0,  5,  4,  1,  3, 4, 6, 5, 3, 0, 3, 2, 5, 1, 3, 3, 4, 4, 4,  4,  6,  8,  11, 8,  1,  1,  3,  3,  3,  3, 2,  4,  3,  3,
    9,  3,  0,  1,  3,  2,  1,  1,  0, 2, 0, 2, 0, 1, 0, 2, 0, 2, 0, 2, 0, 3, 0,  2,  0,  2,  0,  3,  0,  1,  2,  1,  1,  1, 3,  1,  1,  2,
    4,  1,  3,  2,  1,  5,  0,  2,  0, 1, 3, 5, 4, 6, 1, 1, 1, 1, 1, 1, 0, 2, 2,  2,  2,  3,  2,  2,  2,  2,  3,  2,  3,  3, 3,  3,  4,  4,
    3,  3,  4,  4,  5,  6,  7,  9,  4, 5, 7, 8, 9, 2, 2, 3, 4, 3, 3, 4, 2, 3, 3,  4,  2,  3,  2,  2,  2,  4,  2,  4,  3,  2, 4,  2,  2,  4,
    3,  2,  2,  2,  2,  2,  2,  4,  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  6,  6,  5,  3,  4,  4,  2,  5,  3,  6,  7, 9,  10, 12, 12,
    13, 14, 15, 16, 12, 13, 15, 16, 3, 4, 5, 6, 3, 3, 4, 3, 3, 4, 4, 6, 5, 3, 4,  3,  4,  3,  3,  5,  7,  7,  6,  8,  8,  5, 2,  3,  1,  3,
    3,  5,  3,  1,  1,  1,  1,  1,  1, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 19, 16, 20, 16, 15, 13, 18, 14, 13, 11, 8, 10, 13, 15, 5,
    7,  4,  6,  1,  1,  1,  1,  1,  1, 1, 3, 3, 4, 5, 4, 4, 4, 4, 4, 4, 4, 3, 2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3, 3,  3,  3,  6,
    3,  4,  3,  3,  5,  5,  6,  4,  6, 3, 5, 4, 5, 6, 4, 5, 5, 6, 1, 3, 1, 3, 1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  1,  1, 1,  1,  1,  1,
    1,  1,  1,  1,  2,  2,  3,  1,  1, 2, 2, 3, 2, 2, 3, 2, 2, 2, 2, 3, 3, 3, 1,  1,  2,  2,  3,  2,  2,  3,  2,  2,  2,  2, 2,  2,  2,  2,
    2,  2,  2,  2,  1,  3,  2,  2,  1, 2, 2, 2, 1, 2, 0, 3, 0, 1, 0, 2, 0, 4, 0,  4,  0,  1,  3,  1,  3,  3,  3,  3,  6,  3};

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
#line 330 "parser.y"
        {
            free(((*yyvaluep).str_value));
        }
#line 2498 "parser.cpp"
        break;

        case YYSYMBOL_STRING: /* STRING  */
#line 330 "parser.y"
        {
            free(((*yyvaluep).str_value));
        }
#line 2506 "parser.cpp"
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
#line 2520 "parser.cpp"
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
#line 2534 "parser.cpp"
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
#line 2548 "parser.cpp"
        break;

        case YYSYMBOL_column_type_array: /* column_type_array  */
#line 191 "parser.y"
        {
            fprintf(stderr, "destroy column_type_array\n");
            delete (((*yyvaluep).column_type_array_t));
        }
#line 2557 "parser.cpp"
        break;

        case YYSYMBOL_column_type: /* column_type  */
#line 186 "parser.y"
        {
            fprintf(stderr, "destroy column_type\n");
            delete (((*yyvaluep).column_type_t));
        }
#line 2566 "parser.cpp"
        break;

        case YYSYMBOL_column_constraints: /* column_constraints  */
#line 323 "parser.y"
        {
            fprintf(stderr, "destroy constraints\n");
            if ((((*yyvaluep).column_constraints_t)) != nullptr) {
                delete (((*yyvaluep).column_constraints_t));
            }
        }
#line 2577 "parser.cpp"
        break;

        case YYSYMBOL_default_expr: /* default_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 2585 "parser.cpp"
        break;

        case YYSYMBOL_identifier_array: /* identifier_array  */
#line 334 "parser.y"
        {
            fprintf(stderr, "destroy identifier array\n");
            delete (((*yyvaluep).identifier_array_t));
        }
#line 2594 "parser.cpp"
        break;

        case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 334 "parser.y"
        {
            fprintf(stderr, "destroy identifier array\n");
            delete (((*yyvaluep).identifier_array_t));
        }
#line 2603 "parser.cpp"
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
#line 2617 "parser.cpp"
        break;

        case YYSYMBOL_update_expr: /* update_expr  */
#line 287 "parser.y"
        {
            fprintf(stderr, "destroy update expr\n");
            if (((*yyvaluep).update_expr_t) != nullptr) {
                delete ((*yyvaluep).update_expr_t);
            }
        }
#line 2628 "parser.cpp"
        break;

        case YYSYMBOL_select_statement: /* select_statement  */
#line 369 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2638 "parser.cpp"
        break;

        case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 369 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2648 "parser.cpp"
        break;

        case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 369 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2658 "parser.cpp"
        break;

        case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 369 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2668 "parser.cpp"
        break;

        case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 369 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2678 "parser.cpp"
        break;

        case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 369 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2688 "parser.cpp"
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
#line 2702 "parser.cpp"
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
#line 2716 "parser.cpp"
        break;

        case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 357 "parser.y"
        {
            fprintf(stderr, "destroy order by expr\n");
            delete ((*yyvaluep).order_by_expr_t)->expr_;
            delete ((*yyvaluep).order_by_expr_t);
        }
#line 2726 "parser.cpp"
        break;

        case YYSYMBOL_limit_expr: /* limit_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2734 "parser.cpp"
        break;

        case YYSYMBOL_offset_expr: /* offset_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2742 "parser.cpp"
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
#line 2756 "parser.cpp"
        break;

        case YYSYMBOL_from_clause: /* from_clause  */
#line 352 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2765 "parser.cpp"
        break;

        case YYSYMBOL_search_clause: /* search_clause  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2773 "parser.cpp"
        break;

        case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2781 "parser.cpp"
        break;

        case YYSYMBOL_where_clause: /* where_clause  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2789 "parser.cpp"
        break;

        case YYSYMBOL_having_clause: /* having_clause  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2797 "parser.cpp"
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
#line 2811 "parser.cpp"
        break;

        case YYSYMBOL_table_reference: /* table_reference  */
#line 352 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2820 "parser.cpp"
        break;

        case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 352 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2829 "parser.cpp"
        break;

        case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 352 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2838 "parser.cpp"
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
#line 2851 "parser.cpp"
        break;

        case YYSYMBOL_table_alias: /* table_alias  */
#line 347 "parser.y"
        {
            fprintf(stderr, "destroy table alias\n");
            delete (((*yyvaluep).table_alias_t));
        }
#line 2860 "parser.cpp"
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
#line 2874 "parser.cpp"
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
#line 2888 "parser.cpp"
        break;

        case YYSYMBOL_with_expr: /* with_expr  */
#line 363 "parser.y"
        {
            fprintf(stderr, "destroy with expr\n");
            delete ((*yyvaluep).with_expr_t)->select_;
            delete ((*yyvaluep).with_expr_t);
        }
#line 2898 "parser.cpp"
        break;

        case YYSYMBOL_join_clause: /* join_clause  */
#line 352 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2907 "parser.cpp"
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
#line 2921 "parser.cpp"
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
#line 2935 "parser.cpp"
        break;

        case YYSYMBOL_expr_alias: /* expr_alias  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2943 "parser.cpp"
        break;

        case YYSYMBOL_expr: /* expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2951 "parser.cpp"
        break;

        case YYSYMBOL_operand: /* operand  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2959 "parser.cpp"
        break;

        case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2967 "parser.cpp"
        break;

        case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2975 "parser.cpp"
        break;

        case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2983 "parser.cpp"
        break;

        case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2991 "parser.cpp"
        break;

        case YYSYMBOL_query_expr: /* query_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2999 "parser.cpp"
        break;

        case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3007 "parser.cpp"
        break;

        case YYSYMBOL_sub_search: /* sub_search  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3015 "parser.cpp"
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
#line 3029 "parser.cpp"
        break;

        case YYSYMBOL_function_expr: /* function_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3037 "parser.cpp"
        break;

        case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3045 "parser.cpp"
        break;

        case YYSYMBOL_between_expr: /* between_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3053 "parser.cpp"
        break;

        case YYSYMBOL_in_expr: /* in_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3061 "parser.cpp"
        break;

        case YYSYMBOL_case_expr: /* case_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3069 "parser.cpp"
        break;

        case YYSYMBOL_case_check_array: /* case_check_array  */
#line 375 "parser.y"
        {
            fprintf(stderr, "destroy case check array\n");
            if (((*yyvaluep).case_check_array_t) != nullptr) {
                for (auto ptr : *(((*yyvaluep).case_check_array_t))) {
                    delete ptr;
                }
            }
        }
#line 3082 "parser.cpp"
        break;

        case YYSYMBOL_cast_expr: /* cast_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3090 "parser.cpp"
        break;

        case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3098 "parser.cpp"
        break;

        case YYSYMBOL_column_expr: /* column_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3106 "parser.cpp"
        break;

        case YYSYMBOL_constant_expr: /* constant_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3114 "parser.cpp"
        break;

        case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3122 "parser.cpp"
        break;

        case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3130 "parser.cpp"
        break;

        case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3138 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3146 "parser.cpp"
        break;

        case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3154 "parser.cpp"
        break;

        case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3162 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3170 "parser.cpp"
        break;

        case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3178 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3186 "parser.cpp"
        break;

        case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3194 "parser.cpp"
        break;

        case YYSYMBOL_curly_brackets_expr: /* curly_brackets_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3202 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_curly_brackets_expr: /* unclosed_curly_brackets_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3210 "parser.cpp"
        break;

        case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 384 "parser.y"
        {
            delete (((*yyvaluep).int_sparse_ele_t));
        }
#line 3218 "parser.cpp"
        break;

        case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 388 "parser.y"
        {
            delete (((*yyvaluep).float_sparse_ele_t));
        }
#line 3226 "parser.cpp"
        break;

        case YYSYMBOL_array_expr: /* array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3234 "parser.cpp"
        break;

        case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3242 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3250 "parser.cpp"
        break;

        case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3258 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3266 "parser.cpp"
        break;

        case YYSYMBOL_interval_expr: /* interval_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3274 "parser.cpp"
        break;

        case YYSYMBOL_file_path: /* file_path  */
#line 330 "parser.y"
        {
            free(((*yyvaluep).str_value));
        }
#line 3282 "parser.cpp"
        break;

        case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 220 "parser.y"
        {
            fprintf(stderr, "destroy if not exists info\n");
            if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
                delete (((*yyvaluep).if_not_exists_info_t));
            }
        }
#line 3293 "parser.cpp"
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
#line 3307 "parser.cpp"
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
#line 3321 "parser.cpp"
        break;

        case YYSYMBOL_index_info: /* index_info  */
#line 196 "parser.y"
        {
            fprintf(stderr, "destroy index info\n");
            if ((((*yyvaluep).index_info_t)) != nullptr) {
                delete (((*yyvaluep).index_info_t));
            }
        }
#line 3332 "parser.cpp"
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

#line 3440 "parser.cpp"

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
#line 520 "parser.y"
        {
            result->statements_ptr_ = (yyvsp[-1].stmt_array);
        }
#line 3655 "parser.cpp"
        break;

        case 3: /* statement_list: statement  */
#line 524 "parser.y"
        {
            (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
            yylloc.string_length = 0;
            (yyval.stmt_array) = new std::vector<infinity::BaseStatement *>();
            (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
        }
#line 3666 "parser.cpp"
        break;

        case 4: /* statement_list: statement_list ';' statement  */
#line 530 "parser.y"
        {
            (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
            yylloc.string_length = 0;
            (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
            (yyval.stmt_array) = (yyvsp[-2].stmt_array);
        }
#line 3677 "parser.cpp"
        break;

        case 5: /* statement: create_statement  */
#line 537 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].create_stmt);
        }
#line 3683 "parser.cpp"
        break;

        case 6: /* statement: drop_statement  */
#line 538 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].drop_stmt);
        }
#line 3689 "parser.cpp"
        break;

        case 7: /* statement: copy_statement  */
#line 539 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].copy_stmt);
        }
#line 3695 "parser.cpp"
        break;

        case 8: /* statement: show_statement  */
#line 540 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].show_stmt);
        }
#line 3701 "parser.cpp"
        break;

        case 9: /* statement: select_statement  */
#line 541 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].select_stmt);
        }
#line 3707 "parser.cpp"
        break;

        case 10: /* statement: delete_statement  */
#line 542 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].delete_stmt);
        }
#line 3713 "parser.cpp"
        break;

        case 11: /* statement: update_statement  */
#line 543 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].update_stmt);
        }
#line 3719 "parser.cpp"
        break;

        case 12: /* statement: insert_statement  */
#line 544 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].insert_stmt);
        }
#line 3725 "parser.cpp"
        break;

        case 13: /* statement: explain_statement  */
#line 545 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].explain_stmt);
        }
#line 3731 "parser.cpp"
        break;

        case 14: /* statement: flush_statement  */
#line 546 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].flush_stmt);
        }
#line 3737 "parser.cpp"
        break;

        case 15: /* statement: optimize_statement  */
#line 547 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].optimize_stmt);
        }
#line 3743 "parser.cpp"
        break;

        case 16: /* statement: command_statement  */
#line 548 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].command_stmt);
        }
#line 3749 "parser.cpp"
        break;

        case 17: /* statement: compact_statement  */
#line 549 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].compact_stmt);
        }
#line 3755 "parser.cpp"
        break;

        case 18: /* statement: admin_statement  */
#line 550 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].admin_stmt);
        }
#line 3761 "parser.cpp"
        break;

        case 19: /* statement: alter_statement  */
#line 551 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].alter_stmt);
        }
#line 3767 "parser.cpp"
        break;

        case 20: /* statement: check_statement  */
#line 552 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].check_stmt);
        }
#line 3773 "parser.cpp"
        break;

        case 21: /* explainable_statement: create_statement  */
#line 554 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].create_stmt);
        }
#line 3779 "parser.cpp"
        break;

        case 22: /* explainable_statement: drop_statement  */
#line 555 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].drop_stmt);
        }
#line 3785 "parser.cpp"
        break;

        case 23: /* explainable_statement: copy_statement  */
#line 556 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].copy_stmt);
        }
#line 3791 "parser.cpp"
        break;

        case 24: /* explainable_statement: show_statement  */
#line 557 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].show_stmt);
        }
#line 3797 "parser.cpp"
        break;

        case 25: /* explainable_statement: select_statement  */
#line 558 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].select_stmt);
        }
#line 3803 "parser.cpp"
        break;

        case 26: /* explainable_statement: delete_statement  */
#line 559 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].delete_stmt);
        }
#line 3809 "parser.cpp"
        break;

        case 27: /* explainable_statement: update_statement  */
#line 560 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].update_stmt);
        }
#line 3815 "parser.cpp"
        break;

        case 28: /* explainable_statement: insert_statement  */
#line 561 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].insert_stmt);
        }
#line 3821 "parser.cpp"
        break;

        case 29: /* explainable_statement: flush_statement  */
#line 562 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].flush_stmt);
        }
#line 3827 "parser.cpp"
        break;

        case 30: /* explainable_statement: optimize_statement  */
#line 563 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].optimize_stmt);
        }
#line 3833 "parser.cpp"
        break;

        case 31: /* explainable_statement: command_statement  */
#line 564 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].command_stmt);
        }
#line 3839 "parser.cpp"
        break;

        case 32: /* explainable_statement: compact_statement  */
#line 565 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].compact_stmt);
        }
#line 3845 "parser.cpp"
        break;

        case 33: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER COMMENT STRING  */
#line 572 "parser.y"
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
#line 3867 "parser.cpp"
        break;

        case 34: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 589 "parser.y"
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
#line 3887 "parser.cpp"
        break;

        case 35: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 606 "parser.y"
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
#line 3905 "parser.cpp"
        break;

        case 36: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 622 "parser.y"
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
#line 3938 "parser.cpp"
        break;

        case 37: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 651 "parser.y"
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
#line 3958 "parser.cpp"
        break;

        case 38: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list COMMENT STRING
                  */
#line 666 "parser.y"
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
#line 3994 "parser.cpp"
        break;

        case 39: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement COMMENT STRING  */
#line 698 "parser.y"
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
#line 4016 "parser.cpp"
        break;

        case 40: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 716 "parser.y"
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
#line 4037 "parser.cpp"
        break;

        case 41: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info  */
#line 734 "parser.y"
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
#line 4070 "parser.cpp"
        break;

        case 42: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info COMMENT STRING  */
#line 762 "parser.y"
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
#line 4105 "parser.cpp"
        break;

        case 43: /* table_element_array: table_element  */
#line 793 "parser.y"
        {
            (yyval.table_element_array_t) = new std::vector<infinity::TableElement *>();
            (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
        }
#line 4114 "parser.cpp"
        break;

        case 44: /* table_element_array: table_element_array ',' table_element  */
#line 797 "parser.y"
        {
            (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
            (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
        }
#line 4123 "parser.cpp"
        break;

        case 45: /* column_def_array: table_column  */
#line 802 "parser.y"
        {
            (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef *>();
            (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
        }
#line 4132 "parser.cpp"
        break;

        case 46: /* column_def_array: column_def_array ',' table_column  */
#line 806 "parser.y"
        {
            (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
            (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
        }
#line 4141 "parser.cpp"
        break;

        case 47: /* table_element: table_column  */
#line 812 "parser.y"
        {
            (yyval.table_element_t) = (yyvsp[0].table_column_t);
        }
#line 4149 "parser.cpp"
        break;

        case 48: /* table_element: table_constraint  */
#line 815 "parser.y"
        {
            (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
        }
#line 4157 "parser.cpp"
        break;

        case 49: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 822 "parser.y"
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
#line 4182 "parser.cpp"
        break;

        case 50: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 842 "parser.y"
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
#line 4209 "parser.cpp"
        break;

        case 51: /* table_column: IDENTIFIER column_type with_index_param_list default_expr COMMENT STRING  */
#line 864 "parser.y"
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
#line 4238 "parser.cpp"
        break;

        case 52: /* table_column: IDENTIFIER column_type column_constraints default_expr COMMENT STRING  */
#line 888 "parser.y"
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
#line 4268 "parser.cpp"
        break;

        case 53: /* column_type_array: column_type  */
#line 914 "parser.y"
        {
            (yyval.column_type_array_t) = new std::vector<std::unique_ptr<infinity::ColumnType>>();
            (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
        }
#line 4277 "parser.cpp"
        break;

        case 54: /* column_type_array: column_type_array ',' column_type  */
#line 918 "parser.y"
        {
            (yyval.column_type_array_t) = (yyvsp[-2].column_type_array_t);
            (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
        }
#line 4286 "parser.cpp"
        break;

        case 55: /* column_type: BOOLEAN  */
#line 924 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4292 "parser.cpp"
        break;

        case 56: /* column_type: TINYINT  */
#line 925 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4298 "parser.cpp"
        break;

        case 57: /* column_type: SMALLINT  */
#line 926 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4304 "parser.cpp"
        break;

        case 58: /* column_type: INTEGER  */
#line 927 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4310 "parser.cpp"
        break;

        case 59: /* column_type: INT  */
#line 928 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4316 "parser.cpp"
        break;

        case 60: /* column_type: BIGINT  */
#line 929 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4322 "parser.cpp"
        break;

        case 61: /* column_type: HUGEINT  */
#line 930 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4328 "parser.cpp"
        break;

        case 62: /* column_type: FLOAT  */
#line 931 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4334 "parser.cpp"
        break;

        case 63: /* column_type: REAL  */
#line 932 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4340 "parser.cpp"
        break;

        case 64: /* column_type: DOUBLE  */
#line 933 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4346 "parser.cpp"
        break;

        case 65: /* column_type: FLOAT16  */
#line 934 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4352 "parser.cpp"
        break;

        case 66: /* column_type: BFLOAT16  */
#line 935 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4358 "parser.cpp"
        break;

        case 67: /* column_type: DATE  */
#line 936 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4364 "parser.cpp"
        break;

        case 68: /* column_type: TIME  */
#line 937 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4370 "parser.cpp"
        break;

        case 69: /* column_type: DATETIME  */
#line 938 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4376 "parser.cpp"
        break;

        case 70: /* column_type: TIMESTAMP  */
#line 939 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4382 "parser.cpp"
        break;

        case 71: /* column_type: UUID  */
#line 940 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4388 "parser.cpp"
        break;

        case 72: /* column_type: POINT  */
#line 941 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4394 "parser.cpp"
        break;

        case 73: /* column_type: LINE  */
#line 942 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4400 "parser.cpp"
        break;

        case 74: /* column_type: LSEG  */
#line 943 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4406 "parser.cpp"
        break;

        case 75: /* column_type: BOX  */
#line 944 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4412 "parser.cpp"
        break;

        case 76: /* column_type: CIRCLE  */
#line 947 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4418 "parser.cpp"
        break;

        case 77: /* column_type: VARCHAR  */
#line 949 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4424 "parser.cpp"
        break;

        case 78: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 950 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal,
                                                             0,
                                                             (yyvsp[-3].long_value),
                                                             (yyvsp[-1].long_value),
                                                             infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4430 "parser.cpp"
        break;

        case 79: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 951 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4436 "parser.cpp"
        break;

        case 80: /* column_type: DECIMAL  */
#line 952 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4442 "parser.cpp"
        break;

        case 81: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 955 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4448 "parser.cpp"
        break;

        case 82: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 956 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4454 "parser.cpp"
        break;

        case 83: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 957 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4460 "parser.cpp"
        break;

        case 84: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 958 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4466 "parser.cpp"
        break;

        case 85: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 959 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4472 "parser.cpp"
        break;

        case 86: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 960 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4478 "parser.cpp"
        break;

        case 87: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 961 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4484 "parser.cpp"
        break;

        case 88: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 962 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4490 "parser.cpp"
        break;

        case 89: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 963 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4496 "parser.cpp"
        break;

        case 90: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 964 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4502 "parser.cpp"
        break;

        case 91: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 965 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4508 "parser.cpp"
        break;

        case 92: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 966 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4514 "parser.cpp"
        break;

        case 93: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 967 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4520 "parser.cpp"
        break;

        case 94: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 968 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4526 "parser.cpp"
        break;

        case 95: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 969 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4532 "parser.cpp"
        break;

        case 96: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 970 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4538 "parser.cpp"
        break;

        case 97: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 971 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4544 "parser.cpp"
        break;

        case 98: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 972 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4550 "parser.cpp"
        break;

        case 99: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 973 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4556 "parser.cpp"
        break;

        case 100: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 974 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector,
                                                             (yyvsp[-1].long_value),
                                                             0,
                                                             0,
                                                             infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4562 "parser.cpp"
        break;

        case 101: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 975 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector,
                                                             (yyvsp[-1].long_value),
                                                             0,
                                                             0,
                                                             infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4568 "parser.cpp"
        break;

        case 102: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 976 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4574 "parser.cpp"
        break;

        case 103: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 977 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4580 "parser.cpp"
        break;

        case 104: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 978 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4586 "parser.cpp"
        break;

        case 105: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 979 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4592 "parser.cpp"
        break;

        case 106: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 980 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4598 "parser.cpp"
        break;

        case 107: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 981 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4604 "parser.cpp"
        break;

        case 108: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 982 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4610 "parser.cpp"
        break;

        case 109: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 983 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4616 "parser.cpp"
        break;

        case 110: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 984 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4622 "parser.cpp"
        break;

        case 111: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 985 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4628 "parser.cpp"
        break;

        case 112: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 986 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4634 "parser.cpp"
        break;

        case 113: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 987 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4640 "parser.cpp"
        break;

        case 114: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 988 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4646 "parser.cpp"
        break;

        case 115: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 989 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4652 "parser.cpp"
        break;

        case 116: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 990 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4658 "parser.cpp"
        break;

        case 117: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 991 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4664 "parser.cpp"
        break;

        case 118: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 992 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4670 "parser.cpp"
        break;

        case 119: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 993 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4676 "parser.cpp"
        break;

        case 120: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 994 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4682 "parser.cpp"
        break;

        case 121: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 995 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4688 "parser.cpp"
        break;

        case 122: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 996 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray,
                                                             (yyvsp[-1].long_value),
                                                             0,
                                                             0,
                                                             infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4694 "parser.cpp"
        break;

        case 123: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 997 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray,
                                                             (yyvsp[-1].long_value),
                                                             0,
                                                             0,
                                                             infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4700 "parser.cpp"
        break;

        case 124: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 998 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4706 "parser.cpp"
        break;

        case 125: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 999 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4712 "parser.cpp"
        break;

        case 126: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1000 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4718 "parser.cpp"
        break;

        case 127: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1001 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4724 "parser.cpp"
        break;

        case 128: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1002 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4730 "parser.cpp"
        break;

        case 129: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1003 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4736 "parser.cpp"
        break;

        case 130: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1004 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4742 "parser.cpp"
        break;

        case 131: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1005 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4748 "parser.cpp"
        break;

        case 132: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1006 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4754 "parser.cpp"
        break;

        case 133: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1007 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4760 "parser.cpp"
        break;

        case 134: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1008 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4766 "parser.cpp"
        break;

        case 135: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1009 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4772 "parser.cpp"
        break;

        case 136: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4778 "parser.cpp"
        break;

        case 137: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1011 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4784 "parser.cpp"
        break;

        case 138: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1012 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4790 "parser.cpp"
        break;

        case 139: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1013 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4796 "parser.cpp"
        break;

        case 140: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1014 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4802 "parser.cpp"
        break;

        case 141: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4808 "parser.cpp"
        break;

        case 142: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4814 "parser.cpp"
        break;

        case 143: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1017 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4820 "parser.cpp"
        break;

        case 144: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1018 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4826 "parser.cpp"
        break;

        case 145: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1019 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4832 "parser.cpp"
        break;

        case 146: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1020 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4838 "parser.cpp"
        break;

        case 147: /* column_type: ARRAY '(' column_type ')'  */
#line 1021 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kArray, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
            (yyval.column_type_t)->element_types_.emplace_back((yyvsp[-1].column_type_t));
        }
#line 4847 "parser.cpp"
        break;

        case 148: /* column_type: TUPLE '(' column_type_array ')'  */
#line 1025 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTuple, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
            (yyval.column_type_t)->element_types_ = std::move(*((yyvsp[-1].column_type_array_t)));
            delete (yyvsp[-1].column_type_array_t);
        }
#line 4857 "parser.cpp"
        break;

        case 149: /* column_constraints: column_constraint  */
#line 1048 "parser.y"
        {
            (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
            (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
        }
#line 4866 "parser.cpp"
        break;

        case 150: /* column_constraints: column_constraints column_constraint  */
#line 1052 "parser.y"
        {
            if ((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
                yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
                delete (yyvsp[-1].column_constraints_t);
                YYERROR;
            }
            (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
            (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
        }
#line 4880 "parser.cpp"
        break;

        case 151: /* column_constraint: PRIMARY KEY  */
#line 1062 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
        }
#line 4888 "parser.cpp"
        break;

        case 152: /* column_constraint: UNIQUE  */
#line 1065 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
        }
#line 4896 "parser.cpp"
        break;

        case 153: /* column_constraint: NULLABLE  */
#line 1068 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
        }
#line 4904 "parser.cpp"
        break;

        case 154: /* column_constraint: NOT NULLABLE  */
#line 1071 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
        }
#line 4912 "parser.cpp"
        break;

        case 155: /* default_expr: DEFAULT constant_expr  */
#line 1075 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 4920 "parser.cpp"
        break;

        case 156: /* default_expr: %empty  */
#line 1078 "parser.y"
        {
            (yyval.const_expr_t) = nullptr;
        }
#line 4928 "parser.cpp"
        break;

        case 157: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1083 "parser.y"
        {
            (yyval.table_constraint_t) = new infinity::TableConstraint();
            (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
            (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
        }
#line 4938 "parser.cpp"
        break;

        case 158: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1088 "parser.y"
        {
            (yyval.table_constraint_t) = new infinity::TableConstraint();
            (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
            (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
        }
#line 4948 "parser.cpp"
        break;

        case 159: /* identifier_array: IDENTIFIER  */
#line 1095 "parser.y"
        {
            (yyval.identifier_array_t) = new std::vector<std::string>();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 4959 "parser.cpp"
        break;

        case 160: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1101 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
            (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
        }
#line 4970 "parser.cpp"
        break;

        case 161: /* delete_statement: DELETE FROM table_name where_clause  */
#line 1111 "parser.y"
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
#line 4987 "parser.cpp"
        break;

        case 162: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES insert_row_list  */
#line 1127 "parser.y"
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
#line 5030 "parser.cpp"
        break;

        case 163: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1165 "parser.y"
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
#line 5050 "parser.cpp"
        break;

        case 164: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1181 "parser.y"
        {
            (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
        }
#line 5058 "parser.cpp"
        break;

        case 165: /* optional_identifier_array: %empty  */
#line 1184 "parser.y"
        {
            (yyval.identifier_array_t) = nullptr;
        }
#line 5066 "parser.cpp"
        break;

        case 166: /* explain_statement: EXPLAIN IDENTIFIER explainable_statement  */
#line 1191 "parser.y"
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
#line 5089 "parser.cpp"
        break;

        case 167: /* explain_statement: EXPLAIN explainable_statement  */
#line 1208 "parser.y"
        {
            (yyval.explain_stmt) = new infinity::ExplainStatement();
            (yyval.explain_stmt)->type_ = infinity::ExplainType::kPhysical;
            (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
        }
#line 5099 "parser.cpp"
        break;

        case 168: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1217 "parser.y"
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
#line 5116 "parser.cpp"
        break;

        case 169: /* update_expr_array: update_expr  */
#line 1230 "parser.y"
        {
            (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr *>();
            (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
        }
#line 5125 "parser.cpp"
        break;

        case 170: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1234 "parser.y"
        {
            (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
            (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
        }
#line 5134 "parser.cpp"
        break;

        case 171: /* update_expr: IDENTIFIER '=' expr  */
#line 1239 "parser.y"
        {
            (yyval.update_expr_t) = new infinity::UpdateExpr();
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));
            (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
        }
#line 5146 "parser.cpp"
        break;

        case 172: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1252 "parser.y"
        {
            (yyval.drop_stmt) = new infinity::DropStatement();
            std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

            ParserHelper::ToLower((yyvsp[0].str_value));
            drop_schema_info->schema_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));

            (yyval.drop_stmt)->drop_info_ = drop_schema_info;
            (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
        }
#line 5162 "parser.cpp"
        break;

        case 173: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1265 "parser.y"
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
#line 5180 "parser.cpp"
        break;

        case 174: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1280 "parser.y"
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
#line 5198 "parser.cpp"
        break;

        case 175: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1295 "parser.y"
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
#line 5216 "parser.cpp"
        break;

        case 176: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1310 "parser.y"
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
#line 5239 "parser.cpp"
        break;

        case 177: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1333 "parser.y"
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
#line 5297 "parser.cpp"
        break;

        case 178: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1386 "parser.y"
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
#line 5357 "parser.cpp"
        break;

        case 179: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1441 "parser.y"
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
#line 5409 "parser.cpp"
        break;

        case 180: /* select_statement: select_without_paren  */
#line 1492 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[0].select_stmt);
        }
#line 5417 "parser.cpp"
        break;

        case 181: /* select_statement: select_with_paren  */
#line 1495 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[0].select_stmt);
        }
#line 5425 "parser.cpp"
        break;

        case 182: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1498 "parser.y"
        {
            infinity::SelectStatement *node = (yyvsp[-2].select_stmt);
            while (node->nested_select_ != nullptr) {
                node = node->nested_select_;
            }
            node->set_op_ = (yyvsp[-1].set_operator_t);
            node->nested_select_ = (yyvsp[0].select_stmt);
            (yyval.select_stmt) = (yyvsp[-2].select_stmt);
        }
#line 5439 "parser.cpp"
        break;

        case 183: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1507 "parser.y"
        {
            infinity::SelectStatement *node = (yyvsp[-2].select_stmt);
            while (node->nested_select_ != nullptr) {
                node = node->nested_select_;
            }
            node->set_op_ = (yyvsp[-1].set_operator_t);
            node->nested_select_ = (yyvsp[0].select_stmt);
            (yyval.select_stmt) = (yyvsp[-2].select_stmt);
        }
#line 5453 "parser.cpp"
        break;

        case 184: /* select_with_paren: '(' select_without_paren ')'  */
#line 1517 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5461 "parser.cpp"
        break;

        case 185: /* select_with_paren: '(' select_with_paren ')'  */
#line 1520 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5469 "parser.cpp"
        break;

        case 186: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1524 "parser.y"
        {
            (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
            (yyval.select_stmt) = (yyvsp[0].select_stmt);
        }
#line 5478 "parser.cpp"
        break;

        case 187: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1529 "parser.y"
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
#line 5509 "parser.cpp"
        break;

        case 188: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1556 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5517 "parser.cpp"
        break;

        case 189: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1559 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5525 "parser.cpp"
        break;

        case 190: /* select_clause_without_modifier: SELECT distinct expr_array highlight_clause from_clause search_clause where_clause
                     group_by_clause having_clause  */
#line 1564 "parser.y"
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
#line 5546 "parser.cpp"
        break;

        case 191: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1581 "parser.y"
        {
            (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
        }
#line 5554 "parser.cpp"
        break;

        case 192: /* order_by_clause: %empty  */
#line 1584 "parser.y"
        {
            (yyval.order_by_expr_list_t) = nullptr;
        }
#line 5562 "parser.cpp"
        break;

        case 193: /* order_by_expr_list: order_by_expr  */
#line 1588 "parser.y"
        {
            (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr *>();
            (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
        }
#line 5571 "parser.cpp"
        break;

        case 194: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1592 "parser.y"
        {
            (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
            (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
        }
#line 5580 "parser.cpp"
        break;

        case 195: /* order_by_expr: expr order_by_type  */
#line 1597 "parser.y"
        {
            (yyval.order_by_expr_t) = new infinity::OrderByExpr();
            (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
            (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
        }
#line 5590 "parser.cpp"
        break;

        case 196: /* order_by_type: ASC  */
#line 1603 "parser.y"
        {
            (yyval.order_by_type_t) = infinity::kAsc;
        }
#line 5598 "parser.cpp"
        break;

        case 197: /* order_by_type: DESC  */
#line 1606 "parser.y"
        {
            (yyval.order_by_type_t) = infinity::kDesc;
        }
#line 5606 "parser.cpp"
        break;

        case 198: /* order_by_type: %empty  */
#line 1609 "parser.y"
        {
            (yyval.order_by_type_t) = infinity::kAsc;
        }
#line 5614 "parser.cpp"
        break;

        case 199: /* limit_expr: LIMIT expr  */
#line 1613 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5622 "parser.cpp"
        break;

        case 200: /* limit_expr: %empty  */
#line 1617 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5628 "parser.cpp"
        break;

        case 201: /* offset_expr: OFFSET expr  */
#line 1619 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5636 "parser.cpp"
        break;

        case 202: /* offset_expr: %empty  */
#line 1623 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5642 "parser.cpp"
        break;

        case 203: /* distinct: DISTINCT  */
#line 1625 "parser.y"
        {
            (yyval.bool_value) = true;
        }
#line 5650 "parser.cpp"
        break;

        case 204: /* distinct: %empty  */
#line 1628 "parser.y"
        {
            (yyval.bool_value) = false;
        }
#line 5658 "parser.cpp"
        break;

        case 205: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1632 "parser.y"
        {
            (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
        }
#line 5666 "parser.cpp"
        break;

        case 206: /* highlight_clause: %empty  */
#line 1635 "parser.y"
        {
            (yyval.expr_array_t) = nullptr;
        }
#line 5674 "parser.cpp"
        break;

        case 207: /* from_clause: FROM table_reference  */
#line 1639 "parser.y"
        {
            (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
        }
#line 5682 "parser.cpp"
        break;

        case 208: /* from_clause: %empty  */
#line 1642 "parser.y"
        {
            (yyval.table_reference_t) = nullptr;
        }
#line 5690 "parser.cpp"
        break;

        case 209: /* search_clause: SEARCH sub_search_array  */
#line 1646 "parser.y"
        {
            infinity::SearchExpr *search_expr = new infinity::SearchExpr();
            search_expr->SetExprs((yyvsp[0].expr_array_t));
            (yyval.expr_t) = search_expr;
        }
#line 5700 "parser.cpp"
        break;

        case 210: /* search_clause: %empty  */
#line 1651 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5708 "parser.cpp"
        break;

        case 211: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1655 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5716 "parser.cpp"
        break;

        case 212: /* optional_search_filter_expr: %empty  */
#line 1658 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5724 "parser.cpp"
        break;

        case 213: /* where_clause: WHERE expr  */
#line 1662 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5732 "parser.cpp"
        break;

        case 214: /* where_clause: %empty  */
#line 1665 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5740 "parser.cpp"
        break;

        case 215: /* having_clause: HAVING expr  */
#line 1669 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5748 "parser.cpp"
        break;

        case 216: /* having_clause: %empty  */
#line 1672 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5756 "parser.cpp"
        break;

        case 217: /* group_by_clause: GROUP BY expr_array  */
#line 1676 "parser.y"
        {
            (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
        }
#line 5764 "parser.cpp"
        break;

        case 218: /* group_by_clause: %empty  */
#line 1679 "parser.y"
        {
            (yyval.expr_array_t) = nullptr;
        }
#line 5772 "parser.cpp"
        break;

        case 219: /* set_operator: UNION  */
#line 1683 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
        }
#line 5780 "parser.cpp"
        break;

        case 220: /* set_operator: UNION ALL  */
#line 1686 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
        }
#line 5788 "parser.cpp"
        break;

        case 221: /* set_operator: INTERSECT  */
#line 1689 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
        }
#line 5796 "parser.cpp"
        break;

        case 222: /* set_operator: EXCEPT  */
#line 1692 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
        }
#line 5804 "parser.cpp"
        break;

        case 223: /* table_reference: table_reference_unit  */
#line 1700 "parser.y"
        {
            (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
        }
#line 5812 "parser.cpp"
        break;

        case 224: /* table_reference: table_reference ',' table_reference_unit  */
#line 1703 "parser.y"
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
#line 5830 "parser.cpp"
        break;

        case 227: /* table_reference_name: table_name table_alias  */
#line 1720 "parser.y"
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
#line 5848 "parser.cpp"
        break;

        case 228: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1734 "parser.y"
        {
            infinity::SubqueryReference *subquery_reference = new infinity::SubqueryReference();
            subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
            subquery_reference->alias_ = (yyvsp[0].table_alias_t);
            (yyval.table_reference_t) = subquery_reference;
        }
#line 5859 "parser.cpp"
        break;

        case 229: /* table_name: IDENTIFIER  */
#line 1743 "parser.y"
        {
            (yyval.table_name_t) = new infinity::TableName();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
        }
#line 5869 "parser.cpp"
        break;

        case 230: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1748 "parser.y"
        {
            (yyval.table_name_t) = new infinity::TableName();
            ParserHelper::ToLower((yyvsp[-2].str_value));
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
            (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
        }
#line 5881 "parser.cpp"
        break;

        case 231: /* table_alias: AS IDENTIFIER  */
#line 1757 "parser.y"
        {
            (yyval.table_alias_t) = new infinity::TableAlias();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
        }
#line 5891 "parser.cpp"
        break;

        case 232: /* table_alias: IDENTIFIER  */
#line 1762 "parser.y"
        {
            (yyval.table_alias_t) = new infinity::TableAlias();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
        }
#line 5901 "parser.cpp"
        break;

        case 233: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1767 "parser.y"
        {
            (yyval.table_alias_t) = new infinity::TableAlias();
            ParserHelper::ToLower((yyvsp[-3].str_value));
            (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
            (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
        }
#line 5912 "parser.cpp"
        break;

        case 234: /* table_alias: %empty  */
#line 1773 "parser.y"
        {
            (yyval.table_alias_t) = nullptr;
        }
#line 5920 "parser.cpp"
        break;

        case 235: /* with_clause: WITH with_expr_list  */
#line 1780 "parser.y"
        {
            (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
        }
#line 5928 "parser.cpp"
        break;

        case 236: /* with_clause: %empty  */
#line 1783 "parser.y"
        {
            (yyval.with_expr_list_t) = nullptr;
        }
#line 5936 "parser.cpp"
        break;

        case 237: /* with_expr_list: with_expr  */
#line 1787 "parser.y"
        {
            (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr *>();
            (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
        }
#line 5945 "parser.cpp"
        break;

        case 238: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1790 "parser.y"
        {
            (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
            (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
        }
#line 5954 "parser.cpp"
        break;

        case 239: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1795 "parser.y"
        {
            (yyval.with_expr_t) = new infinity::WithExpr();
            ParserHelper::ToLower((yyvsp[-4].str_value));
            (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
            free((yyvsp[-4].str_value));
            (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
        }
#line 5966 "parser.cpp"
        break;

        case 240: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1807 "parser.y"
        {
            infinity::JoinReference *join_reference = new infinity::JoinReference();
            join_reference->left_ = (yyvsp[-3].table_reference_t);
            join_reference->right_ = (yyvsp[0].table_reference_t);
            join_reference->join_type_ = infinity::JoinType::kNatural;
            (yyval.table_reference_t) = join_reference;
        }
#line 5978 "parser.cpp"
        break;

        case 241: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1814 "parser.y"
        {
            infinity::JoinReference *join_reference = new infinity::JoinReference();
            join_reference->left_ = (yyvsp[-5].table_reference_t);
            join_reference->right_ = (yyvsp[-2].table_reference_t);
            join_reference->join_type_ = (yyvsp[-4].join_type_t);
            join_reference->condition_ = (yyvsp[0].expr_t);
            (yyval.table_reference_t) = join_reference;
        }
#line 5991 "parser.cpp"
        break;

        case 242: /* join_type: INNER  */
#line 1828 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kInner;
        }
#line 5999 "parser.cpp"
        break;

        case 243: /* join_type: LEFT  */
#line 1831 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kLeft;
        }
#line 6007 "parser.cpp"
        break;

        case 244: /* join_type: RIGHT  */
#line 1834 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kRight;
        }
#line 6015 "parser.cpp"
        break;

        case 245: /* join_type: OUTER  */
#line 1837 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kFull;
        }
#line 6023 "parser.cpp"
        break;

        case 246: /* join_type: FULL  */
#line 1840 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kFull;
        }
#line 6031 "parser.cpp"
        break;

        case 247: /* join_type: CROSS  */
#line 1843 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kCross;
        }
#line 6039 "parser.cpp"
        break;

        case 248: /* join_type: %empty  */
#line 1846 "parser.y"
        {
        }
#line 6046 "parser.cpp"
        break;

        case 249: /* show_statement: SHOW DATABASES  */
#line 1852 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
        }
#line 6055 "parser.cpp"
        break;

        case 250: /* show_statement: SHOW TABLES  */
#line 1856 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
        }
#line 6064 "parser.cpp"
        break;

        case 251: /* show_statement: SHOW TASKS  */
#line 1860 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTasks;
        }
#line 6073 "parser.cpp"
        break;

        case 252: /* show_statement: SHOW CONFIGS  */
#line 1864 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
        }
#line 6082 "parser.cpp"
        break;

        case 253: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1868 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 6094 "parser.cpp"
        break;

        case 254: /* show_statement: SHOW PROFILES  */
#line 1875 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
        }
#line 6103 "parser.cpp"
        break;

        case 255: /* show_statement: SHOW BUFFER  */
#line 1879 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
        }
#line 6112 "parser.cpp"
        break;

        case 256: /* show_statement: SHOW MEMINDEX  */
#line 1883 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
        }
#line 6121 "parser.cpp"
        break;

        case 257: /* show_statement: SHOW QUERIES  */
#line 1887 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
        }
#line 6130 "parser.cpp"
        break;

        case 258: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1891 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
            (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
        }
#line 6140 "parser.cpp"
        break;

        case 259: /* show_statement: SHOW TRANSACTIONS  */
#line 1896 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
        }
#line 6149 "parser.cpp"
        break;

        case 260: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1900 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
            (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
        }
#line 6159 "parser.cpp"
        break;

        case 261: /* show_statement: SHOW TRANSACTION HISTORY  */
#line 1905 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactionHistory;
        }
#line 6168 "parser.cpp"
        break;

        case 262: /* show_statement: SHOW SESSION VARIABLES  */
#line 1909 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
        }
#line 6177 "parser.cpp"
        break;

        case 263: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1913 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
        }
#line 6186 "parser.cpp"
        break;

        case 264: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1917 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
            (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 6197 "parser.cpp"
        break;

        case 265: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1923 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
            (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 6208 "parser.cpp"
        break;

        case 266: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1929 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 6220 "parser.cpp"
        break;

        case 267: /* show_statement: SHOW TABLE table_name  */
#line 1936 "parser.y"
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
#line 6236 "parser.cpp"
        break;

        case 268: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1947 "parser.y"
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
#line 6252 "parser.cpp"
        break;

        case 269: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1958 "parser.y"
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
#line 6268 "parser.cpp"
        break;

        case 270: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1969 "parser.y"
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
#line 6285 "parser.cpp"
        break;

        case 271: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1981 "parser.y"
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
#line 6302 "parser.cpp"
        break;

        case 272: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1993 "parser.y"
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
#line 6320 "parser.cpp"
        break;

        case 273: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 2006 "parser.y"
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
#line 6339 "parser.cpp"
        break;

        case 274: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 2020 "parser.y"
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
#line 6355 "parser.cpp"
        break;

        case 275: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 2031 "parser.y"
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
#line 6375 "parser.cpp"
        break;

        case 276: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 2046 "parser.y"
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
#line 6397 "parser.cpp"
        break;

        case 277: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNKS  */
#line 2063 "parser.y"
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
#line 6419 "parser.cpp"
        break;

        case 278: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 2080 "parser.y"
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
#line 6442 "parser.cpp"
        break;

        case 279: /* show_statement: SHOW LOGS  */
#line 2098 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
        }
#line 6451 "parser.cpp"
        break;

        case 280: /* show_statement: SHOW CATALOG  */
#line 2102 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalog;
        }
#line 6460 "parser.cpp"
        break;

        case 281: /* show_statement: SHOW CATALOG STRING  */
#line 2106 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCatalogKey;
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.show_stmt)->var_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 6472 "parser.cpp"
        break;

        case 282: /* show_statement: SHOW CATALOG TO file_path  */
#line 2113 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogToFile;
            (yyval.show_stmt)->file_path_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 6483 "parser.cpp"
        break;

        case 283: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2119 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
        }
#line 6492 "parser.cpp"
        break;

        case 284: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2123 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
        }
#line 6501 "parser.cpp"
        break;

        case 285: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2127 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
            (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 6512 "parser.cpp"
        break;

        case 286: /* show_statement: SHOW MEMORY  */
#line 2133 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
        }
#line 6521 "parser.cpp"
        break;

        case 287: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2137 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
        }
#line 6530 "parser.cpp"
        break;

        case 288: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2141 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
        }
#line 6539 "parser.cpp"
        break;

        case 289: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2145 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));
        }
#line 6551 "parser.cpp"
        break;

        case 290: /* show_statement: SHOW SNAPSHOTS  */
#line 2152 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListSnapshots;
        }
#line 6560 "parser.cpp"
        break;

        case 291: /* show_statement: SHOW SNAPSHOT IDENTIFIER  */
#line 2156 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowSnapshot;
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.show_stmt)->snapshot_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 6572 "parser.cpp"
        break;

        case 292: /* show_statement: SHOW CACHES  */
#line 2163 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCaches;
        }
#line 6581 "parser.cpp"
        break;

        case 293: /* show_statement: SHOW CACHE  */
#line 2167 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowCache;
        }
#line 6590 "parser.cpp"
        break;

        case 294: /* show_statement: SHOW COMPACT  */
#line 2171 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCompact;
        }
#line 6599 "parser.cpp"
        break;

        case 295: /* show_statement: SHOW COMPACT NOT NULLABLE  */
#line 2175 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCompact;
            (yyval.show_stmt)->show_nullable_ = false;
        }
#line 6609 "parser.cpp"
        break;

        case 296: /* show_statement: SHOW CHECKPOINT  */
#line 2180 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCheckpoint;
        }
#line 6618 "parser.cpp"
        break;

        case 297: /* show_statement: SHOW CHECKPOINT NOT NULLABLE  */
#line 2184 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCheckpoint;
            (yyval.show_stmt)->show_nullable_ = false;
        }
#line 6628 "parser.cpp"
        break;

        case 298: /* show_statement: SHOW CHECKPOINT LONG_VALUE  */
#line 2189 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowCheckpoint;
            (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
        }
#line 6638 "parser.cpp"
        break;

        case 299: /* show_statement: SHOW OPTIMIZE  */
#line 2194 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListOptimize;
        }
#line 6647 "parser.cpp"
        break;

        case 300: /* show_statement: SHOW OPTIMIZE NOT NULLABLE  */
#line 2198 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListOptimize;
            (yyval.show_stmt)->show_nullable_ = false;
        }
#line 6657 "parser.cpp"
        break;

        case 301: /* show_statement: SHOW IMPORT  */
#line 2203 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListImport;
        }
#line 6666 "parser.cpp"
        break;

        case 302: /* show_statement: SHOW CLEAN  */
#line 2207 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListClean;
        }
#line 6675 "parser.cpp"
        break;

        case 303: /* show_statement: SHOW CLEAN NOT NULLABLE  */
#line 2211 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListClean;
            (yyval.show_stmt)->show_nullable_ = false;
        }
#line 6685 "parser.cpp"
        break;

        case 304: /* show_statement: SHOW CLEAN LONG_VALUE  */
#line 2216 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowClean;
            (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
        }
#line 6695 "parser.cpp"
        break;

        case 305: /* flush_statement: FLUSH DATA  */
#line 2225 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
        }
#line 6704 "parser.cpp"
        break;

        case 306: /* flush_statement: FLUSH LOG  */
#line 2229 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
        }
#line 6713 "parser.cpp"
        break;

        case 307: /* flush_statement: FLUSH BUFFER  */
#line 2233 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
        }
#line 6722 "parser.cpp"
        break;

        case 308: /* flush_statement: FLUSH CATALOG  */
#line 2237 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kCatalog;
        }
#line 6731 "parser.cpp"
        break;

        case 309: /* optimize_statement: OPTIMIZE table_name  */
#line 2245 "parser.y"
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
#line 6746 "parser.cpp"
        break;

        case 310: /* command_statement: USE IDENTIFIER  */
#line 2259 "parser.y"
        {
            (yyval.command_stmt) = new infinity::CommandStatement();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 6757 "parser.cpp"
        break;

        case 311: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2265 "parser.y"
        {
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
            free((yyvsp[0].str_value));
        }
#line 6767 "parser.cpp"
        break;

        case 312: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2270 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
            free((yyvsp[-1].str_value));
        }
#line 6778 "parser.cpp"
        break;

        case 313: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2276 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
            free((yyvsp[-1].str_value));
        }
#line 6789 "parser.cpp"
        break;

        case 314: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2282 "parser.y"
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
#line 6802 "parser.cpp"
        break;

        case 315: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2290 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession,
                                                                                     infinity::SetVarType::kInteger,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].long_value));
            free((yyvsp[-1].str_value));
        }
#line 6813 "parser.cpp"
        break;

        case 316: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2296 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession,
                                                                                     infinity::SetVarType::kDouble,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].double_value));
            free((yyvsp[-1].str_value));
        }
#line 6824 "parser.cpp"
        break;

        case 317: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2302 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
            free((yyvsp[-1].str_value));
        }
#line 6835 "parser.cpp"
        break;

        case 318: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2308 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
            free((yyvsp[-1].str_value));
        }
#line 6846 "parser.cpp"
        break;

        case 319: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2314 "parser.y"
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
#line 6859 "parser.cpp"
        break;

        case 320: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2322 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal,
                                                                                     infinity::SetVarType::kInteger,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].long_value));
            free((yyvsp[-1].str_value));
        }
#line 6870 "parser.cpp"
        break;

        case 321: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2328 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal,
                                                                                     infinity::SetVarType::kDouble,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].double_value));
            free((yyvsp[-1].str_value));
        }
#line 6881 "parser.cpp"
        break;

        case 322: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2334 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
            free((yyvsp[-1].str_value));
        }
#line 6892 "parser.cpp"
        break;

        case 323: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2340 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
            free((yyvsp[-1].str_value));
        }
#line 6903 "parser.cpp"
        break;

        case 324: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2346 "parser.y"
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
#line 6916 "parser.cpp"
        break;

        case 325: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2354 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig,
                                                                                     infinity::SetVarType::kInteger,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].long_value));
            free((yyvsp[-1].str_value));
        }
#line 6927 "parser.cpp"
        break;

        case 326: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2360 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig,
                                                                                     infinity::SetVarType::kDouble,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].double_value));
            free((yyvsp[-1].str_value));
        }
#line 6938 "parser.cpp"
        break;

        case 327: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON TABLE IDENTIFIER  */
#line 2366 "parser.y"
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
#line 6951 "parser.cpp"
        break;

        case 328: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON DATABASE IDENTIFIER  */
#line 2374 "parser.y"
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
#line 6964 "parser.cpp"
        break;

        case 329: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON SYSTEM  */
#line 2382 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SnapshotCmd>((yyvsp[-2].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kSystem);
            free((yyvsp[-2].str_value));
        }
#line 6975 "parser.cpp"
        break;

        case 330: /* command_statement: DROP SNAPSHOT IDENTIFIER  */
#line 2388 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kDrop, infinity::SnapshotScope::kIgnore);
            free((yyvsp[0].str_value));
        }
#line 6986 "parser.cpp"
        break;

        case 331: /* command_statement: RESTORE DATABASE SNAPSHOT IDENTIFIER  */
#line 2394 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kDatabase);
            free((yyvsp[0].str_value));
        }
#line 6997 "parser.cpp"
        break;

        case 332: /* command_statement: RESTORE TABLE SNAPSHOT IDENTIFIER  */
#line 2400 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kTable);
            free((yyvsp[0].str_value));
        }
#line 7008 "parser.cpp"
        break;

        case 333: /* command_statement: CLEAN DATA  */
#line 2406 "parser.y"
        {
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::CleanupCmd>();
        }
#line 7017 "parser.cpp"
        break;

        case 334: /* command_statement: DUMP INDEX IDENTIFIER ON table_name  */
#line 2410 "parser.y"
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
#line 7033 "parser.cpp"
        break;

        case 335: /* compact_statement: COMPACT TABLE table_name  */
#line 2422 "parser.y"
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
#line 2435 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
            (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
            (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
        }
#line 7061 "parser.cpp"
        break;

        case 337: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2441 "parser.y"
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
#line 2448 "parser.y"
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
#line 2456 "parser.y"
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
#line 2465 "parser.y"
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
#line 2475 "parser.y"
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

        case 342: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT
                     LONG_VALUE  */
#line 2485 "parser.y"
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

        case 343: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT
                     LONG_VALUE BLOCKS  */
#line 2496 "parser.y"
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

        case 344: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT
                     LONG_VALUE BLOCK LONG_VALUE  */
#line 2507 "parser.y"
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

        case 345: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT
                     LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2519 "parser.y"
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
#line 2531 "parser.y"
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

        case 347: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX
                     LONG_VALUE  */
#line 2541 "parser.y"
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

        case 348: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX
                     LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2552 "parser.y"
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

        case 349: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX
                     LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2564 "parser.y"
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
#line 2577 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
        }
#line 7271 "parser.cpp"
        break;

        case 351: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2581 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
            (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
        }
#line 7281 "parser.cpp"
        break;

        case 352: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2586 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
            (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
        }
#line 7291 "parser.cpp"
        break;

        case 353: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2591 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
            (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
            (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
        }
#line 7302 "parser.cpp"
        break;

        case 354: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2597 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
        }
#line 7311 "parser.cpp"
        break;

        case 355: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2601 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
        }
#line 7320 "parser.cpp"
        break;

        case 356: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2605 "parser.y"
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
#line 2612 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
        }
#line 7341 "parser.cpp"
        break;

        case 358: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2616 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
        }
#line 7350 "parser.cpp"
        break;

        case 359: /* admin_statement: ADMIN SHOW SNAPSHOT IDENTIFIER  */
#line 2620 "parser.y"
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
#line 2627 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
            (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7373 "parser.cpp"
        break;

        case 361: /* admin_statement: ADMIN EXPORT SNAPSHOT STRING TO STRING  */
#line 2633 "parser.y"
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
#line 2641 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
            (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7397 "parser.cpp"
        break;

        case 363: /* admin_statement: ADMIN SHOW NODES  */
#line 2647 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
        }
#line 7406 "parser.cpp"
        break;

        case 364: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2651 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
            (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7417 "parser.cpp"
        break;

        case 365: /* admin_statement: ADMIN SHOW NODE  */
#line 2657 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
        }
#line 7426 "parser.cpp"
        break;

        case 366: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2661 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
            (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7437 "parser.cpp"
        break;

        case 367: /* admin_statement: ADMIN SET ADMIN  */
#line 2667 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
            (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
        }
#line 7447 "parser.cpp"
        break;

        case 368: /* admin_statement: ADMIN SET STANDALONE  */
#line 2672 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
            (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
        }
#line 7457 "parser.cpp"
        break;

        case 369: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2677 "parser.y"
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
#line 2684 "parser.y"
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
#line 2693 "parser.y"
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
#line 2703 "parser.y"
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
#line 2713 "parser.y"
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
#line 7529 "parser.cpp"
        break;

        case 374: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2725 "parser.y"
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

        case 375: /* alter_statement: ALTER IDENTIFIER ON table_name with_index_param_list  */
#line 2736 "parser.y"
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
#line 7568 "parser.cpp"
        break;

        case 376: /* check_statement: CHECK SYSTEM  */
#line 2755 "parser.y"
        {
            (yyval.check_stmt) = new infinity::CheckStatement();
            (yyval.check_stmt)->check_type_ = infinity::CheckStmtType::kSystem;
        }
#line 7577 "parser.cpp"
        break;

        case 377: /* check_statement: CHECK TABLE table_name  */
#line 2759 "parser.y"
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
#line 7593 "parser.cpp"
        break;

        case 378: /* expr_array: expr_alias  */
#line 2775 "parser.y"
        {
            (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr *>();
            (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
        }
#line 7602 "parser.cpp"
        break;

        case 379: /* expr_array: expr_array ',' expr_alias  */
#line 2779 "parser.y"
        {
            (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
        }
#line 7611 "parser.cpp"
        break;

        case 380: /* insert_row_list: '(' expr_array ')'  */
#line 2784 "parser.y"
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
#line 7626 "parser.cpp"
        break;

        case 381: /* insert_row_list: insert_row_list ',' '(' expr_array ')'  */
#line 2794 "parser.y"
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
#line 7641 "parser.cpp"
        break;

        case 382: /* expr_alias: expr AS IDENTIFIER  */
#line 2816 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[-2].expr_t);
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7652 "parser.cpp"
        break;

        case 383: /* expr_alias: expr  */
#line 2822 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 7660 "parser.cpp"
        break;

        case 389: /* operand: '(' expr ')'  */
#line 2832 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[-1].expr_t);
        }
#line 7668 "parser.cpp"
        break;

        case 390: /* operand: '(' select_without_paren ')'  */
#line 2835 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 7679 "parser.cpp"
        break;

        case 391: /* operand: constant_expr  */
#line 2841 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].const_expr_t);
        }
#line 7687 "parser.cpp"
        break;

        case 402: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING
                     optional_search_filter_expr ')'  */
#line 2857 "parser.y"
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
#line 7707 "parser.cpp"
        break;

        case 403: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING
                     optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')'  */
#line 2873 "parser.y"
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
#line 7729 "parser.cpp"
        break;

        case 404: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING
                     optional_search_filter_expr ')' IGNORE INDEX  */
#line 2891 "parser.y"
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
#line 7750 "parser.cpp"
        break;

        case 405: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING
                     INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2909 "parser.y"
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
#line 7812 "parser.cpp"
        break;

        case 406: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE
                     INDEX  */
#line 2967 "parser.y"
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
#line 7866 "parser.cpp"
        break;

        case 407: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')'
                     with_index_param_list  */
#line 3017 "parser.y"
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
#line 7924 "parser.cpp"
        break;

        case 408: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3071 "parser.y"
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
#line 7983 "parser.cpp"
        break;

        case 409: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')'
                     USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 3129 "parser.y"
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
#line 8012 "parser.cpp"
        break;

        case 410: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')'
                     IGNORE INDEX  */
#line 3154 "parser.y"
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
#line 8039 "parser.cpp"
        break;

        case 411: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')'
                     with_index_param_list  */
#line 3177 "parser.y"
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
#line 8064 "parser.cpp"
        break;

        case 412: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')'
                     with_index_param_list  */
#line 3198 "parser.y"
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
#line 8089 "parser.cpp"
        break;

        case 413: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3219 "parser.y"
        {
            infinity::MatchExpr *match_text_expr = new infinity::MatchExpr();
            match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
            match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
            match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
            free((yyvsp[-4].str_value));
            free((yyvsp[-2].str_value));
            (yyval.expr_t) = match_text_expr;
        }
#line 8103 "parser.cpp"
        break;

        case 414: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3228 "parser.y"
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
#line 8119 "parser.cpp"
        break;

        case 415: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3239 "parser.y"
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
#line 8135 "parser.cpp"
        break;

        case 416: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3250 "parser.y"
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
#line 8153 "parser.cpp"
        break;

        case 417: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 3264 "parser.y"
        {
            infinity::MatchExpr *match_text_expr = new infinity::MatchExpr();
            match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
            match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
            free((yyvsp[-2].str_value));
            (yyval.expr_t) = match_text_expr;
        }
#line 8165 "parser.cpp"
        break;

        case 418: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3271 "parser.y"
        {
            infinity::MatchExpr *match_text_expr = new infinity::MatchExpr();
            match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
            match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
            match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
            free((yyvsp[-4].str_value));
            free((yyvsp[-2].str_value));
            (yyval.expr_t) = match_text_expr;
        }
#line 8179 "parser.cpp"
        break;

        case 419: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3281 "parser.y"
        {
            infinity::FusionExpr *fusion_expr = new infinity::FusionExpr();
            fusion_expr->method_ = std::string((yyvsp[-1].str_value));
            free((yyvsp[-1].str_value));
            (yyval.expr_t) = fusion_expr;
        }
#line 8190 "parser.cpp"
        break;

        case 420: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 3287 "parser.y"
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
#line 8206 "parser.cpp"
        break;

        case 421: /* sub_search: match_vector_expr  */
#line 3299 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8214 "parser.cpp"
        break;

        case 422: /* sub_search: match_text_expr  */
#line 3302 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8222 "parser.cpp"
        break;

        case 423: /* sub_search: match_tensor_expr  */
#line 3305 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8230 "parser.cpp"
        break;

        case 424: /* sub_search: match_sparse_expr  */
#line 3308 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8238 "parser.cpp"
        break;

        case 425: /* sub_search: query_expr  */
#line 3311 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8246 "parser.cpp"
        break;

        case 426: /* sub_search: fusion_expr  */
#line 3314 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8254 "parser.cpp"
        break;

        case 427: /* sub_search_array: sub_search  */
#line 3318 "parser.y"
        {
            (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr *>();
            (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
        }
#line 8263 "parser.cpp"
        break;

        case 428: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3322 "parser.y"
        {
            (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
        }
#line 8272 "parser.cpp"
        break;

        case 429: /* function_expr: IDENTIFIER '(' ')'  */
#line 3327 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            ParserHelper::ToLower((yyvsp[-2].str_value));
            func_expr->func_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));
            func_expr->arguments_ = nullptr;
            (yyval.expr_t) = func_expr;
        }
#line 8285 "parser.cpp"
        break;

        case 430: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3335 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            ParserHelper::ToLower((yyvsp[-3].str_value));
            func_expr->func_name_ = (yyvsp[-3].str_value);
            free((yyvsp[-3].str_value));
            func_expr->arguments_ = (yyvsp[-1].expr_array_t);
            (yyval.expr_t) = func_expr;
        }
#line 8298 "parser.cpp"
        break;

        case 431: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 3343 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            ParserHelper::ToLower((yyvsp[-4].str_value));
            func_expr->func_name_ = (yyvsp[-4].str_value);
            free((yyvsp[-4].str_value));
            func_expr->arguments_ = (yyvsp[-1].expr_array_t);
            func_expr->distinct_ = true;
            (yyval.expr_t) = func_expr;
        }
#line 8312 "parser.cpp"
        break;

        case 432: /* function_expr: YEAR '(' expr ')'  */
#line 3352 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "year";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8324 "parser.cpp"
        break;

        case 433: /* function_expr: MONTH '(' expr ')'  */
#line 3359 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "month";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8336 "parser.cpp"
        break;

        case 434: /* function_expr: DAY '(' expr ')'  */
#line 3366 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "day";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8348 "parser.cpp"
        break;

        case 435: /* function_expr: HOUR '(' expr ')'  */
#line 3373 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "hour";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8360 "parser.cpp"
        break;

        case 436: /* function_expr: MINUTE '(' expr ')'  */
#line 3380 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "minute";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8372 "parser.cpp"
        break;

        case 437: /* function_expr: SECOND '(' expr ')'  */
#line 3387 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "second";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8384 "parser.cpp"
        break;

        case 438: /* function_expr: operand IS NOT NULLABLE  */
#line 3394 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "is_not_null";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8396 "parser.cpp"
        break;

        case 439: /* function_expr: operand IS NULLABLE  */
#line 3401 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "is_null";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8408 "parser.cpp"
        break;

        case 440: /* function_expr: NOT operand  */
#line 3408 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "not";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8420 "parser.cpp"
        break;

        case 441: /* function_expr: '-' operand  */
#line 3415 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "-";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8432 "parser.cpp"
        break;

        case 442: /* function_expr: '+' operand  */
#line 3422 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "+";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8444 "parser.cpp"
        break;

        case 443: /* function_expr: operand '-' operand  */
#line 3429 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "-";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8457 "parser.cpp"
        break;

        case 444: /* function_expr: operand '+' operand  */
#line 3437 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "+";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8470 "parser.cpp"
        break;

        case 445: /* function_expr: operand '*' operand  */
#line 3445 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "*";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8483 "parser.cpp"
        break;

        case 446: /* function_expr: operand '/' operand  */
#line 3453 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "/";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8496 "parser.cpp"
        break;

        case 447: /* function_expr: operand '%' operand  */
#line 3461 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "%";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8509 "parser.cpp"
        break;

        case 448: /* function_expr: operand '=' operand  */
#line 3469 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8522 "parser.cpp"
        break;

        case 449: /* function_expr: operand EQUAL operand  */
#line 3477 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8535 "parser.cpp"
        break;

        case 450: /* function_expr: operand NOT_EQ operand  */
#line 3485 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "<>";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8548 "parser.cpp"
        break;

        case 451: /* function_expr: operand '<' operand  */
#line 3493 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "<";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8561 "parser.cpp"
        break;

        case 452: /* function_expr: operand '>' operand  */
#line 3501 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = ">";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8574 "parser.cpp"
        break;

        case 453: /* function_expr: operand LESS_EQ operand  */
#line 3509 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "<=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8587 "parser.cpp"
        break;

        case 454: /* function_expr: operand GREATER_EQ operand  */
#line 3517 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = ">=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8600 "parser.cpp"
        break;

        case 455: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3525 "parser.y"
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
#line 8635 "parser.cpp"
        break;

        case 456: /* function_expr: operand LIKE operand  */
#line 3555 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "like";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8648 "parser.cpp"
        break;

        case 457: /* function_expr: operand NOT LIKE operand  */
#line 3563 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "not_like";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8661 "parser.cpp"
        break;

        case 458: /* conjunction_expr: expr AND expr  */
#line 3572 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "and";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8674 "parser.cpp"
        break;

        case 459: /* conjunction_expr: expr OR expr  */
#line 3580 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "or";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8687 "parser.cpp"
        break;

        case 460: /* between_expr: operand BETWEEN operand AND operand  */
#line 3589 "parser.y"
        {
            infinity::BetweenExpr *between_expr = new infinity::BetweenExpr();
            between_expr->value_ = (yyvsp[-4].expr_t);
            between_expr->lower_bound_ = (yyvsp[-2].expr_t);
            between_expr->upper_bound_ = (yyvsp[0].expr_t);
            (yyval.expr_t) = between_expr;
        }
#line 8699 "parser.cpp"
        break;

        case 461: /* in_expr: operand IN '(' expr_array ')'  */
#line 3597 "parser.y"
        {
            infinity::InExpr *in_expr = new infinity::InExpr(true);
            in_expr->left_ = (yyvsp[-4].expr_t);
            in_expr->arguments_ = (yyvsp[-1].expr_array_t);
            (yyval.expr_t) = in_expr;
        }
#line 8710 "parser.cpp"
        break;

        case 462: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3603 "parser.y"
        {
            infinity::InExpr *in_expr = new infinity::InExpr(false);
            in_expr->left_ = (yyvsp[-5].expr_t);
            in_expr->arguments_ = (yyvsp[-1].expr_array_t);
            (yyval.expr_t) = in_expr;
        }
#line 8721 "parser.cpp"
        break;

        case 463: /* case_expr: CASE expr case_check_array END  */
#line 3610 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->expr_ = (yyvsp[-2].expr_t);
            case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
            (yyval.expr_t) = case_expr;
        }
#line 8732 "parser.cpp"
        break;

        case 464: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3616 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->expr_ = (yyvsp[-4].expr_t);
            case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
            case_expr->else_expr_ = (yyvsp[-1].expr_t);
            (yyval.expr_t) = case_expr;
        }
#line 8744 "parser.cpp"
        break;

        case 465: /* case_expr: CASE case_check_array END  */
#line 3623 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
            (yyval.expr_t) = case_expr;
        }
#line 8754 "parser.cpp"
        break;

        case 466: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3628 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
            case_expr->else_expr_ = (yyvsp[-1].expr_t);
            (yyval.expr_t) = case_expr;
        }
#line 8765 "parser.cpp"
        break;

        case 467: /* case_check_array: WHEN expr THEN expr  */
#line 3635 "parser.y"
        {
            (yyval.case_check_array_t) = new std::vector<infinity::WhenThen *>();
            infinity::WhenThen *when_then_ptr = new infinity::WhenThen();
            when_then_ptr->when_ = (yyvsp[-2].expr_t);
            when_then_ptr->then_ = (yyvsp[0].expr_t);
            (yyval.case_check_array_t)->emplace_back(when_then_ptr);
        }
#line 8777 "parser.cpp"
        break;

        case 468: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3642 "parser.y"
        {
            infinity::WhenThen *when_then_ptr = new infinity::WhenThen();
            when_then_ptr->when_ = (yyvsp[-2].expr_t);
            when_then_ptr->then_ = (yyvsp[0].expr_t);
            (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
            (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
        }
#line 8789 "parser.cpp"
        break;

        case 469: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3650 "parser.y"
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
#line 8806 "parser.cpp"
        break;

        case 470: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3663 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8817 "parser.cpp"
        break;

        case 471: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3669 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8828 "parser.cpp"
        break;

        case 472: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3675 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
            subquery_expr->left_ = (yyvsp[-4].expr_t);
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8840 "parser.cpp"
        break;

        case 473: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3682 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
            subquery_expr->left_ = (yyvsp[-5].expr_t);
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8852 "parser.cpp"
        break;

        case 474: /* column_expr: IDENTIFIER  */
#line 3690 "parser.y"
        {
            infinity::ColumnExpr *column_expr = new infinity::ColumnExpr();
            ParserHelper::ToLower((yyvsp[0].str_value));
            column_expr->names_.emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
            (yyval.expr_t) = column_expr;
        }
#line 8864 "parser.cpp"
        break;

        case 475: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3697 "parser.y"
        {
            infinity::ColumnExpr *column_expr = (infinity::ColumnExpr *)(yyvsp[-2].expr_t);
            ParserHelper::ToLower((yyvsp[0].str_value));
            column_expr->names_.emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
            (yyval.expr_t) = column_expr;
        }
#line 8876 "parser.cpp"
        break;

        case 476: /* column_expr: '*'  */
#line 3704 "parser.y"
        {
            infinity::ColumnExpr *column_expr = new infinity::ColumnExpr();
            column_expr->star_ = true;
            (yyval.expr_t) = column_expr;
        }
#line 8886 "parser.cpp"
        break;

        case 477: /* column_expr: column_expr '.' '*'  */
#line 3709 "parser.y"
        {
            infinity::ColumnExpr *column_expr = (infinity::ColumnExpr *)(yyvsp[-2].expr_t);
            if (column_expr->star_) {
                yyerror(&yyloc, scanner, result, "Invalid column expression format");
                YYERROR;
            }
            column_expr->star_ = true;
            (yyval.expr_t) = column_expr;
        }
#line 8900 "parser.cpp"
        break;

        case 478: /* constant_expr: STRING  */
#line 3719 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
            const_expr->str_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8910 "parser.cpp"
        break;

        case 479: /* constant_expr: TRUE  */
#line 3724 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
            const_expr->bool_value_ = true;
            (yyval.const_expr_t) = const_expr;
        }
#line 8920 "parser.cpp"
        break;

        case 480: /* constant_expr: FALSE  */
#line 3729 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
            const_expr->bool_value_ = false;
            (yyval.const_expr_t) = const_expr;
        }
#line 8930 "parser.cpp"
        break;

        case 481: /* constant_expr: DOUBLE_VALUE  */
#line 3734 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
            const_expr->double_value_ = (yyvsp[0].double_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8940 "parser.cpp"
        break;

        case 482: /* constant_expr: LONG_VALUE  */
#line 3739 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
            const_expr->integer_value_ = (yyvsp[0].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8950 "parser.cpp"
        break;

        case 483: /* constant_expr: DATE STRING  */
#line 3744 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8960 "parser.cpp"
        break;

        case 484: /* constant_expr: TIME STRING  */
#line 3749 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8970 "parser.cpp"
        break;

        case 485: /* constant_expr: DATETIME STRING  */
#line 3754 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8980 "parser.cpp"
        break;

        case 486: /* constant_expr: TIMESTAMP STRING  */
#line 3759 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8990 "parser.cpp"
        break;

        case 487: /* constant_expr: INTERVAL interval_expr  */
#line 3764 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 8998 "parser.cpp"
        break;

        case 488: /* constant_expr: interval_expr  */
#line 3767 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9006 "parser.cpp"
        break;

        case 489: /* constant_expr: common_array_expr  */
#line 3770 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9014 "parser.cpp"
        break;

        case 490: /* constant_expr: curly_brackets_expr  */
#line 3773 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9022 "parser.cpp"
        break;

        case 491: /* common_array_expr: array_expr  */
#line 3777 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9030 "parser.cpp"
        break;

        case 492: /* common_array_expr: subarray_array_expr  */
#line 3780 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9038 "parser.cpp"
        break;

        case 493: /* common_array_expr: sparse_array_expr  */
#line 3783 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9046 "parser.cpp"
        break;

        case 494: /* common_array_expr: empty_array_expr  */
#line 3786 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9054 "parser.cpp"
        break;

        case 495: /* common_sparse_array_expr: sparse_array_expr  */
#line 3790 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9062 "parser.cpp"
        break;

        case 496: /* common_sparse_array_expr: array_expr  */
#line 3793 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9070 "parser.cpp"
        break;

        case 497: /* common_sparse_array_expr: empty_array_expr  */
#line 3796 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9078 "parser.cpp"
        break;

        case 498: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3800 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9086 "parser.cpp"
        break;

        case 499: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3804 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
            const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
            (yyval.const_expr_t) = const_expr;
        }
#line 9096 "parser.cpp"
        break;

        case 500: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3809 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9105 "parser.cpp"
        break;

        case 501: /* sparse_array_expr: long_sparse_array_expr  */
#line 3814 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9113 "parser.cpp"
        break;

        case 502: /* sparse_array_expr: double_sparse_array_expr  */
#line 3817 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9121 "parser.cpp"
        break;

        case 503: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3821 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9129 "parser.cpp"
        break;

        case 504: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3825 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
            const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
            const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
            delete (yyvsp[0].int_sparse_ele_t);
            (yyval.const_expr_t) = const_expr;
        }
#line 9141 "parser.cpp"
        break;

        case 505: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3832 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
            (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
            delete (yyvsp[0].int_sparse_ele_t);
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9152 "parser.cpp"
        break;

        case 506: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3839 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9160 "parser.cpp"
        break;

        case 507: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3843 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
            const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
            const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
            delete (yyvsp[0].float_sparse_ele_t);
            (yyval.const_expr_t) = const_expr;
        }
#line 9172 "parser.cpp"
        break;

        case 508: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3850 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
            (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
            delete (yyvsp[0].float_sparse_ele_t);
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9183 "parser.cpp"
        break;

        case 509: /* empty_array_expr: '[' ']'  */
#line 3857 "parser.y"
        {
            (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
        }
#line 9191 "parser.cpp"
        break;

        case 510: /* curly_brackets_expr: unclosed_curly_brackets_expr '}'  */
#line 3861 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9199 "parser.cpp"
        break;

        case 511: /* curly_brackets_expr: '{' '}'  */
#line 3864 "parser.y"
        {
            (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
        }
#line 9207 "parser.cpp"
        break;

        case 512: /* unclosed_curly_brackets_expr: '{' constant_expr  */
#line 3868 "parser.y"
        {
            (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
            (yyval.const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
        }
#line 9216 "parser.cpp"
        break;

        case 513: /* unclosed_curly_brackets_expr: unclosed_curly_brackets_expr ',' constant_expr  */
#line 3872 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9225 "parser.cpp"
        break;

        case 514: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3877 "parser.y"
        {
            (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
        }
#line 9233 "parser.cpp"
        break;

        case 515: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3881 "parser.y"
        {
            (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
        }
#line 9241 "parser.cpp"
        break;

        case 516: /* array_expr: long_array_expr  */
#line 3885 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9249 "parser.cpp"
        break;

        case 517: /* array_expr: double_array_expr  */
#line 3888 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9257 "parser.cpp"
        break;

        case 518: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3892 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9265 "parser.cpp"
        break;

        case 519: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3896 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
            const_expr->long_array_.emplace_back((yyvsp[0].long_value));
            (yyval.const_expr_t) = const_expr;
        }
#line 9275 "parser.cpp"
        break;

        case 520: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3901 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9284 "parser.cpp"
        break;

        case 521: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3906 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9292 "parser.cpp"
        break;

        case 522: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3910 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
            const_expr->double_array_.emplace_back((yyvsp[0].double_value));
            (yyval.const_expr_t) = const_expr;
        }
#line 9302 "parser.cpp"
        break;

        case 523: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3915 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9311 "parser.cpp"
        break;

        case 524: /* interval_expr: LONG_VALUE SECONDS  */
#line 3920 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kSecond;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9322 "parser.cpp"
        break;

        case 525: /* interval_expr: LONG_VALUE SECOND  */
#line 3926 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kSecond;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9333 "parser.cpp"
        break;

        case 526: /* interval_expr: LONG_VALUE MINUTES  */
#line 3932 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMinute;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9344 "parser.cpp"
        break;

        case 527: /* interval_expr: LONG_VALUE MINUTE  */
#line 3938 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMinute;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9355 "parser.cpp"
        break;

        case 528: /* interval_expr: LONG_VALUE HOURS  */
#line 3944 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kHour;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9366 "parser.cpp"
        break;

        case 529: /* interval_expr: LONG_VALUE HOUR  */
#line 3950 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kHour;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9377 "parser.cpp"
        break;

        case 530: /* interval_expr: LONG_VALUE DAYS  */
#line 3956 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kDay;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9388 "parser.cpp"
        break;

        case 531: /* interval_expr: LONG_VALUE DAY  */
#line 3962 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kDay;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9399 "parser.cpp"
        break;

        case 532: /* interval_expr: LONG_VALUE MONTHS  */
#line 3968 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMonth;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9410 "parser.cpp"
        break;

        case 533: /* interval_expr: LONG_VALUE MONTH  */
#line 3974 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMonth;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9421 "parser.cpp"
        break;

        case 534: /* interval_expr: LONG_VALUE YEARS  */
#line 3980 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kYear;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9432 "parser.cpp"
        break;

        case 535: /* interval_expr: LONG_VALUE YEAR  */
#line 3986 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kYear;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9443 "parser.cpp"
        break;

        case 536: /* copy_option_list: copy_option  */
#line 3997 "parser.y"
        {
            (yyval.copy_option_array) = new std::vector<infinity::CopyOption *>();
            (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
        }
#line 9452 "parser.cpp"
        break;

        case 537: /* copy_option_list: copy_option_list ',' copy_option  */
#line 4001 "parser.y"
        {
            (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
            (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
        }
#line 9461 "parser.cpp"
        break;

        case 538: /* copy_option: FORMAT IDENTIFIER  */
#line 4006 "parser.y"
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
#line 9498 "parser.cpp"
        break;

        case 539: /* copy_option: DELIMITER STRING  */
#line 4038 "parser.y"
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
#line 9513 "parser.cpp"
        break;

        case 540: /* copy_option: HEADER  */
#line 4048 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
            (yyval.copy_option_t)->header_ = true;
        }
#line 9523 "parser.cpp"
        break;

        case 541: /* copy_option: OFFSET LONG_VALUE  */
#line 4053 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
            (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
        }
#line 9533 "parser.cpp"
        break;

        case 542: /* copy_option: LIMIT LONG_VALUE  */
#line 4058 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
            (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
        }
#line 9543 "parser.cpp"
        break;

        case 543: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 4063 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
            (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
        }
#line 9553 "parser.cpp"
        break;

        case 544: /* file_path: STRING  */
#line 4069 "parser.y"
        {
            (yyval.str_value) = (yyvsp[0].str_value);
        }
#line 9561 "parser.cpp"
        break;

        case 545: /* if_exists: IF EXISTS  */
#line 4073 "parser.y"
        {
            (yyval.bool_value) = true;
        }
#line 9567 "parser.cpp"
        break;

        case 546: /* if_exists: %empty  */
#line 4074 "parser.y"
        {
            (yyval.bool_value) = false;
        }
#line 9573 "parser.cpp"
        break;

        case 547: /* if_not_exists: IF NOT EXISTS  */
#line 4076 "parser.y"
        {
            (yyval.bool_value) = true;
        }
#line 9579 "parser.cpp"
        break;

        case 548: /* if_not_exists: %empty  */
#line 4077 "parser.y"
        {
            (yyval.bool_value) = false;
        }
#line 9585 "parser.cpp"
        break;

        case 551: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 4092 "parser.y"
        {
            (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
            (yyval.if_not_exists_info_t)->exists_ = true;
            (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 9598 "parser.cpp"
        break;

        case 552: /* if_not_exists_info: %empty  */
#line 4100 "parser.y"
        {
            (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
        }
#line 9606 "parser.cpp"
        break;

        case 553: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 4104 "parser.y"
        {
            (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
        }
#line 9614 "parser.cpp"
        break;

        case 554: /* with_index_param_list: %empty  */
#line 4107 "parser.y"
        {
            (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter *>();
        }
#line 9622 "parser.cpp"
        break;

        case 555: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 4111 "parser.y"
        {
            (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
        }
#line 9630 "parser.cpp"
        break;

        case 556: /* optional_table_properties_list: %empty  */
#line 4114 "parser.y"
        {
            (yyval.with_index_param_list_t) = nullptr;
        }
#line 9638 "parser.cpp"
        break;

        case 557: /* index_param_list: index_param  */
#line 4118 "parser.y"
        {
            (yyval.index_param_list_t) = new std::vector<infinity::InitParameter *>();
            (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
        }
#line 9647 "parser.cpp"
        break;

        case 558: /* index_param_list: index_param_list ',' index_param  */
#line 4122 "parser.y"
        {
            (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
            (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
        }
#line 9656 "parser.cpp"
        break;

        case 559: /* index_param: IDENTIFIER  */
#line 4127 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 9667 "parser.cpp"
        break;

        case 560: /* index_param: IDENTIFIER '=' IDENTIFIER  */
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
#line 9682 "parser.cpp"
        break;

        case 561: /* index_param: IDENTIFIER '=' STRING  */
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
#line 9697 "parser.cpp"
        break;

        case 562: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 4153 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));

            (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
        }
#line 9710 "parser.cpp"
        break;

        case 563: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 4161 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));

            (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
        }
#line 9723 "parser.cpp"
        break;

        case 564: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 4172 "parser.y"
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
            ParserHelper::ToLower((yyvsp[-4].str_value));
            (yyval.index_info_t)->column_name_ = (yyvsp[-4].str_value);
            (yyval.index_info_t)->index_param_list_ = (yyvsp[0].with_index_param_list_t);
            free((yyvsp[-4].str_value));
        }
#line 9760 "parser.cpp"
        break;

        case 565: /* index_info: '(' IDENTIFIER ')'  */
#line 4204 "parser.y"
        {
            (yyval.index_info_t) = new infinity::IndexInfo();
            (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
            free((yyvsp[-1].str_value));
        }
#line 9772 "parser.cpp"
        break;

#line 9776 "parser.cpp"

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

#line 4212 "parser.y"

void yyerror(YYLTYPE *llocp, void *lexer, infinity::ParserResult *result, const char *msg) {
    if (result->IsError())
        return;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
    fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

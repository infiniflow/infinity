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
    YYSYMBOL_JSON_EXTRACT = 152,                         /* JSON_EXTRACT  */
    YYSYMBOL_EQUAL = 153,                                /* EQUAL  */
    YYSYMBOL_NOT_EQ = 154,                               /* NOT_EQ  */
    YYSYMBOL_LESS_EQ = 155,                              /* LESS_EQ  */
    YYSYMBOL_GREATER_EQ = 156,                           /* GREATER_EQ  */
    YYSYMBOL_BETWEEN = 157,                              /* BETWEEN  */
    YYSYMBOL_AND = 158,                                  /* AND  */
    YYSYMBOL_OR = 159,                                   /* OR  */
    YYSYMBOL_EXTRACT = 160,                              /* EXTRACT  */
    YYSYMBOL_LIKE = 161,                                 /* LIKE  */
    YYSYMBOL_DATA = 162,                                 /* DATA  */
    YYSYMBOL_LOG = 163,                                  /* LOG  */
    YYSYMBOL_BUFFER = 164,                               /* BUFFER  */
    YYSYMBOL_TRANSACTIONS = 165,                         /* TRANSACTIONS  */
    YYSYMBOL_TRANSACTION = 166,                          /* TRANSACTION  */
    YYSYMBOL_MEMINDEX = 167,                             /* MEMINDEX  */
    YYSYMBOL_USING = 168,                                /* USING  */
    YYSYMBOL_SESSION = 169,                              /* SESSION  */
    YYSYMBOL_GLOBAL = 170,                               /* GLOBAL  */
    YYSYMBOL_OFF = 171,                                  /* OFF  */
    YYSYMBOL_EXPORT = 172,                               /* EXPORT  */
    YYSYMBOL_CONFIGS = 173,                              /* CONFIGS  */
    YYSYMBOL_CONFIG = 174,                               /* CONFIG  */
    YYSYMBOL_PROFILES = 175,                             /* PROFILES  */
    YYSYMBOL_VARIABLES = 176,                            /* VARIABLES  */
    YYSYMBOL_VARIABLE = 177,                             /* VARIABLE  */
    YYSYMBOL_LOGS = 178,                                 /* LOGS  */
    YYSYMBOL_CATALOGS = 179,                             /* CATALOGS  */
    YYSYMBOL_CATALOG = 180,                              /* CATALOG  */
    YYSYMBOL_SEARCH = 181,                               /* SEARCH  */
    YYSYMBOL_MATCH = 182,                                /* MATCH  */
    YYSYMBOL_MAXSIM = 183,                               /* MAXSIM  */
    YYSYMBOL_QUERY = 184,                                /* QUERY  */
    YYSYMBOL_QUERIES = 185,                              /* QUERIES  */
    YYSYMBOL_FUSION = 186,                               /* FUSION  */
    YYSYMBOL_ROWLIMIT = 187,                             /* ROWLIMIT  */
    YYSYMBOL_ADMIN = 188,                                /* ADMIN  */
    YYSYMBOL_LEADER = 189,                               /* LEADER  */
    YYSYMBOL_FOLLOWER = 190,                             /* FOLLOWER  */
    YYSYMBOL_LEARNER = 191,                              /* LEARNER  */
    YYSYMBOL_CONNECT = 192,                              /* CONNECT  */
    YYSYMBOL_STANDALONE = 193,                           /* STANDALONE  */
    YYSYMBOL_NODES = 194,                                /* NODES  */
    YYSYMBOL_NODE = 195,                                 /* NODE  */
    YYSYMBOL_REMOVE = 196,                               /* REMOVE  */
    YYSYMBOL_SNAPSHOT = 197,                             /* SNAPSHOT  */
    YYSYMBOL_SNAPSHOTS = 198,                            /* SNAPSHOTS  */
    YYSYMBOL_RECOVER = 199,                              /* RECOVER  */
    YYSYMBOL_RESTORE = 200,                              /* RESTORE  */
    YYSYMBOL_CACHES = 201,                               /* CACHES  */
    YYSYMBOL_CACHE = 202,                                /* CACHE  */
    YYSYMBOL_PERSISTENCE = 203,                          /* PERSISTENCE  */
    YYSYMBOL_OBJECT = 204,                               /* OBJECT  */
    YYSYMBOL_OBJECTS = 205,                              /* OBJECTS  */
    YYSYMBOL_FILES = 206,                                /* FILES  */
    YYSYMBOL_MEMORY = 207,                               /* MEMORY  */
    YYSYMBOL_ALLOCATION = 208,                           /* ALLOCATION  */
    YYSYMBOL_HISTORY = 209,                              /* HISTORY  */
    YYSYMBOL_CHECK = 210,                                /* CHECK  */
    YYSYMBOL_CLEAN = 211,                                /* CLEAN  */
    YYSYMBOL_CHECKPOINT = 212,                           /* CHECKPOINT  */
    YYSYMBOL_IMPORT = 213,                               /* IMPORT  */
    YYSYMBOL_NUMBER = 214,                               /* NUMBER  */
    YYSYMBOL_215_ = 215,                                 /* '='  */
    YYSYMBOL_216_ = 216,                                 /* '<'  */
    YYSYMBOL_217_ = 217,                                 /* '>'  */
    YYSYMBOL_218_ = 218,                                 /* '+'  */
    YYSYMBOL_219_ = 219,                                 /* '-'  */
    YYSYMBOL_220_ = 220,                                 /* '*'  */
    YYSYMBOL_221_ = 221,                                 /* '/'  */
    YYSYMBOL_222_ = 222,                                 /* '%'  */
    YYSYMBOL_223_ = 223,                                 /* ';'  */
    YYSYMBOL_224_ = 224,                                 /* '('  */
    YYSYMBOL_225_ = 225,                                 /* ')'  */
    YYSYMBOL_226_ = 226,                                 /* ','  */
    YYSYMBOL_227_ = 227,                                 /* '.'  */
    YYSYMBOL_228_ = 228,                                 /* ']'  */
    YYSYMBOL_229_ = 229,                                 /* '['  */
    YYSYMBOL_230_ = 230,                                 /* '}'  */
    YYSYMBOL_231_ = 231,                                 /* '{'  */
    YYSYMBOL_232_ = 232,                                 /* ':'  */
    YYSYMBOL_YYACCEPT = 233,                             /* $accept  */
    YYSYMBOL_input_pattern = 234,                        /* input_pattern  */
    YYSYMBOL_statement_list = 235,                       /* statement_list  */
    YYSYMBOL_statement = 236,                            /* statement  */
    YYSYMBOL_explainable_statement = 237,                /* explainable_statement  */
    YYSYMBOL_create_statement = 238,                     /* create_statement  */
    YYSYMBOL_table_element_array = 239,                  /* table_element_array  */
    YYSYMBOL_column_def_array = 240,                     /* column_def_array  */
    YYSYMBOL_table_element = 241,                        /* table_element  */
    YYSYMBOL_table_column = 242,                         /* table_column  */
    YYSYMBOL_column_type_array = 243,                    /* column_type_array  */
    YYSYMBOL_column_type = 244,                          /* column_type  */
    YYSYMBOL_column_constraints = 245,                   /* column_constraints  */
    YYSYMBOL_column_constraint = 246,                    /* column_constraint  */
    YYSYMBOL_default_expr = 247,                         /* default_expr  */
    YYSYMBOL_table_constraint = 248,                     /* table_constraint  */
    YYSYMBOL_identifier_array = 249,                     /* identifier_array  */
    YYSYMBOL_delete_statement = 250,                     /* delete_statement  */
    YYSYMBOL_insert_statement = 251,                     /* insert_statement  */
    YYSYMBOL_optional_identifier_array = 252,            /* optional_identifier_array  */
    YYSYMBOL_explain_statement = 253,                    /* explain_statement  */
    YYSYMBOL_update_statement = 254,                     /* update_statement  */
    YYSYMBOL_update_expr_array = 255,                    /* update_expr_array  */
    YYSYMBOL_update_expr = 256,                          /* update_expr  */
    YYSYMBOL_drop_statement = 257,                       /* drop_statement  */
    YYSYMBOL_copy_statement = 258,                       /* copy_statement  */
    YYSYMBOL_select_statement = 259,                     /* select_statement  */
    YYSYMBOL_select_with_paren = 260,                    /* select_with_paren  */
    YYSYMBOL_select_without_paren = 261,                 /* select_without_paren  */
    YYSYMBOL_select_clause_with_modifier = 262,          /* select_clause_with_modifier  */
    YYSYMBOL_select_clause_without_modifier_paren = 263, /* select_clause_without_modifier_paren  */
    YYSYMBOL_select_clause_without_modifier = 264,       /* select_clause_without_modifier  */
    YYSYMBOL_order_by_clause = 265,                      /* order_by_clause  */
    YYSYMBOL_order_by_expr_list = 266,                   /* order_by_expr_list  */
    YYSYMBOL_order_by_expr = 267,                        /* order_by_expr  */
    YYSYMBOL_order_by_type = 268,                        /* order_by_type  */
    YYSYMBOL_limit_expr = 269,                           /* limit_expr  */
    YYSYMBOL_offset_expr = 270,                          /* offset_expr  */
    YYSYMBOL_distinct = 271,                             /* distinct  */
    YYSYMBOL_highlight_clause = 272,                     /* highlight_clause  */
    YYSYMBOL_from_clause = 273,                          /* from_clause  */
    YYSYMBOL_search_clause = 274,                        /* search_clause  */
    YYSYMBOL_optional_search_filter_expr = 275,          /* optional_search_filter_expr  */
    YYSYMBOL_where_clause = 276,                         /* where_clause  */
    YYSYMBOL_having_clause = 277,                        /* having_clause  */
    YYSYMBOL_group_by_clause = 278,                      /* group_by_clause  */
    YYSYMBOL_set_operator = 279,                         /* set_operator  */
    YYSYMBOL_table_reference = 280,                      /* table_reference  */
    YYSYMBOL_table_reference_unit = 281,                 /* table_reference_unit  */
    YYSYMBOL_table_reference_name = 282,                 /* table_reference_name  */
    YYSYMBOL_table_name = 283,                           /* table_name  */
    YYSYMBOL_table_alias = 284,                          /* table_alias  */
    YYSYMBOL_with_clause = 285,                          /* with_clause  */
    YYSYMBOL_with_expr_list = 286,                       /* with_expr_list  */
    YYSYMBOL_with_expr = 287,                            /* with_expr  */
    YYSYMBOL_join_clause = 288,                          /* join_clause  */
    YYSYMBOL_join_type = 289,                            /* join_type  */
    YYSYMBOL_show_statement = 290,                       /* show_statement  */
    YYSYMBOL_flush_statement = 291,                      /* flush_statement  */
    YYSYMBOL_optimize_statement = 292,                   /* optimize_statement  */
    YYSYMBOL_command_statement = 293,                    /* command_statement  */
    YYSYMBOL_compact_statement = 294,                    /* compact_statement  */
    YYSYMBOL_admin_statement = 295,                      /* admin_statement  */
    YYSYMBOL_alter_statement = 296,                      /* alter_statement  */
    YYSYMBOL_check_statement = 297,                      /* check_statement  */
    YYSYMBOL_expr_array = 298,                           /* expr_array  */
    YYSYMBOL_insert_row_list = 299,                      /* insert_row_list  */
    YYSYMBOL_expr_alias = 300,                           /* expr_alias  */
    YYSYMBOL_expr = 301,                                 /* expr  */
    YYSYMBOL_operand = 302,                              /* operand  */
    YYSYMBOL_match_tensor_expr = 303,                    /* match_tensor_expr  */
    YYSYMBOL_match_vector_expr = 304,                    /* match_vector_expr  */
    YYSYMBOL_match_sparse_expr = 305,                    /* match_sparse_expr  */
    YYSYMBOL_match_text_expr = 306,                      /* match_text_expr  */
    YYSYMBOL_query_expr = 307,                           /* query_expr  */
    YYSYMBOL_fusion_expr = 308,                          /* fusion_expr  */
    YYSYMBOL_sub_search = 309,                           /* sub_search  */
    YYSYMBOL_sub_search_array = 310,                     /* sub_search_array  */
    YYSYMBOL_function_expr = 311,                        /* function_expr  */
    YYSYMBOL_conjunction_expr = 312,                     /* conjunction_expr  */
    YYSYMBOL_between_expr = 313,                         /* between_expr  */
    YYSYMBOL_in_expr = 314,                              /* in_expr  */
    YYSYMBOL_case_expr = 315,                            /* case_expr  */
    YYSYMBOL_case_check_array = 316,                     /* case_check_array  */
    YYSYMBOL_cast_expr = 317,                            /* cast_expr  */
    YYSYMBOL_subquery_expr = 318,                        /* subquery_expr  */
    YYSYMBOL_column_expr = 319,                          /* column_expr  */
    YYSYMBOL_constant_expr = 320,                        /* constant_expr  */
    YYSYMBOL_common_array_expr = 321,                    /* common_array_expr  */
    YYSYMBOL_common_sparse_array_expr = 322,             /* common_sparse_array_expr  */
    YYSYMBOL_subarray_array_expr = 323,                  /* subarray_array_expr  */
    YYSYMBOL_unclosed_subarray_array_expr = 324,         /* unclosed_subarray_array_expr  */
    YYSYMBOL_sparse_array_expr = 325,                    /* sparse_array_expr  */
    YYSYMBOL_long_sparse_array_expr = 326,               /* long_sparse_array_expr  */
    YYSYMBOL_unclosed_long_sparse_array_expr = 327,      /* unclosed_long_sparse_array_expr  */
    YYSYMBOL_double_sparse_array_expr = 328,             /* double_sparse_array_expr  */
    YYSYMBOL_unclosed_double_sparse_array_expr = 329,    /* unclosed_double_sparse_array_expr  */
    YYSYMBOL_empty_array_expr = 330,                     /* empty_array_expr  */
    YYSYMBOL_curly_brackets_expr = 331,                  /* curly_brackets_expr  */
    YYSYMBOL_unclosed_curly_brackets_expr = 332,         /* unclosed_curly_brackets_expr  */
    YYSYMBOL_int_sparse_ele = 333,                       /* int_sparse_ele  */
    YYSYMBOL_float_sparse_ele = 334,                     /* float_sparse_ele  */
    YYSYMBOL_array_expr = 335,                           /* array_expr  */
    YYSYMBOL_long_array_expr = 336,                      /* long_array_expr  */
    YYSYMBOL_unclosed_long_array_expr = 337,             /* unclosed_long_array_expr  */
    YYSYMBOL_double_array_expr = 338,                    /* double_array_expr  */
    YYSYMBOL_unclosed_double_array_expr = 339,           /* unclosed_double_array_expr  */
    YYSYMBOL_interval_expr = 340,                        /* interval_expr  */
    YYSYMBOL_copy_option_list = 341,                     /* copy_option_list  */
    YYSYMBOL_copy_option = 342,                          /* copy_option  */
    YYSYMBOL_file_path = 343,                            /* file_path  */
    YYSYMBOL_if_exists = 344,                            /* if_exists  */
    YYSYMBOL_if_not_exists = 345,                        /* if_not_exists  */
    YYSYMBOL_semicolon = 346,                            /* semicolon  */
    YYSYMBOL_if_not_exists_info = 347,                   /* if_not_exists_info  */
    YYSYMBOL_with_index_param_list = 348,                /* with_index_param_list  */
    YYSYMBOL_optional_table_properties_list = 349,       /* optional_table_properties_list  */
    YYSYMBOL_index_param_list = 350,                     /* index_param_list  */
    YYSYMBOL_index_param = 351,                          /* index_param  */
    YYSYMBOL_index_info = 352                            /* index_info  */
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
#define YYLAST 1604

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS 233
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS 120
/* YYNRULES -- Number of rules.  */
#define YYNRULES 568
/* YYNSTATES -- Number of states.  */
#define YYNSTATES 1276

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK 469

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX) (0 <= (YYX) && (YYX) <= YYMAXUTOK ? YY_CAST(yysymbol_kind_t, yytranslate[YYX]) : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] = {
    0,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   222, 2,   2,   224, 225, 220, 218, 226, 219, 227, 221, 2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   232, 223, 216, 215, 217, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   229, 2,   228, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   231, 2,   230, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
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
    193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] = {
    0,    515,  515,  519,  525,  532,  533,  534,  535,  536,  537,  538,  539,  540,  541,  542,  543,  544,  545,  546,  547,  549,  550,  551,
    552,  553,  554,  555,  556,  557,  558,  559,  560,  567,  584,  601,  617,  646,  661,  693,  711,  729,  757,  788,  792,  797,  801,  807,
    810,  817,  837,  859,  883,  909,  913,  919,  920,  921,  922,  923,  924,  925,  926,  927,  928,  929,  930,  931,  932,  933,  934,  935,
    936,  937,  938,  939,  940,  943,  945,  946,  947,  948,  951,  952,  953,  954,  955,  956,  957,  958,  959,  960,  961,  962,  963,  964,
    965,  966,  967,  968,  969,  970,  971,  972,  973,  974,  975,  976,  977,  978,  979,  980,  981,  982,  983,  984,  985,  986,  987,  988,
    989,  990,  991,  992,  993,  994,  995,  996,  997,  998,  999,  1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1010, 1011, 1012,
    1013, 1014, 1015, 1016, 1017, 1021, 1044, 1048, 1058, 1061, 1064, 1067, 1071, 1074, 1079, 1084, 1091, 1097, 1107, 1123, 1161, 1177, 1180, 1187,
    1204, 1213, 1226, 1230, 1235, 1248, 1261, 1276, 1291, 1306, 1329, 1382, 1437, 1488, 1491, 1494, 1503, 1513, 1516, 1520, 1525, 1552, 1555, 1560,
    1577, 1580, 1584, 1588, 1593, 1599, 1602, 1605, 1609, 1612, 1615, 1618, 1621, 1624, 1628, 1631, 1635, 1638, 1642, 1647, 1651, 1654, 1658, 1661,
    1665, 1668, 1672, 1675, 1679, 1682, 1685, 1688, 1696, 1699, 1714, 1714, 1716, 1730, 1739, 1744, 1753, 1758, 1763, 1769, 1776, 1779, 1783, 1786,
    1791, 1803, 1810, 1824, 1827, 1830, 1833, 1836, 1839, 1842, 1849, 1853, 1857, 1861, 1865, 1872, 1876, 1880, 1884, 1888, 1893, 1897, 1902, 1906,
    1910, 1914, 1920, 1926, 1933, 1944, 1955, 1966, 1978, 1990, 2003, 2017, 2028, 2043, 2060, 2077, 2095, 2099, 2103, 2110, 2116, 2120, 2124, 2130,
    2134, 2138, 2142, 2149, 2153, 2160, 2164, 2168, 2172, 2177, 2181, 2186, 2191, 2195, 2200, 2204, 2208, 2213, 2222, 2226, 2230, 2234, 2242, 2256,
    2262, 2267, 2273, 2279, 2287, 2293, 2299, 2305, 2311, 2319, 2325, 2331, 2337, 2343, 2351, 2357, 2363, 2371, 2379, 2385, 2391, 2397, 2403, 2409,
    2413, 2425, 2438, 2444, 2451, 2459, 2468, 2478, 2488, 2499, 2510, 2522, 2534, 2544, 2555, 2567, 2580, 2584, 2589, 2594, 2600, 2604, 2608, 2615,
    2619, 2623, 2630, 2636, 2644, 2650, 2654, 2660, 2664, 2670, 2675, 2680, 2687, 2696, 2706, 2716, 2728, 2739, 2758, 2762, 2778, 2782, 2787, 2797,
    2819, 2825, 2829, 2830, 2831, 2832, 2833, 2835, 2838, 2844, 2847, 2848, 2849, 2850, 2851, 2852, 2853, 2854, 2855, 2856, 2860, 2876, 2894, 2912,
    2970, 3020, 3074, 3132, 3157, 3180, 3201, 3222, 3231, 3243, 3250, 3260, 3266, 3278, 3281, 3284, 3287, 3290, 3293, 3297, 3301, 3306, 3314, 3322,
    3331, 3346, 3353, 3360, 3367, 3374, 3381, 3388, 3395, 3402, 3409, 3416, 3423, 3431, 3439, 3447, 3455, 3463, 3471, 3479, 3487, 3495, 3503, 3511,
    3519, 3549, 3557, 3566, 3574, 3583, 3591, 3597, 3604, 3610, 3617, 3622, 3629, 3636, 3644, 3657, 3663, 3669, 3676, 3684, 3691, 3698, 3703, 3713,
    3718, 3723, 3728, 3733, 3738, 3743, 3748, 3753, 3758, 3763, 3766, 3769, 3772, 3776, 3779, 3782, 3785, 3789, 3792, 3795, 3799, 3803, 3808, 3813,
    3816, 3820, 3824, 3831, 3838, 3842, 3849, 3856, 3860, 3863, 3867, 3871, 3876, 3880, 3884, 3887, 3891, 3895, 3900, 3905, 3909, 3914, 3919, 3925,
    3931, 3937, 3943, 3949, 3955, 3961, 3967, 3973, 3979, 3985, 3996, 4000, 4005, 4010, 4039, 4049, 4054, 4059, 4064, 4070, 4074, 4075, 4077, 4078,
    4080, 4081, 4093, 4101, 4105, 4108, 4112, 4115, 4119, 4123, 4128, 4134, 4144, 4154, 4162, 4173, 4226};
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
                                      "JSON_EXTRACT",
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

#define YYPACT_NINF (-787)

#define yypact_value_is_default(Yyn) ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-556)

#define yytable_value_is_error(Yyn) ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] = {
    1049, 132,  60,   158,  109,  55,   109,  -81,  276,  986,  43,   156,  187,  305,  258,  109,  295,  128,  125,  398,  51,   147,  -52,  322,
    104,  -787, -787, -787, -787, -787, -787, -787, -787, 405,  -787, -787, 333,  -787, -787, -787, -787, -787, -787, -787, -787, 281,  281,  281,
    281,  357,  341,  109,  314,  314,  314,  314,  314,  364,  162,  377,  109,  -21,  419,  447,  492,  1085, -787, -787, -787, -787, -787, -787,
    -787, 405,  -787, -787, -787, -787, -787, 280,  445,  449,  536,  109,  -787, -787, -787, -787, -787, 23,   -787, 264,  384,  -787, 543,  -787,
    -787, 57,   537,  -787, 549,  -787, -787, -787, 286,  67,   138,  272,  -787, 464,  109,  109,  553,  -787, -787, -787, -787, -787, -787, 566,
    366,  -787, 593,  407,  431,  300,  347,  439,  634,  456,  568,  443,  457,  459,  109,  -787, -787, 451,  453,  -787, 1024, -787, 621,  -787,
    -787, 28,   605,  -787, 613,  612,  684,  109,  109,  109,  686,  629,  631,  471,  640,  713,  109,  109,  109,  715,  -787, 718,  720,  659,
    723,  723,  668,  59,   92,   100,  -787, 504,  599,  618,  -787, 515,  -787, -787, -787, 737,  -787, 749,  -787, -787, 723,  -787, -787, 763,
    -787, -787, -787, -787, -787, 642,  -787, 655,  109,  454,  -787, 707,  572,  295,  723,  -787, 786,  -787, 641,  -787, 805,  -787, -787, 810,
    -787, 811,  -787, 815,  824,  -787, 831,  790,  847,  656,  868,  872,  878,  -787, -787, -787, -787, -787, 28,   -787, -787, -787, 668,  832,
    821,  816,  756,  -30,  -787, 471,  -787, 109,  423,  890,  42,   -787, -787, -787, -787, -787, 833,  -787, 679,  -26,  -787, 668,  -787, -787,
    818,  820,  676,  -787, -787, 518,  774,  677,  678,  493,  911,  913,  915,  917,  919,  -787, -787, 921,  701,  704,  706,  708,  709,  712,
    714,  716,  358,  717,  719,  862,  862,  -787, 544,  18,   11,   152,  -787, -2,   582,  -787, -787, -787, -787, -787, -787, -787, -787, -787,
    -787, -787, -787, -787, 710,  -787, -787, -787, -103, -787, -787, 88,   -787, 342,  -787, -787, 240,  -787, -787, 351,  -787, 354,  -787, -787,
    -787, -787, -787, -787, -787, -787, -787, -787, -787, -787, -787, -787, -787, -787, -787, -787, -787, 926,  925,  -787, -787, -787, -787, -787,
    -787, -787, -787, -787, 861,  896,  898,  869,  109,  333,  -787, -787, -787, 944,  61,   -787, 943,  -787, -787, 873,  374,  -787, 947,  -787,
    -787, -787, 728,  729,  -41,  668,  668,  886,  -787, 953,  -52,  44,   904,  738,  958,  960,  -787, -787, 359,  743,  -787, 109,  668,  720,
    -787, 432,  744,  745,  369,  -787, -787, -787, -787, -787, -787, -787, -787, -787, -787, -787, -787, 862,  752,  1047, 899,  668,  668,  176,
    417,  -787, -787, -787, -787, -787, 518,  -787, 668,  668,  668,  668,  668,  668,  30,   976,  757,  758,  759,  760,  984,  987,  312,  312,
    765,  -92,  -787, 764,  -787, -787, -787, -787, -787, -787, 918,  668,  994,  668,  668,  -15,  787,  194,  862,  862,  862,  862,  862,  862,
    862,  862,  862,  862,  862,  862,  862,  862,  31,   784,  -787, 1009, -787, 1011, -787, 14,   -787, 1014, -787, 1018, -787, 983,  564,  804,
    -787, 806,  808,  1026, -787, 817,  -787, 1037, -787, 49,   1041, 881,  883,  -787, -787, -787, 668,  979,  841,  -787, 135,  432,  668,  -787,
    -787, 72,   1221, 927,  844,  394,  -787, -787, -787, -52,  1066, 935,  -787, -787, -787, 1068, 668,  848,  -787, 432,  -787, 6,    6,    668,
    -787, 424,  899,  912,  851,  2,    -16,  428,  -787, 668,  668,  -51,  122,  181,  199,  201,  203,  -787, 476,  1007, 668,  30,   668,  1080,
    859,  480,  -787, -787, 702,  723,  -787, -787, -787, 929,  864,  862,  544,  957,  -787, 177,  177,  163,  163,  688,  177,  177,  163,  163,
    312,  312,  -787, -787, -787, -787, -787, -787, 858,  -787, 871,  -787, -787, -787, -787, 1094, 1098, -787, 1102, 890,  1103, -787, -787, -787,
    1101, -787, -787, 1105, 1106, 891,  24,   931,  668,  -787, -787, -787, 432,  1112, -787, -787, -787, -787, -787, -787, -787, -787, -787, -787,
    -787, -787, 894,  -787, -787, -787, -787, -787, -787, -787, -787, -787, -787, -787, -787, 895,  902,  906,  907,  908,  909,  910,  914,  329,
    916,  890,  1108, 44,   405,  920,  1137, -787, 484,  922,  1136, 1138, 1139, 34,   -787, 1141, 494,  -787, 508,  519,  -787, 923,  -787, 1221,
    668,  -787, 668,  111,  101,  -787, -787, -787, -787, -787, -787, 1145, 862,  -86,  520,  -48,  928,  200,  933,  -787, 1158, -787, -787, 1087,
    544,  177,  942,  523,  -787, 862,  1162, 1164, 705,  1126, 959,  531,  -787, 534,  540,  -787, 1167, -787, -787, -52,  949,  725,  -787, 75,
    -787, 389,  659,  -787, -787, 1170, 1221, 1221, 731,  880,  1030, 1124, 1147, 1257, 1045, 1048, -787, -787, 234,  -787, 1051, 890,  567,  955,
    1046, -787, 1022, -787, -787, 668,  -787, -787, -787, -787, -787, -787, -787, 6,    -787, -787, -787, 961,  432,  115,  -787, 668,  966,  195,
    668,  784,  963,  1178, 968,  668,  -787, 970,  980,  981,  569,  -787, -787, 1047, 1199, -787, 1201, 609,  -787, 1102, -787, -787, 1103, 505,
    32,   24,   1152, -787, -787, -787, -787, -787, -787, 1153, -787, 1208, -787, -787, -787, -787, -787, -787, -787, -787, 988,  1161, 606,  995,
    608,  -787, 999,  1000, 1001, 1002, 1006, 1010, 1017, 1021, 1029, 1140, 1032, 1035, 1044, 1050, 1052, 1053, 1054, 1055, 1056, 1057, 1151, 1058,
    1060, 1061, 1062, 1063, 1064, 1065, 1067, 1069, 1071, 1157, 1072, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1081, 1082, 1163, 1109, 1110, 1113,
    1127, 1128, 1131, 1132, 1133, 1134, 1135, 1179, 1142, 1143, 1144, 1146, 1148, 1149, 1150, 1154, 1155, 1156, 1182, 1159, -787, -787, 14,   -787,
    1171, 1193, 614,  -787, 1102, 1267, 1274, 630,  -787, -787, -787, 432,  -787, -787, 178,  1160, 16,   1165, -787, -787, -787, 1166, 1226, 1168,
    432,  -787, 6,    -787, -787, -787, -787, -787, -787, -787, -787, -787, -787, 1288, -787, 75,   725,  24,   24,   1114, 389,  1312, 1313, -787,
    1360, -787, -787, 1221, 1365, 1367, 1371, 1372, 1373, 1377, 1381, 1382, 1383, 1169, 1384, 1388, 1390, 1391, 1392, 1393, 1394, 1395, 1396, 1397,
    1180, 1398, 1399, 1401, 1402, 1403, 1404, 1405, 1406, 1407, 1408, 1189, 1410, 1411, 1412, 1413, 1414, 1415, 1416, 1417, 1418, 1419, 1200, 1421,
    1422, 1423, 1424, 1425, 1426, 1427, 1428, 1429, 1430, 1211, 1432, 1433, 1434, 1435, 1436, 1437, 1438, 1439, 1440, 1441, 1222, 1443, -787, 1446,
    1447, -787, 635,  -787, 861,  -787, 1448, 1449, 1450, 254,  1230, -787, 637,  1451, -787, -787, 1400, 890,  -787, 668,  668,  -787, 1231, -787,
    1233, 1234, 1235, 1236, 1238, 1239, 1240, 1241, 1242, 1462, 1244, 1245, 1246, 1247, 1248, 1249, 1250, 1251, 1252, 1253, 1473, 1255, 1256, 1258,
    1259, 1260, 1261, 1262, 1263, 1264, 1265, 1476, 1266, 1268, 1269, 1270, 1271, 1272, 1273, 1275, 1276, 1277, 1486, 1278, 1279, 1280, 1281, 1282,
    1283, 1284, 1285, 1286, 1287, 1493, 1289, 1290, 1291, 1292, 1293, 1294, 1295, 1296, 1297, 1298, 1507, 1299, -787, -787, -787, -787, 1300, 1301,
    1302, 968,  -787, -787, 563,  668,  644,  891,  432,  -787, -787, -787, -787, -787, -787, -787, -787, -787, -787, 1304, -787, -787, -787, -787,
    -787, -787, -787, -787, -787, -787, 1305, -787, -787, -787, -787, -787, -787, -787, -787, -787, -787, 1306, -787, -787, -787, -787, -787, -787,
    -787, -787, -787, -787, 1307, -787, -787, -787, -787, -787, -787, -787, -787, -787, -787, 1308, -787, -787, -787, -787, -787, -787, -787, -787,
    -787, -787, 1309, -787, 1521, 1531, 196,  1311, 1314, 1532, 1534, -787, -787, -787, 432,  -787, -787, -787, -787, -787, -787, -787, 1315, 1316,
    968,  861,  -787, 1537, 829,  229,  1319, 1533, 1320, -787, 837,  1540, -787, 968,  861,  968,  -10,  1541, -787, 1501, 1324, -787, 1325, 1512,
    1513, -787, -787, -787, -1,   -85,  -787, 1329, 1515, 1516, -787, 1517, 1518, 1555, -787, 1335, -787, 1336, 1337, 1558, 1560, 861,  1339, 1340,
    -787, 861,  -787, -787};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] = {
    237, 0,   0,   0,   0,   0,   0,   0,   237, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   237, 0,   553, 3,   5,   10,  12,
    13,  11,  6,   7,   9,   182, 181, 0,   8,   14,  15,  16,  17,  18,  19,  20,  551, 551, 551, 551, 551, 0,   0,   549, 549, 549, 549, 549, 0,
    230, 0,   0,   0,   0,   0,   0,   237, 168, 21,  26,  28,  27,  22,  23,  25,  24,  29,  30,  31,  32,  0,   295, 300, 0,   0,   251, 252, 250,
    256, 260, 0,   257, 0,   0,   253, 0,   255, 280, 281, 0,   258, 0,   291, 293, 294, 0,   287, 303, 297, 302, 0,   0,   0,   0,   306, 307, 308,
    309, 311, 310, 0,   236, 238, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   378, 335, 0,   0,   1,   237, 2,   220, 222, 223,
    0,   205, 187, 193, 0,   0,   0,   0,   0,   0,   0,   0,   166, 0,   0,   0,   0,   0,   0,   331, 0,   0,   215, 0,   0,   0,   0,   0,   0,
    167, 0,   0,   0,   267, 268, 261, 262, 263, 0,   264, 0,   254, 282, 0,   259, 292, 0,   285, 284, 288, 289, 305, 0,   299, 0,   0,   0,   337,
    0,   0,   0,   0,   359, 0,   369, 0,   370, 0,   356, 357, 0,   352, 0,   365, 367, 0,   360, 0,   0,   0,   0,   0,   0,   0,   379, 186, 185,
    4,   221, 0,   183, 184, 204, 0,   0,   201, 0,   34,  0,   35,  166, 554, 0,   0,   0,   237, 548, 173, 175, 174, 176, 0,   231, 0,   215, 170,
    0,   162, 547, 0,   0,   475, 479, 482, 483, 0,   0,   0,   0,   0,   0,   0,   0,   0,   480, 481, 0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   477, 237, 0,   0,   0,   380, 385, 386, 400, 398, 401, 399, 402, 403, 395, 390, 389, 388, 396, 397, 387, 394, 393, 491,
    494, 0,   495, 503, 0,   504, 0,   496, 492, 0,   493, 518, 0,   519, 0,   490, 315, 317, 316, 313, 314, 320, 322, 321, 318, 319, 325, 327, 326,
    323, 324, 290, 296, 301, 0,   0,   270, 269, 275, 265, 266, 283, 286, 304, 298, 557, 0,   0,   0,   0,   0,   239, 312, 362, 0,   353, 358, 0,
    366, 361, 0,   0,   368, 0,   333, 334, 332, 0,   0,   207, 0,   0,   203, 550, 0,   237, 0,   0,   0,   0,   0,   330, 160, 0,   0,   164, 0,
    0,   0,   169, 214, 0,   0,   0,   527, 526, 529, 528, 531, 530, 533, 532, 535, 534, 537, 536, 0,   0,   441, 237, 0,   0,   0,   0,   484, 485,
    486, 487, 488, 0,   489, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   443, 442, 0,   0,   524, 521, 511, 501, 506, 509,
    513, 514, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   500, 0,   505,
    0,   508, 0,   512, 0,   520, 0,   523, 276, 271, 0,   377, 0,   0,   0,   336, 0,   371, 0,   354, 0,   0,   0,   0,   364, 190, 189, 0,   209,
    192, 194, 199, 200, 0,   188, 33,  37,  0,   0,   0,   0,   43,  47,  48,  237, 0,   41,  329, 328, 165, 0,   0,   163, 177, 172, 171, 0,   0,
    0,   429, 0,   237, 0,   0,   0,   0,   0,   466, 0,   0,   0,   0,   0,   0,   0,   0,   475, 0,   0,   0,   0,   0,   0,   213, 0,   392, 391,
    0,   0,   381, 384, 459, 460, 0,   0,   237, 0,   440, 450, 451, 454, 455, 0,   457, 449, 452, 453, 445, 444, 446, 447, 448, 476, 478, 502, 0,
    507, 0,   510, 515, 522, 525, 0,   0,   272, 0,   0,   0,   374, 240, 355, 0,   338, 363, 0,   0,   206, 0,   211, 0,   197, 198, 196, 202, 0,
    55,  56,  59,  60,  57,  58,  61,  62,  78,  63,  65,  64,  81,  68,  69,  70,  66,  67,  71,  72,  73,  74,  75,  76,  77,  0,   0,   0,   0,
    0,   0,   0,   0,   557, 0,   0,   559, 0,   40,  0,   0,   161, 0,   0,   0,   0,   0,   0,   543, 0,   0,   538, 0,   0,   430, 0,   471, 0,
    0,   464, 0,   0,   0,   438, 437, 436, 435, 434, 433, 0,   0,   0,   0,   0,   0,   0,   0,   419, 0,   517, 516, 0,   237, 458, 0,   0,   439,
    0,   0,   0,   277, 273, 562, 0,   560, 0,   0,   45,  339, 372, 373, 237, 208, 224, 226, 235, 227, 0,   215, 195, 39,  0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   153, 154, 157, 150, 157, 0,   0,   0,   36,  44,  568, 42,  382, 0,   545, 544, 542, 541, 540, 546, 180, 0,   178,
    431, 472, 0,   468, 0,   467, 0,   0,   0,   0,   0,   0,   0,   213, 0,   417, 0,   0,   0,   0,   473, 462, 461, 0,   278, 0,   0,   556, 0,
    376, 375, 0,   0,   0,   0,   0,   244, 245, 246, 247, 243, 248, 0,   233, 0,   228, 423, 421, 424, 422, 425, 426, 427, 210, 219, 0,   0,   0,
    53,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   155, 152, 0,   151, 50,  49,  0,   159, 0,   0,   0,   0,   539, 470, 465, 469, 432, 456, 0,   0,
    0,   0,   497, 499, 498, 213, 0,   0,   212, 420, 0,   474, 463, 279, 274, 563, 564, 566, 565, 561, 46,  0,   340, 235, 225, 0,   0,   232, 0,
    0,   217, 80,  0,   148, 149, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   156, 0,   0,   158, 0,   38,  557, 383, 0,   0,   0,   0,   0,   418,
    0,   341, 229, 241, 0,   0,   428, 0,   0,   191, 0,   54,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   52,  51,  558, 567, 0,   0,   213, 213, 415,
    179, 0,   0,   0,   218, 216, 79,  85,  86,  83,  84,  87,  88,  89,  90,  91,  0,   82,  129, 130, 127, 128, 131, 132, 133, 134, 135, 0,   126,
    96,  97,  94,  95,  98,  99,  100, 101, 102, 0,   93,  107, 108, 105, 106, 109, 110, 111, 112, 113, 0,   104, 140, 141, 138, 139, 142, 143, 144,
    145, 146, 0,   137, 118, 119, 116, 117, 120, 121, 122, 123, 124, 0,   115, 0,   0,   0,   0,   0,   0,   0,   343, 342, 348, 242, 234, 92,  136,
    103, 114, 147, 125, 213, 0,   213, 557, 416, 349, 344, 0,   0,   0,   0,   414, 0,   0,   345, 213, 557, 213, 557, 0,   350, 346, 0,   410, 0,
    0,   0,   413, 351, 347, 557, 404, 412, 0,   0,   0,   409, 0,   0,   0,   408, 0,   406, 0,   0,   0,   0,   557, 0,   0,   411, 557, 405, 407};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] = {
    -787, -787, -787, 1431, 1502, 226,  -787, -787, 893,  -581, -787, -681, -787, 807,  802,  -787, -610, 304,  338,  1327, -787, 356,  -787, 1172,
    368,  380,  -7,   1548, -19,  1204, 1341, -96,  -787, -787, 945,  -787, -787, -787, -787, -787, -787, -787, -786, -252, -787, -787, -787, -787,
    761,  -70,  76,   623,  -787, -787, 1375, -787, -787, 382,  392,  397,  400,  410,  -787, -787, -787, -236, -787, 1115, -261, -262, -717, -707,
    -703, -702, -701, -700, 620,  -787, -787, -787, -787, -787, -787, 1173, -787, -787, -396, -294, -291, -787, -787, -787, 789,  -787, -787, -787,
    -787, 791,  -787, -787, 1090, 1091, 792,  -787, -787, -787, -787, 1310, -537, 809,  -158, 570,  651,  -787, -787, -666, -787, 671,  777,  -787};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] = {0,   23,  24,  25,  66,   26,  533, 734, 534, 535, 840, 671, 762, 763, 912, 536, 402, 27,  28,  250,
                                         29,  30,  259, 260, 31,   32,  33,  34,  35,  147, 235, 148, 240, 522, 523, 635, 391, 527, 238, 521,
                                         631, 746, 714, 262, 1053, 958, 145, 740, 741, 742, 743, 828, 36,  120, 121, 744, 825, 37,  38,  39,
                                         40,  41,  42,  43,  44,   299, 545, 300, 301, 302, 303, 304, 305, 306, 307, 308, 835, 836, 309, 310,
                                         311, 312, 313, 432, 314,  315, 316, 317, 318, 929, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328,
                                         462, 463, 329, 330, 331,  332, 333, 334, 688, 689, 264, 159, 150, 141, 155, 504, 768, 731, 732, 539};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] = {
    409,  73,   388,  138,  465,  764,  461,  408,  427,  733,  935,  265,  431,  690,  785,  267,  268,  269,  267,  268,  269,  458,  459,  458,
    459,  394,  16,   58,   829,  180,  360,  454,  455,  569,  605,  457,  146,  777,  830,  735,  261,  520,  831,  832,  833,  834,  109,  530,
    371,  236,  570,  1259, 142,  468,  143,  168,  169,  695,  73,   144,  586,  187,  335,  766,  336,  337,  469,  470,  503,  839,  841,  696,
    469,  470,  682,  683,  110,  503,  826,  403,  59,   624,  61,   1260, 134,  684,  685,  686,  62,   63,   625,  118,  142,  64,   143,  340,
    51,   341,  342,  144,  511,  274,  135,  345,  274,  346,  347,  469,  470,  512,  469,  470,  58,   275,  276,  277,  275,  276,  277,  278,
    16,   338,  278,  489,  778,  490,  1247, 157,  524,  525,  827,  60,   123,  579,  188,  1256, 167,  124,  387,  125,  792,  126,  469,  470,
    196,  547,  587,  1042, 279,  280,  281,  279,  280,  281,  343,  914,  469,  470,  1248, 179,  469,  470,  348,  427,  45,   46,   47,   1257,
    557,  558,  48,   49,   22,   531,  701,  532,  553,  710,  794,  563,  564,  565,  566,  567,  568,  467,  201,  202,  789,  111,  52,   53,
    54,   687,  395,  788,  55,   56,   607,  922,  407,  612,  1224, 170,  796,  633,  634,  637,  584,  585,  229,  197,  591,  592,  593,  594,
    595,  596,  597,  598,  599,  600,  601,  602,  603,  604,  112,  243,  244,  245,  339,  404,  181,  948,  67,   1237, 253,  254,  255,  829,
    297,  464,  298,  297,  460,  298,  460,  297,  739,  830,  295,  606,  234,  831,  832,  833,  834,  951,  1129, 469,  470,  117,  797,  344,
    430,  636,  797,  589,  555,  469,  470,  349,  194,  469,  470,  195,  364,  456,  198,  65,   469,  470,  1055, 1,    629,  2,    3,    4,
    5,    6,    7,    67,   9,    469,  470,  797,  473,  127,  119,  11,   699,  700,  12,   122,  13,   14,   15,   758,  680,  136,  473,  709,
    68,   711,  491,  691,  492,  128,  -556, -556, 797,  129,  139,  397,  130,  721,  590,  140,  473,  50,   -556, -556, 476,  477,  469,  470,
    469,  470,  -556, 469,  470,  146,  1207, 1208, 156,  199,  69,   702,  474,  475,  476,  477,  723,  149,  16,   57,   479,  469,  470,  469,
    470,  469,  470,  759,  70,   760,  761,  164,  910,  68,   1125, 524,  266,  267,  268,  269,  71,   466,  467,  -556, -556, 483,  484,  485,
    486,  487,  158,  529,  72,   165,  74,   166,  -556, 481,  482,  483,  484,  485,  486,  487,  75,   1044, 758,  69,   1038, 76,   703,  503,
    77,   556,  480,  481,  482,  483,  484,  485,  486,  487,  78,   -555, 925,  70,   171,  719,  704,  142,  705,  143,  706,  149,  131,  132,
    144,  71,   551,  786,  1230, 787,  1232, 1134, 182,  183,  270,  271,  508,  72,   791,  74,   17,   133,  172,  1244, 272,  1246, 273,  398,
    399,  75,   759,  274,  760,  761,  76,   805,  365,  77,   495,  113,  114,  115,  496,  275,  276,  277,  400,  78,   19,   278,  366,  367,
    448,  546,  449,  450,  802,  116,  451,  21,   209,  210,  191,  192,  193,  211,  837,  173,  266,  267,  268,  269,  22,   560,  927,  561,
    175,  562,  279,  280,  281,  282,  212,  283,  697,  284,  698,  285,  562,  286,  176,  287,  213,  288,  177,  214,  215,  216,  200,  217,
    923,  289,  676,  926,  485,  486,  487,  693,  936,  919,  949,  178,  950,  218,  219,  189,  220,  221,  186,  266,  267,  268,  269,  290,
    190,  291,  353,  292,  203,  354,  355,  1233, 184,  185,  356,  357,  515,  516,  270,  271,  493,  722,  494,  290,  1245, 291,  1249, 292,
    272,  497,  273,  498,  499,  430,  500,  274,  542,  543,  1258, 293,  294,  295,  469,  470,  205,  296,  552,  275,  276,  277,  297,  206,
    298,  278,  1209, 1272, 207,  1210, 1211, 1275, 616,  617,  1212, 1213, 943,  944,  945,  946,  1030, 270,  271,  674,  675,  204,  16,   160,
    161,  162,  163,  272,  208,  273,  279,  280,  281,  282,  274,  283,  222,  284,  223,  285,  226,  286,  233,  287,  225,  288,  275,  276,
    277,  692,  467,  224,  278,  289,  227,  471,  228,  472,  413,  414,  415,  416,  417,  418,  419,  420,  421,  422,  423,  424,  237,  266,
    267,  268,  269,  290,  230,  291,  231,  292,  239,  279,  280,  281,  282,  241,  283,  242,  284,  246,  285,  247,  286,  248,  287,  249,
    288,  151,  152,  153,  154,  801,  707,  488,  289,  715,  716,  717,  718,  772,  467,  293,  294,  295,  251,  473,  252,  296,  256,  780,
    781,  257,  297,  258,  298,  261,  290,  263,  291,  350,  292,  351,  816,  782,  781,  474,  475,  476,  477,  478,  358,  270,  271,  479,
    783,  467,  793,  488,  804,  467,  352,  272,  359,  273,  806,  807,  810,  811,  274,  812,  543,  555,  293,  294,  295,  813,  814,  361,
    296,  368,  275,  276,  277,  297,  362,  298,  278,  266,  267,  268,  269,  818,  -249, 819,  820,  821,  822,  363,  823,  824,  372,  1136,
    915,  543,  940,  467,  369,  480,  481,  482,  483,  484,  485,  486,  487,  279,  280,  281,  282,  373,  283,  374,  284,  375,  285,  1135,
    286,  376,  287,  377,  288,  473,  842,  843,  844,  845,  846,  378,  289,  847,  848,  959,  960,  962,  963,  379,  849,  850,  851,  1033,
    543,  474,  475,  476,  477,  380,  725,  425,  426,  479,  290,  381,  291,  382,  292,  1037, 467,  272,  852,  273,  1124, 811,  1131, 781,
    274,  266,  267,  268,  269,  1215, 543,  383,  1214, 1235, 1236, 384,  275,  276,  277,  1241, 1242, 385,  278,  1047, 1048, 389,  293,  294,
    295,  390,  392,  393,  296,  401,  406,  405,  410,  297,  411,  298,  412,  428,  429,  480,  481,  482,  483,  484,  485,  486,  487,  279,
    280,  281,  282,  433,  283,  434,  284,  435,  285,  436,  286,  437,  287,  440,  288,  438,  441,  501,  442,  502,  443,  444,  289,  425,
    445,  488,  446,  503,  447,  452,  505,  453,  506,  272,  507,  273,  510,  513,  514,  517,  274,  518,  519,  526,  290,  528,  291,  537,
    292,  540,  538,  541,  275,  276,  277,  544,  549,  550,  278,  853,  854,  855,  856,  857,  554,  16,   858,  859,  571,  572,  573,  574,
    575,  860,  861,  862,  576,  79,   578,  577,  293,  294,  295,  581,  580,  583,  296,  279,  280,  281,  282,  297,  283,  298,  284,  863,
    285,  80,   286,  588,  287,  297,  288,  608,  81,   610,  82,   83,   613,  84,   289,  614,  -552, 615,  85,   86,   618,  621,  619,  1,
    620,  2,    3,    4,    5,    6,    7,    8,    9,    10,   622,  623,  290,  626,  291,  11,   292,  627,  12,   628,  13,   14,   15,   630,
    1,    672,  2,    3,    4,    5,    6,    7,    8,    9,    10,   632,  673,  677,  678,  679,  11,   587,  681,  12,   694,  13,   14,   15,
    293,  294,  295,  708,  712,  713,  296,  469,  720,  724,  726,  297,  1,    298,  2,    3,    4,    5,    6,    7,    728,  9,    16,   727,
    729,  730,  530,  736,  11,   737,  738,  12,   745,  13,   14,   15,   748,  467,  749,  750,  555,  864,  865,  866,  867,  868,  751,  16,
    869,  870,  752,  753,  754,  755,  756,  871,  872,  873,  757,  767,  765,  771,  774,  776,  775,  770,  773,  779,  784,  790,  87,   88,
    89,   90,   795,  91,   92,   874,  798,  93,   94,   95,   799,  16,   96,   800,  97,   803,  718,  717,  98,   99,   808,  815,  809,  817,
    838,  908,  909,  916,  473,  917,  933,  100,  101,  910,  921,  102,  103,  104,  918,  924,  928,  105,  934,  937,  17,   106,  107,  108,
    474,  475,  476,  477,  938,  941,  939,  942,  479,  953,  954,  955,  18,   957,  956,  875,  876,  877,  878,  879,  961,  17,   880,  881,
    19,   964,  965,  966,  967,  882,  883,  884,  968,  973,  20,   21,   969,  18,   886,  887,  888,  889,  890,  970,  984,  891,  892,  971,
    22,   19,   995,  885,  893,  894,  895,  972,  1006, 17,   974,  20,   21,   975,  480,  481,  482,  483,  484,  485,  486,  487,  976,  1035,
    1017, 22,   896,  1028, 977,  1036, 978,  979,  980,  981,  982,  983,  985,  19,   986,  987,  988,  989,  990,  991,  797,  992,  1045, 993,
    21,   994,  996,  997,  998,  999,  1000, 1001, 1002, 1003, 1031, 1004, 1005, 22,   638,  639,  640,  641,  642,  643,  644,  645,  646,  647,
    648,  649,  650,  651,  652,  653,  654,  655,  1032, 656,  657,  658,  659,  660,  661,  1007, 1008, 662,  1049, 1009, 663,  664,  665,  666,
    667,  668,  669,  670,  897,  898,  899,  900,  901,  1010, 1011, 902,  903,  1012, 1013, 1014, 1015, 1016, 904,  905,  906,  1051, 1054, 1052,
    1018, 1019, 1020, 1056, 1021, 1057, 1022, 1023, 1024, 1058, 1059, 1060, 1025, 1026, 1027, 1061, 907,  1029, 1039, 1062, 1063, 1064, 1066, 1040,
    1041, 1043, 1067, 1065, 1068, 1069, 1070, 1071, 1072, 1073, 1074, 1075, 1077, 1078, 1076, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087,
    1088, 1089, 1090, 1091, 1092, 1093, 1094, 1095, 1096, 1097, 1098, 1099, 1100, 1101, 1102, 1103, 1104, 1105, 1106, 1107, 1108, 1109, 1110, 1111,
    1112, 1113, 1114, 1115, 1116, 1117, 1118, 1119, 1120, 1121, 1122, 1123, 1126, 1127, 1128, 1130, 1137, 1132, 1138, 1139, 1140, 1141, 1133, 1142,
    1143, 1144, 1145, 1146, 1147, 1148, 1149, 1150, 1151, 1152, 1153, 1154, 1155, 1156, 1157, 1158, 1159, 1160, 1169, 1161, 1162, 1163, 1164, 1165,
    1166, 1167, 1168, 1170, 1180, 1171, 1172, 1173, 1174, 1175, 1176, 1191, 1177, 1178, 1179, 1181, 1182, 1183, 1184, 1185, 1186, 1187, 1188, 1189,
    1190, 1202, 1192, 1193, 1194, 1195, 1196, 1197, 1198, 1199, 1200, 1201, 1203, 1222, 1204, 1205, 1206, 1216, 1217, 1218, 1219, 1220, 1221, 1223,
    1225, 1239, 1227, 1226, 1228, 1229, 1231, 1234, 1238, 1240, 1243, 1250, 1251, 1252, 1253, 1254, 1255, 1261, 1262, 1263, 1264, 1265, 1266, 1267,
    1268, 1270, 1269, 1271, 1273, 1274, 913,  174,  769,  911,  137,  232,  396,  509,  1046, 386,  1050, 747,  952,  548,  370,  609,  582,  930,
    611,  931,  932,  1034, 947,  0,    920,  439,  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    559};

static const yytype_int16 yycheck[] = {
    261, 8,   238, 22,  298,  671,  297,  259,  270,  619,  796, 169, 273, 550,  695, 4,   5,   6,   4,    5,    6,   5,    6,   5,   6,   55,  78,
    3,   745, 6,   188, 293,  294,  3,    3,    296,  8,    3,   745, 620, 66,   82,  745, 745, 745, 745,  3,    3,   206,  145, 446, 136, 20,  55,
    22,  76,  77,  55,  65,   27,   75,   4,    3,    673,  5,   6,   158, 159,  78,  750, 751, 87,  158,  159,  68,  69,   33,  78,  3,   37,  4,
    32,  6,   168, 33,  79,   80,   81,   169,  170,  41,   15,  20,  174, 22,   3,   36,  5,   6,   27,   39,   90,  51,   3,   90,  5,   6,   158,
    159, 48,  158, 159, 3,    102,  103,  104,  102,  103,  104, 108, 78,  62,   108, 226, 90,  228, 136,  51,   389, 390,  55,  76,  7,   225, 77,
    136, 60,  12,  234, 14,   226,  16,   158,  159,  6,    406, 161, 933, 137,  138, 139, 137, 138, 139,  62,   765, 158,  159, 168, 83,  158, 159,
    62,  425, 32,  33,  34,   168,  429,  430,  38,   39,   224, 129, 225, 131,  412, 573, 226, 440, 441,  442,  443, 444,  445, 226, 110, 111, 87,
    33,  32,  33,  34,  187,  224,  84,   38,   39,   489,  84,  226, 495, 6,    224, 4,   70,  71,  135,  469,  470, 134,  73,  474, 475, 476, 477,
    478, 479, 480, 481, 482,  483,  484,  485,  486,  487,  39,  151, 152, 153,  171, 250, 209, 814, 8,    6,    160, 161,  162, 956, 229, 230, 231,
    229, 228, 231, 228, 229,  224,  956,  220,  220,  224,  956, 956, 956, 956,  225, 4,   158, 159, 3,    66,   171, 88,   526, 66,  73,  73,  158,
    159, 171, 205, 158, 159,  208,  200,  296,  6,    3,    158, 159, 963, 7,    520, 9,   10,  11,  12,   13,   14,  65,   16,  158, 159, 66,  133,
    172, 3,   23,  561, 562,  26,   175,  28,   29,   30,   73,  544, 162, 133,  572, 8,   574, 226, 551,  228,  192, 155,  156, 66,  196, 0,   247,
    199, 587, 132, 223, 133,  197,  153,  154,  155,  156,  158, 159, 158, 159,  161, 158, 159, 8,   1128, 1129, 3,   73,   8,   225, 153, 154, 155,
    156, 588, 72,  78,  197,  161,  158,  159,  158,  159,  158, 159, 129, 8,    131, 132, 3,   134, 65,   1036, 632, 3,    4,   5,   6,   8,   225,
    226, 216, 217, 218, 219,  220,  221,  222,  72,   394,  8,   227, 8,   14,   215, 216, 217, 218, 219,  220,  221, 222,  8,   938, 73,  65,  226,
    8,   225, 78,  8,   428,  215,  216,  217,  218,  219,  220, 221, 222, 8,    62,  225, 65,  3,   581,  225,  20,  225,  22,  225, 72,  32,  33,
    27,  65,  65,  696, 1222, 698,  1224, 1049, 176,  177,  73,  74,  368, 65,   708, 65,  172, 51,  3,    1237, 83,  1239, 85,  32,  33,  65,  129,
    90,  131, 132, 65,  725,  10,   65,   226,  162,  163,  164, 230, 102, 103,  104, 51,  65,  200, 108,  24,   25,  122,  405, 124, 125, 720, 180,
    128, 211, 188, 189, 204,  205,  206,  193,  746,  3,    3,   4,   5,   6,    224, 84,  793, 86,  224,  88,   137, 138,  139, 140, 163, 142, 84,
    144, 86,  146, 88,  148,  73,   150,  173,  152,  73,   176, 177, 178, 62,   180, 789, 160, 537, 792,  220,  221, 222,  554, 797, 773, 33,  3,
    35,  194, 195, 6,   197,  198,  3,    3,    4,    5,    6,   182, 3,   184,  39,  186, 3,   42,  43,   1225, 176, 177,  47,  48,  190, 191, 73,
    74,  226, 588, 228, 182,  1238, 184,  1240, 186,  83,   226, 85,  228, 226,  88,  228, 90,  225, 226,  1252, 218, 219,  220, 158, 159, 226, 224,
    225, 102, 103, 104, 229,  6,    231,  108,  39,   1269, 197, 42,  43,  1273, 44,  45,  47,  48,  3,    4,    5,   6,    910, 73,  74,  225, 226,
    55,  78,  53,  54,  55,   56,   83,   197,  85,   137,  138, 139, 140, 90,   142, 197, 144, 4,   146,  197,  148, 21,   150, 76,  152, 102, 103,
    104, 225, 226, 195, 108,  160,  197,  73,   197,  75,   140, 141, 142, 143,  144, 145, 146, 147, 148,  149,  150, 151,  65,  3,   4,   5,   6,
    182, 225, 184, 225, 186,  67,   137,  138,  139,  140,  73,  142, 3,   144,  3,   146, 62,  148, 62,   150,  224, 152,  46,  47,  48,  49,  720,
    226, 227, 160, 225, 226,  5,    6,    225,  226,  218,  219, 220, 74,  133,  3,   224, 3,   225, 226,  3,    229, 3,    231, 66,  182, 4,   184,
    225, 186, 132, 739, 225,  226,  153,  154,  155,  156,  157, 3,   73,  74,   161, 225, 226, 226, 227,  225,  226, 132,  83,  3,   85,  49,  50,
    225, 226, 90,  225, 226,  73,   218,  219,  220,  225,  226, 4,   224, 62,   102, 103, 104, 229, 132,  231,  108, 3,    4,   5,   6,   56,  57,
    58,  59,  60,  61,  132,  63,   64,   4,    1052, 225,  226, 225, 226, 224,  215, 216, 217, 218, 219,  220,  221, 222,  137, 138, 139, 140, 168,
    142, 6,   144, 3,   146,  1051, 148,  6,    150,  4,    152, 133, 91,  92,   93,  94,  95,  3,   160,  98,   99,  225,  226, 225, 226, 4,   105,
    106, 107, 225, 226, 153,  154,  155,  156,  55,   158,  73,  74,  161, 182,  4,   184, 197, 186, 225,  226,  83,  127,  85,  225, 226, 225, 226,
    90,  3,   4,   5,   6,    225,  226,  3,    1133, 44,   45,  3,   102, 103,  104, 42,  43,  3,   108,  953,  954, 53,   218, 219, 220, 68,  74,
    135, 224, 3,   215, 62,   78,   229,  78,   231,  224,  224, 224, 215, 216,  217, 218, 219, 220, 221,  222,  137, 138,  139, 140, 4,   142, 4,
    144, 4,   146, 4,   148,  4,    150,  224,  152,  6,    224, 3,   224, 6,    224, 224, 160, 73,  224,  227,  224, 78,   224, 224, 46,  224, 46,
    83,  77,  85,  4,   6,    77,   4,    90,   225,  225,  69,  182, 4,   184,  55,  186, 3,   224, 3,    102,  103, 104,  224, 224, 224, 108, 91,
    92,  93,  94,  95,  224,  78,   98,   99,   4,    224,  224, 224, 224, 105,  106, 107, 4,   3,   225,  4,    218, 219,  220, 77,  232, 3,   224,
    137, 138, 139, 140, 229,  142,  231,  144,  127,  146,  23,  148, 224, 150,  229, 152, 6,   30,  6,    32,   33,  6,    35,  160, 5,   0,   42,
    40,  41,  224, 3,   224,  7,    224,  9,    10,   11,   12,  13,  14,  15,   16,  17,  225, 6,   182,  4,    184, 23,   186, 168, 26,  168, 28,
    29,  30,  76,  7,   130,  9,    10,   11,   12,   13,   14,  15,  16,  17,   226, 224, 3,   135, 3,    23,   161, 226,  26,  225, 28,  29,  30,
    218, 219, 220, 76,  4,    226,  224,  158,  224,  132,  232, 229, 7,   231,  9,   10,  11,  12,  13,   14,   6,   16,   78,  232, 6,   3,   3,
    6,   23,  4,   4,   26,   181,  28,   29,   30,   4,    226, 224, 224, 73,   91,  92,  93,  94,  95,   224,  78,  98,   99,  224, 224, 224, 224,
    224, 105, 106, 107, 224,  31,   224,  4,    6,    4,    6,   225, 224, 6,    225, 4,   164, 165, 166,  167,  226, 169,  170, 127, 225, 173, 174,
    175, 4,   78,  178, 78,   180,  225,  6,    5,    184,  185, 46,  6,   215,  226, 6,   132, 130, 224,  133,  135, 4,    197, 198, 134, 225, 201,
    202, 203, 168, 225, 229,  207,  226,  225,  172,  211,  212, 213, 153, 154,  155, 156, 224, 6,   225,  6,    161, 57,   57,  3,   188, 52,  226,
    91,  92,  93,  94,  95,   225,  172,  98,   99,   200,  226, 226, 226, 226,  105, 106, 107, 226, 93,   210,  211, 226,  188, 91,  92,  93,  94,
    95,  226, 93,  98,  99,   226,  224,  200,  93,   127,  105, 106, 107, 226,  93,  172, 226, 210, 211,  226,  215, 216,  217, 218, 219, 220, 221,
    222, 226, 4,   93,  224,  127,  93,   226,  3,    226,  226, 226, 226, 226,  226, 226, 200, 226, 226,  226,  226, 226,  226, 66,  226, 6,   226,
    211, 226, 226, 226, 226,  226,  226,  226,  226,  226,  135, 226, 226, 224,  89,  90,  91,  92,  93,   94,   95,  96,   97,  98,  99,  100, 101,
    102, 103, 104, 105, 106,  135,  108,  109,  110,  111,  112, 113, 226, 226,  116, 224, 226, 119, 120,  121,  122, 123,  124, 125, 126, 91,  92,
    93,  94,  95,  226, 226,  98,   99,   226,  226,  226,  226, 226, 105, 106,  107, 53,  6,   54,  226,  226,  226, 6,    226, 6,   226, 226, 226,
    6,   6,   6,   226, 226,  226,  6,    127,  226,  226,  6,   6,   6,   6,    226, 226, 225, 6,   226,  6,    6,   6,    6,   6,   6,   6,   6,
    6,   6,   226, 6,   6,    6,    6,    6,    6,    6,    6,   226, 6,   6,    6,   6,   6,   6,   6,    6,    6,   6,    226, 6,   6,   6,   6,
    6,   6,   6,   6,   6,    6,    226,  6,    6,    6,    6,   6,   6,   6,    6,   6,   6,   226, 6,    4,    4,   4,    4,   4,   225, 225, 6,
    225, 225, 225, 225, 62,   225,  225,  225,  225,  225,  6,   225, 225, 225,  225, 225, 225, 225, 225,  225,  225, 6,    225, 225, 6,   225, 225,
    225, 225, 225, 225, 225,  225,  225,  6,    225,  225,  225, 225, 225, 225,  6,   225, 225, 225, 225,  225,  225, 225,  225, 225, 225, 225, 225,
    225, 6,   225, 225, 225,  225,  225,  225,  225,  225,  225, 225, 225, 4,    226, 226, 226, 225, 225,  225,  225, 225,  225, 4,   225, 4,   6,
    225, 6,   226, 226, 6,    225,  225,  6,    6,    47,   225, 225, 39,  39,   224, 39,  39,  39,  39,   3,    224, 224,  3,   225, 3,   225, 225,
    764, 65,  675, 762, 22,   140,  245,  369,  951,  234,  956, 632, 817, 407,  205, 491, 467, 794, 493,  794,  794, 916,  811, -1,  781, 281, -1,
    -1,  -1,  -1,  -1,  -1,   -1,   -1,   -1,   -1,   -1,   -1,  431};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] = {
    0,   7,   9,   10,  11,  12,  13,  14,  15,  16,  17,  23,  26,  28,  29,  30,  78,  172, 188, 200, 210, 211, 224, 234, 235, 236, 238, 250, 251,
    253, 254, 257, 258, 259, 260, 261, 285, 290, 291, 292, 293, 294, 295, 296, 297, 32,  33,  34,  38,  39,  197, 36,  32,  33,  34,  38,  39,  197,
    3,   283, 76,  283, 169, 170, 174, 3,   237, 238, 250, 251, 254, 257, 258, 259, 290, 291, 292, 293, 294, 3,   23,  30,  32,  33,  35,  40,  41,
    164, 165, 166, 167, 169, 170, 173, 174, 175, 178, 180, 184, 185, 197, 198, 201, 202, 203, 207, 211, 212, 213, 3,   33,  33,  39,  162, 163, 164,
    180, 3,   283, 3,   286, 287, 175, 7,   12,  14,  16,  172, 192, 196, 199, 32,  33,  51,  33,  51,  162, 260, 261, 0,   223, 346, 20,  22,  27,
    279, 8,   262, 264, 72,  345, 345, 345, 345, 345, 347, 3,   283, 72,  344, 344, 344, 344, 344, 3,   227, 14,  283, 76,  77,  224, 3,   3,   3,
    237, 224, 73,  73,  3,   283, 6,   209, 176, 177, 176, 177, 3,   4,   77,  6,   3,   204, 205, 206, 205, 208, 6,   73,  6,   73,  62,  283, 283,
    3,   55,  226, 6,   197, 197, 188, 189, 193, 163, 173, 176, 177, 178, 180, 194, 195, 197, 198, 197, 4,   195, 76,  197, 197, 197, 283, 225, 225,
    236, 21,  224, 263, 264, 65,  271, 67,  265, 73,  3,   283, 283, 283, 3,   62,  62,  224, 252, 74,  3,   283, 283, 283, 3,   3,   3,   255, 256,
    66,  276, 4,   343, 343, 3,   4,   5,   6,   73,  74,  83,  85,  90,  102, 103, 104, 108, 137, 138, 139, 140, 142, 144, 146, 148, 150, 152, 160,
    182, 184, 186, 218, 219, 220, 224, 229, 231, 298, 300, 301, 302, 303, 304, 305, 306, 307, 308, 311, 312, 313, 314, 315, 317, 318, 319, 320, 321,
    323, 324, 325, 326, 327, 328, 329, 330, 331, 332, 335, 336, 337, 338, 339, 340, 3,   5,   6,   62,  171, 3,   5,   6,   62,  171, 3,   5,   6,
    62,  171, 225, 132, 132, 39,  42,  43,  47,  48,  3,   3,   343, 4,   132, 132, 283, 10,  24,  25,  62,  224, 287, 343, 4,   168, 6,   3,   6,
    4,   3,   4,   55,  4,   197, 3,   3,   3,   263, 264, 298, 53,  68,  269, 74,  135, 55,  224, 252, 283, 32,  33,  51,  3,   249, 37,  261, 62,
    215, 226, 276, 301, 78,  78,  224, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 73,  74,  302, 224, 224, 88,  301, 316, 4,   4,
    4,   4,   4,   6,   340, 224, 224, 224, 224, 224, 224, 224, 224, 122, 124, 125, 128, 224, 224, 302, 302, 261, 301, 5,   6,   228, 321, 333, 334,
    230, 320, 225, 226, 55,  158, 159, 73,  75,  133, 153, 154, 155, 156, 157, 161, 215, 216, 217, 218, 219, 220, 221, 222, 227, 226, 228, 226, 228,
    226, 228, 226, 230, 226, 228, 226, 228, 3,   6,   78,  348, 46,  46,  77,  283, 262, 4,   39,  48,  6,   77,  190, 191, 4,   225, 225, 82,  272,
    266, 267, 301, 301, 69,  270, 4,   259, 3,   129, 131, 239, 241, 242, 248, 55,  224, 352, 3,   3,   225, 226, 224, 299, 283, 301, 256, 224, 224,
    65,  225, 298, 224, 73,  261, 301, 301, 316, 84,  86,  88,  301, 301, 301, 301, 301, 301, 3,   319, 4,   224, 224, 224, 224, 4,   4,   225, 225,
    232, 77,  300, 3,   301, 301, 75,  161, 224, 73,  132, 302, 302, 302, 302, 302, 302, 302, 302, 302, 302, 302, 302, 302, 302, 3,   220, 321, 6,
    333, 6,   334, 320, 6,   5,   42,  44,  45,  224, 224, 224, 3,   225, 6,   32,  41,  4,   168, 168, 298, 76,  273, 226, 70,  71,  268, 301, 135,
    89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 108, 109, 110, 111, 112, 113, 116, 119, 120, 121, 122,
    123, 124, 125, 126, 244, 130, 224, 225, 226, 259, 3,   135, 3,   298, 226, 68,  69,  79,  80,  81,  187, 341, 342, 341, 298, 225, 261, 225, 55,
    87,  84,  86,  301, 301, 225, 225, 225, 225, 225, 225, 226, 76,  301, 319, 301, 4,   226, 275, 225, 226, 5,   6,   343, 224, 302, 261, 298, 132,
    158, 232, 232, 6,   6,   3,   350, 351, 249, 240, 242, 6,   4,   4,   224, 280, 281, 282, 283, 288, 181, 274, 267, 4,   224, 224, 224, 224, 224,
    224, 224, 224, 224, 73,  129, 131, 132, 245, 246, 348, 224, 249, 31,  349, 241, 225, 4,   225, 224, 6,   6,   4,   3,   90,  6,   225, 226, 225,
    225, 225, 244, 301, 301, 84,  87,  4,   302, 226, 226, 226, 226, 4,   66,  225, 4,   78,  261, 298, 225, 225, 302, 49,  50,  46,  215, 225, 226,
    225, 225, 226, 6,   259, 226, 56,  58,  59,  60,  61,  63,  64,  289, 3,   55,  284, 303, 304, 305, 306, 307, 308, 309, 310, 276, 6,   244, 243,
    244, 91,  92,  93,  94,  95,  98,  99,  105, 106, 107, 127, 91,  92,  93,  94,  95,  98,  99,  105, 106, 107, 127, 91,  92,  93,  94,  95,  98,
    99,  105, 106, 107, 127, 91,  92,  93,  94,  95,  98,  99,  105, 106, 107, 127, 91,  92,  93,  94,  95,  98,  99,  105, 106, 107, 127, 91,  92,
    93,  94,  95,  98,  99,  105, 106, 107, 127, 132, 130, 134, 246, 247, 247, 249, 225, 224, 135, 168, 298, 342, 225, 84,  301, 225, 225, 301, 321,
    229, 322, 325, 330, 335, 4,   226, 275, 301, 225, 224, 225, 225, 6,   6,   3,   4,   5,   6,   351, 242, 33,  35,  225, 281, 57,  57,  3,   226,
    52,  278, 225, 226, 225, 225, 226, 226, 226, 226, 226, 226, 226, 226, 226, 226, 93,  226, 226, 226, 226, 226, 226, 226, 226, 226, 226, 93,  226,
    226, 226, 226, 226, 226, 226, 226, 226, 226, 93,  226, 226, 226, 226, 226, 226, 226, 226, 226, 226, 93,  226, 226, 226, 226, 226, 226, 226, 226,
    226, 226, 93,  226, 226, 226, 226, 226, 226, 226, 226, 226, 226, 93,  226, 320, 135, 135, 225, 350, 4,   3,   225, 226, 226, 226, 226, 275, 225,
    341, 6,   284, 282, 282, 224, 309, 53,  54,  277, 6,   244, 6,   6,   6,   6,   6,   6,   6,   6,   6,   226, 6,   6,   6,   6,   6,   6,   6,
    6,   6,   6,   226, 6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   226, 6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   226, 6,   6,   6,
    6,   6,   6,   6,   6,   6,   6,   226, 6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   226, 6,   4,   4,   225, 348, 4,   4,   4,   4,   225,
    225, 6,   62,  249, 298, 301, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 6,   225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 6,   225,
    225, 225, 225, 225, 225, 225, 225, 225, 225, 6,   225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 6,   225, 225, 225, 225, 225, 225, 225, 225,
    225, 225, 6,   225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 6,   225, 226, 226, 226, 275, 275, 39,  42,  43,  47,  48,  301, 225, 225, 225,
    225, 225, 225, 225, 4,   4,   6,   225, 225, 6,   6,   226, 275, 226, 275, 348, 6,   44,  45,  6,   225, 4,   225, 42,  43,  6,   275, 348, 275,
    136, 168, 348, 6,   47,  225, 225, 39,  39,  136, 168, 348, 136, 168, 224, 39,  39,  39,  39,  3,   224, 224, 225, 3,   3,   348, 225, 225, 348};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] = {
    0,   233, 234, 235, 235, 236, 236, 236, 236, 236, 236, 236, 236, 236, 236, 236, 236, 236, 236, 236, 236, 237, 237, 237, 237, 237, 237, 237, 237,
    237, 237, 237, 237, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 239, 239, 240, 240, 241, 241, 242, 242, 242, 242, 243, 243, 244, 244, 244,
    244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244,
    244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244,
    244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244,
    244, 244, 244, 244, 244, 245, 245, 246, 246, 246, 246, 247, 247, 248, 248, 249, 249, 250, 251, 251, 252, 252, 253, 253, 254, 255, 255, 256, 257,
    257, 257, 257, 257, 258, 258, 258, 259, 259, 259, 259, 260, 260, 261, 262, 263, 263, 264, 265, 265, 266, 266, 267, 268, 268, 268, 269, 269, 270,
    270, 271, 271, 272, 272, 273, 273, 274, 274, 275, 275, 276, 276, 277, 277, 278, 278, 279, 279, 279, 279, 280, 280, 281, 281, 282, 282, 283, 283,
    284, 284, 284, 284, 285, 285, 286, 286, 287, 288, 288, 289, 289, 289, 289, 289, 289, 289, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290,
    290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290,
    290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 290, 291, 291, 291, 291, 292, 293, 293, 293, 293, 293, 293, 293, 293,
    293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 293, 294, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295,
    295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 296, 296, 296,
    296, 297, 297, 298, 298, 299, 299, 300, 300, 301, 301, 301, 301, 301, 302, 302, 302, 302, 302, 302, 302, 302, 302, 302, 302, 302, 302, 303, 303,
    303, 304, 304, 304, 304, 305, 305, 305, 305, 306, 306, 307, 307, 308, 308, 309, 309, 309, 309, 309, 309, 310, 310, 311, 311, 311, 311, 311, 311,
    311, 311, 311, 311, 311, 311, 311, 311, 311, 311, 311, 311, 311, 311, 311, 311, 311, 311, 311, 311, 311, 311, 311, 311, 312, 312, 313, 314, 314,
    315, 315, 315, 315, 316, 316, 317, 318, 318, 318, 318, 319, 319, 319, 319, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320,
    321, 321, 321, 321, 322, 322, 322, 323, 324, 324, 325, 325, 326, 327, 327, 328, 329, 329, 330, 331, 331, 332, 332, 333, 334, 335, 335, 336, 337,
    337, 338, 339, 339, 340, 340, 340, 340, 340, 340, 340, 340, 340, 340, 340, 340, 341, 341, 342, 342, 342, 342, 342, 342, 342, 343, 344, 344, 345,
    345, 346, 346, 347, 347, 348, 348, 349, 349, 350, 350, 351, 351, 351, 351, 351, 352, 352};

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
    13, 18, 14, 13, 11, 8, 10, 5, 7, 4, 6, 1, 1, 1,  1,  1,  1,  1,  3,  3,  4,  5,  6,  4,  4, 4, 4, 4, 4, 4, 3, 2, 2, 2, 3, 3,  3,  3,  3,  3,  3,
    3,  3,  3,  3,  3,  6, 3,  4, 3, 3, 5, 5, 6, 4,  6,  3,  5,  4,  5,  6,  4,  5,  5,  6,  1, 3, 1, 3, 1, 1, 1, 1, 1, 2, 2, 2,  2,  2,  2,  1,  1,
    1,  1,  1,  1,  1,  1, 1,  1, 2, 2, 3, 1, 1, 2,  2,  3,  2,  2,  3,  2,  2,  2,  2,  3,  3, 3, 1, 1, 2, 2, 3, 2, 2, 3, 2, 2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  1, 3,  2, 2, 2, 1, 2, 2, 2,  1,  2,  0,  3,  0,  1,  0,  2,  0,  4,  0, 4, 0, 1, 3, 1, 3, 3, 3, 3, 6, 3};

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
#line 2505 "parser.cpp"
        break;

        case YYSYMBOL_STRING: /* STRING  */
#line 330 "parser.y"
        {
            free(((*yyvaluep).str_value));
        }
#line 2513 "parser.cpp"
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
#line 2527 "parser.cpp"
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
#line 2541 "parser.cpp"
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
#line 2555 "parser.cpp"
        break;

        case YYSYMBOL_column_type_array: /* column_type_array  */
#line 191 "parser.y"
        {
            fprintf(stderr, "destroy column_type_array\n");
            delete (((*yyvaluep).column_type_array_t));
        }
#line 2564 "parser.cpp"
        break;

        case YYSYMBOL_column_type: /* column_type  */
#line 186 "parser.y"
        {
            fprintf(stderr, "destroy column_type\n");
            delete (((*yyvaluep).column_type_t));
        }
#line 2573 "parser.cpp"
        break;

        case YYSYMBOL_column_constraints: /* column_constraints  */
#line 323 "parser.y"
        {
            fprintf(stderr, "destroy constraints\n");
            if ((((*yyvaluep).column_constraints_t)) != nullptr) {
                delete (((*yyvaluep).column_constraints_t));
            }
        }
#line 2584 "parser.cpp"
        break;

        case YYSYMBOL_default_expr: /* default_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 2592 "parser.cpp"
        break;

        case YYSYMBOL_identifier_array: /* identifier_array  */
#line 334 "parser.y"
        {
            fprintf(stderr, "destroy identifier array\n");
            delete (((*yyvaluep).identifier_array_t));
        }
#line 2601 "parser.cpp"
        break;

        case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 334 "parser.y"
        {
            fprintf(stderr, "destroy identifier array\n");
            delete (((*yyvaluep).identifier_array_t));
        }
#line 2610 "parser.cpp"
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
#line 2624 "parser.cpp"
        break;

        case YYSYMBOL_update_expr: /* update_expr  */
#line 287 "parser.y"
        {
            fprintf(stderr, "destroy update expr\n");
            if (((*yyvaluep).update_expr_t) != nullptr) {
                delete ((*yyvaluep).update_expr_t);
            }
        }
#line 2635 "parser.cpp"
        break;

        case YYSYMBOL_select_statement: /* select_statement  */
#line 369 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2645 "parser.cpp"
        break;

        case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 369 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2655 "parser.cpp"
        break;

        case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 369 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2665 "parser.cpp"
        break;

        case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 369 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2675 "parser.cpp"
        break;

        case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 369 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2685 "parser.cpp"
        break;

        case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 369 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2695 "parser.cpp"
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
#line 2709 "parser.cpp"
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
#line 2723 "parser.cpp"
        break;

        case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 357 "parser.y"
        {
            fprintf(stderr, "destroy order by expr\n");
            delete ((*yyvaluep).order_by_expr_t)->expr_;
            delete ((*yyvaluep).order_by_expr_t);
        }
#line 2733 "parser.cpp"
        break;

        case YYSYMBOL_limit_expr: /* limit_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2741 "parser.cpp"
        break;

        case YYSYMBOL_offset_expr: /* offset_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2749 "parser.cpp"
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
#line 2763 "parser.cpp"
        break;

        case YYSYMBOL_from_clause: /* from_clause  */
#line 352 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2772 "parser.cpp"
        break;

        case YYSYMBOL_search_clause: /* search_clause  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2780 "parser.cpp"
        break;

        case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2788 "parser.cpp"
        break;

        case YYSYMBOL_where_clause: /* where_clause  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2796 "parser.cpp"
        break;

        case YYSYMBOL_having_clause: /* having_clause  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2804 "parser.cpp"
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
#line 2818 "parser.cpp"
        break;

        case YYSYMBOL_table_reference: /* table_reference  */
#line 352 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2827 "parser.cpp"
        break;

        case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 352 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2836 "parser.cpp"
        break;

        case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 352 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2845 "parser.cpp"
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
#line 2858 "parser.cpp"
        break;

        case YYSYMBOL_table_alias: /* table_alias  */
#line 347 "parser.y"
        {
            fprintf(stderr, "destroy table alias\n");
            delete (((*yyvaluep).table_alias_t));
        }
#line 2867 "parser.cpp"
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
#line 2881 "parser.cpp"
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
#line 2895 "parser.cpp"
        break;

        case YYSYMBOL_with_expr: /* with_expr  */
#line 363 "parser.y"
        {
            fprintf(stderr, "destroy with expr\n");
            delete ((*yyvaluep).with_expr_t)->select_;
            delete ((*yyvaluep).with_expr_t);
        }
#line 2905 "parser.cpp"
        break;

        case YYSYMBOL_join_clause: /* join_clause  */
#line 352 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2914 "parser.cpp"
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
#line 2928 "parser.cpp"
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
#line 2942 "parser.cpp"
        break;

        case YYSYMBOL_expr_alias: /* expr_alias  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2950 "parser.cpp"
        break;

        case YYSYMBOL_expr: /* expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2958 "parser.cpp"
        break;

        case YYSYMBOL_operand: /* operand  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2966 "parser.cpp"
        break;

        case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2974 "parser.cpp"
        break;

        case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2982 "parser.cpp"
        break;

        case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2990 "parser.cpp"
        break;

        case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2998 "parser.cpp"
        break;

        case YYSYMBOL_query_expr: /* query_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3006 "parser.cpp"
        break;

        case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3014 "parser.cpp"
        break;

        case YYSYMBOL_sub_search: /* sub_search  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3022 "parser.cpp"
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
#line 3036 "parser.cpp"
        break;

        case YYSYMBOL_function_expr: /* function_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3044 "parser.cpp"
        break;

        case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3052 "parser.cpp"
        break;

        case YYSYMBOL_between_expr: /* between_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3060 "parser.cpp"
        break;

        case YYSYMBOL_in_expr: /* in_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3068 "parser.cpp"
        break;

        case YYSYMBOL_case_expr: /* case_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3076 "parser.cpp"
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
#line 3089 "parser.cpp"
        break;

        case YYSYMBOL_cast_expr: /* cast_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3097 "parser.cpp"
        break;

        case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3105 "parser.cpp"
        break;

        case YYSYMBOL_column_expr: /* column_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3113 "parser.cpp"
        break;

        case YYSYMBOL_constant_expr: /* constant_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3121 "parser.cpp"
        break;

        case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3129 "parser.cpp"
        break;

        case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3137 "parser.cpp"
        break;

        case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3145 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3153 "parser.cpp"
        break;

        case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3161 "parser.cpp"
        break;

        case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3169 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3177 "parser.cpp"
        break;

        case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3185 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3193 "parser.cpp"
        break;

        case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3201 "parser.cpp"
        break;

        case YYSYMBOL_curly_brackets_expr: /* curly_brackets_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3209 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_curly_brackets_expr: /* unclosed_curly_brackets_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3217 "parser.cpp"
        break;

        case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 384 "parser.y"
        {
            delete (((*yyvaluep).int_sparse_ele_t));
        }
#line 3225 "parser.cpp"
        break;

        case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 388 "parser.y"
        {
            delete (((*yyvaluep).float_sparse_ele_t));
        }
#line 3233 "parser.cpp"
        break;

        case YYSYMBOL_array_expr: /* array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3241 "parser.cpp"
        break;

        case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3249 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3257 "parser.cpp"
        break;

        case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3265 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3273 "parser.cpp"
        break;

        case YYSYMBOL_interval_expr: /* interval_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3281 "parser.cpp"
        break;

        case YYSYMBOL_file_path: /* file_path  */
#line 330 "parser.y"
        {
            free(((*yyvaluep).str_value));
        }
#line 3289 "parser.cpp"
        break;

        case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 220 "parser.y"
        {
            fprintf(stderr, "destroy if not exists info\n");
            if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
                delete (((*yyvaluep).if_not_exists_info_t));
            }
        }
#line 3300 "parser.cpp"
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
#line 3314 "parser.cpp"
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
#line 3328 "parser.cpp"
        break;

        case YYSYMBOL_index_info: /* index_info  */
#line 196 "parser.y"
        {
            fprintf(stderr, "destroy index info\n");
            if ((((*yyvaluep).index_info_t)) != nullptr) {
                delete (((*yyvaluep).index_info_t));
            }
        }
#line 3339 "parser.cpp"
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

#line 3447 "parser.cpp"

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
#line 515 "parser.y"
        {
            result->statements_ptr_ = (yyvsp[-1].stmt_array);
        }
#line 3662 "parser.cpp"
        break;

        case 3: /* statement_list: statement  */
#line 519 "parser.y"
        {
            (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
            yylloc.string_length = 0;
            (yyval.stmt_array) = new std::vector<infinity::BaseStatement *>();
            (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
        }
#line 3673 "parser.cpp"
        break;

        case 4: /* statement_list: statement_list ';' statement  */
#line 525 "parser.y"
        {
            (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
            yylloc.string_length = 0;
            (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
            (yyval.stmt_array) = (yyvsp[-2].stmt_array);
        }
#line 3684 "parser.cpp"
        break;

        case 5: /* statement: create_statement  */
#line 532 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].create_stmt);
        }
#line 3690 "parser.cpp"
        break;

        case 6: /* statement: drop_statement  */
#line 533 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].drop_stmt);
        }
#line 3696 "parser.cpp"
        break;

        case 7: /* statement: copy_statement  */
#line 534 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].copy_stmt);
        }
#line 3702 "parser.cpp"
        break;

        case 8: /* statement: show_statement  */
#line 535 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].show_stmt);
        }
#line 3708 "parser.cpp"
        break;

        case 9: /* statement: select_statement  */
#line 536 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].select_stmt);
        }
#line 3714 "parser.cpp"
        break;

        case 10: /* statement: delete_statement  */
#line 537 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].delete_stmt);
        }
#line 3720 "parser.cpp"
        break;

        case 11: /* statement: update_statement  */
#line 538 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].update_stmt);
        }
#line 3726 "parser.cpp"
        break;

        case 12: /* statement: insert_statement  */
#line 539 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].insert_stmt);
        }
#line 3732 "parser.cpp"
        break;

        case 13: /* statement: explain_statement  */
#line 540 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].explain_stmt);
        }
#line 3738 "parser.cpp"
        break;

        case 14: /* statement: flush_statement  */
#line 541 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].flush_stmt);
        }
#line 3744 "parser.cpp"
        break;

        case 15: /* statement: optimize_statement  */
#line 542 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].optimize_stmt);
        }
#line 3750 "parser.cpp"
        break;

        case 16: /* statement: command_statement  */
#line 543 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].command_stmt);
        }
#line 3756 "parser.cpp"
        break;

        case 17: /* statement: compact_statement  */
#line 544 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].compact_stmt);
        }
#line 3762 "parser.cpp"
        break;

        case 18: /* statement: admin_statement  */
#line 545 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].admin_stmt);
        }
#line 3768 "parser.cpp"
        break;

        case 19: /* statement: alter_statement  */
#line 546 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].alter_stmt);
        }
#line 3774 "parser.cpp"
        break;

        case 20: /* statement: check_statement  */
#line 547 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].check_stmt);
        }
#line 3780 "parser.cpp"
        break;

        case 21: /* explainable_statement: create_statement  */
#line 549 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].create_stmt);
        }
#line 3786 "parser.cpp"
        break;

        case 22: /* explainable_statement: drop_statement  */
#line 550 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].drop_stmt);
        }
#line 3792 "parser.cpp"
        break;

        case 23: /* explainable_statement: copy_statement  */
#line 551 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].copy_stmt);
        }
#line 3798 "parser.cpp"
        break;

        case 24: /* explainable_statement: show_statement  */
#line 552 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].show_stmt);
        }
#line 3804 "parser.cpp"
        break;

        case 25: /* explainable_statement: select_statement  */
#line 553 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].select_stmt);
        }
#line 3810 "parser.cpp"
        break;

        case 26: /* explainable_statement: delete_statement  */
#line 554 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].delete_stmt);
        }
#line 3816 "parser.cpp"
        break;

        case 27: /* explainable_statement: update_statement  */
#line 555 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].update_stmt);
        }
#line 3822 "parser.cpp"
        break;

        case 28: /* explainable_statement: insert_statement  */
#line 556 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].insert_stmt);
        }
#line 3828 "parser.cpp"
        break;

        case 29: /* explainable_statement: flush_statement  */
#line 557 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].flush_stmt);
        }
#line 3834 "parser.cpp"
        break;

        case 30: /* explainable_statement: optimize_statement  */
#line 558 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].optimize_stmt);
        }
#line 3840 "parser.cpp"
        break;

        case 31: /* explainable_statement: command_statement  */
#line 559 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].command_stmt);
        }
#line 3846 "parser.cpp"
        break;

        case 32: /* explainable_statement: compact_statement  */
#line 560 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].compact_stmt);
        }
#line 3852 "parser.cpp"
        break;

        case 33: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER COMMENT STRING  */
#line 567 "parser.y"
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
#line 3874 "parser.cpp"
        break;

        case 34: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 584 "parser.y"
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
#line 3894 "parser.cpp"
        break;

        case 35: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 601 "parser.y"
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
#line 3912 "parser.cpp"
        break;

        case 36: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 617 "parser.y"
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
#line 3945 "parser.cpp"
        break;

        case 37: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 646 "parser.y"
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
#line 3965 "parser.cpp"
        break;

        case 38: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list COMMENT STRING
                  */
#line 661 "parser.y"
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
#line 4001 "parser.cpp"
        break;

        case 39: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement COMMENT STRING  */
#line 693 "parser.y"
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
#line 4023 "parser.cpp"
        break;

        case 40: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 711 "parser.y"
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
#line 4044 "parser.cpp"
        break;

        case 41: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info  */
#line 729 "parser.y"
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
#line 4077 "parser.cpp"
        break;

        case 42: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info COMMENT STRING  */
#line 757 "parser.y"
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
#line 4112 "parser.cpp"
        break;

        case 43: /* table_element_array: table_element  */
#line 788 "parser.y"
        {
            (yyval.table_element_array_t) = new std::vector<infinity::TableElement *>();
            (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
        }
#line 4121 "parser.cpp"
        break;

        case 44: /* table_element_array: table_element_array ',' table_element  */
#line 792 "parser.y"
        {
            (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
            (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
        }
#line 4130 "parser.cpp"
        break;

        case 45: /* column_def_array: table_column  */
#line 797 "parser.y"
        {
            (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef *>();
            (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
        }
#line 4139 "parser.cpp"
        break;

        case 46: /* column_def_array: column_def_array ',' table_column  */
#line 801 "parser.y"
        {
            (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
            (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
        }
#line 4148 "parser.cpp"
        break;

        case 47: /* table_element: table_column  */
#line 807 "parser.y"
        {
            (yyval.table_element_t) = (yyvsp[0].table_column_t);
        }
#line 4156 "parser.cpp"
        break;

        case 48: /* table_element: table_constraint  */
#line 810 "parser.y"
        {
            (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
        }
#line 4164 "parser.cpp"
        break;

        case 49: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 817 "parser.y"
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
#line 4189 "parser.cpp"
        break;

        case 50: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 837 "parser.y"
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
#line 4216 "parser.cpp"
        break;

        case 51: /* table_column: IDENTIFIER column_type with_index_param_list default_expr COMMENT STRING  */
#line 859 "parser.y"
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
#line 4245 "parser.cpp"
        break;

        case 52: /* table_column: IDENTIFIER column_type column_constraints default_expr COMMENT STRING  */
#line 883 "parser.y"
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
#line 4275 "parser.cpp"
        break;

        case 53: /* column_type_array: column_type  */
#line 909 "parser.y"
        {
            (yyval.column_type_array_t) = new std::vector<std::unique_ptr<infinity::ColumnType>>();
            (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
        }
#line 4284 "parser.cpp"
        break;

        case 54: /* column_type_array: column_type_array ',' column_type  */
#line 913 "parser.y"
        {
            (yyval.column_type_array_t) = (yyvsp[-2].column_type_array_t);
            (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
        }
#line 4293 "parser.cpp"
        break;

        case 55: /* column_type: BOOLEAN  */
#line 919 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4299 "parser.cpp"
        break;

        case 56: /* column_type: JSON  */
#line 920 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kJson, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4305 "parser.cpp"
        break;

        case 57: /* column_type: TINYINT  */
#line 921 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4311 "parser.cpp"
        break;

        case 58: /* column_type: SMALLINT  */
#line 922 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4317 "parser.cpp"
        break;

        case 59: /* column_type: INTEGER  */
#line 923 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4323 "parser.cpp"
        break;

        case 60: /* column_type: INT  */
#line 924 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4329 "parser.cpp"
        break;

        case 61: /* column_type: BIGINT  */
#line 925 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4335 "parser.cpp"
        break;

        case 62: /* column_type: HUGEINT  */
#line 926 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4341 "parser.cpp"
        break;

        case 63: /* column_type: FLOAT  */
#line 927 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4347 "parser.cpp"
        break;

        case 64: /* column_type: REAL  */
#line 928 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4353 "parser.cpp"
        break;

        case 65: /* column_type: DOUBLE  */
#line 929 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4359 "parser.cpp"
        break;

        case 66: /* column_type: FLOAT16  */
#line 930 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4365 "parser.cpp"
        break;

        case 67: /* column_type: BFLOAT16  */
#line 931 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4371 "parser.cpp"
        break;

        case 68: /* column_type: DATE  */
#line 932 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4377 "parser.cpp"
        break;

        case 69: /* column_type: TIME  */
#line 933 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4383 "parser.cpp"
        break;

        case 70: /* column_type: DATETIME  */
#line 934 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4389 "parser.cpp"
        break;

        case 71: /* column_type: TIMESTAMP  */
#line 935 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4395 "parser.cpp"
        break;

        case 72: /* column_type: UUID  */
#line 936 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4401 "parser.cpp"
        break;

        case 73: /* column_type: POINT  */
#line 937 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4407 "parser.cpp"
        break;

        case 74: /* column_type: LINE  */
#line 938 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4413 "parser.cpp"
        break;

        case 75: /* column_type: LSEG  */
#line 939 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4419 "parser.cpp"
        break;

        case 76: /* column_type: BOX  */
#line 940 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4425 "parser.cpp"
        break;

        case 77: /* column_type: CIRCLE  */
#line 943 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4431 "parser.cpp"
        break;

        case 78: /* column_type: VARCHAR  */
#line 945 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4437 "parser.cpp"
        break;

        case 79: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 946 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal,
                                                             0,
                                                             (yyvsp[-3].long_value),
                                                             (yyvsp[-1].long_value),
                                                             infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4443 "parser.cpp"
        break;

        case 80: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 947 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4449 "parser.cpp"
        break;

        case 81: /* column_type: DECIMAL  */
#line 948 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4455 "parser.cpp"
        break;

        case 82: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 951 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4461 "parser.cpp"
        break;

        case 83: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 952 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4467 "parser.cpp"
        break;

        case 84: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 953 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4473 "parser.cpp"
        break;

        case 85: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 954 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4479 "parser.cpp"
        break;

        case 86: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 955 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4485 "parser.cpp"
        break;

        case 87: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 956 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4491 "parser.cpp"
        break;

        case 88: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 957 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4497 "parser.cpp"
        break;

        case 89: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 958 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4503 "parser.cpp"
        break;

        case 90: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 959 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4509 "parser.cpp"
        break;

        case 91: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 960 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4515 "parser.cpp"
        break;

        case 92: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 961 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4521 "parser.cpp"
        break;

        case 93: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 962 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4527 "parser.cpp"
        break;

        case 94: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 963 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4533 "parser.cpp"
        break;

        case 95: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 964 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4539 "parser.cpp"
        break;

        case 96: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 965 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4545 "parser.cpp"
        break;

        case 97: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 966 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4551 "parser.cpp"
        break;

        case 98: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 967 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4557 "parser.cpp"
        break;

        case 99: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 968 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4563 "parser.cpp"
        break;

        case 100: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 969 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4569 "parser.cpp"
        break;

        case 101: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 970 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector,
                                                             (yyvsp[-1].long_value),
                                                             0,
                                                             0,
                                                             infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4575 "parser.cpp"
        break;

        case 102: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 971 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector,
                                                             (yyvsp[-1].long_value),
                                                             0,
                                                             0,
                                                             infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4581 "parser.cpp"
        break;

        case 103: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 972 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4587 "parser.cpp"
        break;

        case 104: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 973 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4593 "parser.cpp"
        break;

        case 105: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 974 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4599 "parser.cpp"
        break;

        case 106: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 975 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4605 "parser.cpp"
        break;

        case 107: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 976 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4611 "parser.cpp"
        break;

        case 108: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 977 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4617 "parser.cpp"
        break;

        case 109: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 978 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4623 "parser.cpp"
        break;

        case 110: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 979 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4629 "parser.cpp"
        break;

        case 111: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 980 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4635 "parser.cpp"
        break;

        case 112: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 981 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4641 "parser.cpp"
        break;

        case 113: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 982 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4647 "parser.cpp"
        break;

        case 114: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 983 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4653 "parser.cpp"
        break;

        case 115: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 984 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4659 "parser.cpp"
        break;

        case 116: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 985 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4665 "parser.cpp"
        break;

        case 117: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 986 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4671 "parser.cpp"
        break;

        case 118: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 987 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4677 "parser.cpp"
        break;

        case 119: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 988 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4683 "parser.cpp"
        break;

        case 120: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 989 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4689 "parser.cpp"
        break;

        case 121: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 990 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4695 "parser.cpp"
        break;

        case 122: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 991 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4701 "parser.cpp"
        break;

        case 123: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 992 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray,
                                                             (yyvsp[-1].long_value),
                                                             0,
                                                             0,
                                                             infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4707 "parser.cpp"
        break;

        case 124: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 993 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray,
                                                             (yyvsp[-1].long_value),
                                                             0,
                                                             0,
                                                             infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4713 "parser.cpp"
        break;

        case 125: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 994 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4719 "parser.cpp"
        break;

        case 126: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 995 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4725 "parser.cpp"
        break;

        case 127: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 996 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4731 "parser.cpp"
        break;

        case 128: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 997 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4737 "parser.cpp"
        break;

        case 129: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 998 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4743 "parser.cpp"
        break;

        case 130: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 999 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4749 "parser.cpp"
        break;

        case 131: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1000 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4755 "parser.cpp"
        break;

        case 132: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1001 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4761 "parser.cpp"
        break;

        case 133: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1002 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4767 "parser.cpp"
        break;

        case 134: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1003 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4773 "parser.cpp"
        break;

        case 135: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1004 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4779 "parser.cpp"
        break;

        case 136: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1005 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4785 "parser.cpp"
        break;

        case 137: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1006 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4791 "parser.cpp"
        break;

        case 138: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1007 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4797 "parser.cpp"
        break;

        case 139: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1008 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4803 "parser.cpp"
        break;

        case 140: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1009 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4809 "parser.cpp"
        break;

        case 141: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4815 "parser.cpp"
        break;

        case 142: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1011 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4821 "parser.cpp"
        break;

        case 143: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1012 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4827 "parser.cpp"
        break;

        case 144: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1013 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4833 "parser.cpp"
        break;

        case 145: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1014 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4839 "parser.cpp"
        break;

        case 146: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4845 "parser.cpp"
        break;

        case 147: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4851 "parser.cpp"
        break;

        case 148: /* column_type: ARRAY '(' column_type ')'  */
#line 1017 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kArray, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
            (yyval.column_type_t)->element_types_.emplace_back((yyvsp[-1].column_type_t));
        }
#line 4860 "parser.cpp"
        break;

        case 149: /* column_type: TUPLE '(' column_type_array ')'  */
#line 1021 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTuple, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
            (yyval.column_type_t)->element_types_ = std::move(*((yyvsp[-1].column_type_array_t)));
            delete (yyvsp[-1].column_type_array_t);
        }
#line 4870 "parser.cpp"
        break;

        case 150: /* column_constraints: column_constraint  */
#line 1044 "parser.y"
        {
            (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
            (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
        }
#line 4879 "parser.cpp"
        break;

        case 151: /* column_constraints: column_constraints column_constraint  */
#line 1048 "parser.y"
        {
            if ((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
                yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
                delete (yyvsp[-1].column_constraints_t);
                YYERROR;
            }
            (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
            (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
        }
#line 4893 "parser.cpp"
        break;

        case 152: /* column_constraint: PRIMARY KEY  */
#line 1058 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
        }
#line 4901 "parser.cpp"
        break;

        case 153: /* column_constraint: UNIQUE  */
#line 1061 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
        }
#line 4909 "parser.cpp"
        break;

        case 154: /* column_constraint: NULLABLE  */
#line 1064 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
        }
#line 4917 "parser.cpp"
        break;

        case 155: /* column_constraint: NOT NULLABLE  */
#line 1067 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
        }
#line 4925 "parser.cpp"
        break;

        case 156: /* default_expr: DEFAULT constant_expr  */
#line 1071 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 4933 "parser.cpp"
        break;

        case 157: /* default_expr: %empty  */
#line 1074 "parser.y"
        {
            (yyval.const_expr_t) = nullptr;
        }
#line 4941 "parser.cpp"
        break;

        case 158: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1079 "parser.y"
        {
            (yyval.table_constraint_t) = new infinity::TableConstraint();
            (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
            (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
        }
#line 4951 "parser.cpp"
        break;

        case 159: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1084 "parser.y"
        {
            (yyval.table_constraint_t) = new infinity::TableConstraint();
            (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
            (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
        }
#line 4961 "parser.cpp"
        break;

        case 160: /* identifier_array: IDENTIFIER  */
#line 1091 "parser.y"
        {
            (yyval.identifier_array_t) = new std::vector<std::string>();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 4972 "parser.cpp"
        break;

        case 161: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1097 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
            (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
        }
#line 4983 "parser.cpp"
        break;

        case 162: /* delete_statement: DELETE FROM table_name where_clause  */
#line 1107 "parser.y"
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
#line 5000 "parser.cpp"
        break;

        case 163: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES insert_row_list  */
#line 1123 "parser.y"
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
#line 5043 "parser.cpp"
        break;

        case 164: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1161 "parser.y"
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
#line 5063 "parser.cpp"
        break;

        case 165: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1177 "parser.y"
        {
            (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
        }
#line 5071 "parser.cpp"
        break;

        case 166: /* optional_identifier_array: %empty  */
#line 1180 "parser.y"
        {
            (yyval.identifier_array_t) = nullptr;
        }
#line 5079 "parser.cpp"
        break;

        case 167: /* explain_statement: EXPLAIN IDENTIFIER explainable_statement  */
#line 1187 "parser.y"
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
#line 5102 "parser.cpp"
        break;

        case 168: /* explain_statement: EXPLAIN explainable_statement  */
#line 1204 "parser.y"
        {
            (yyval.explain_stmt) = new infinity::ExplainStatement();
            (yyval.explain_stmt)->type_ = infinity::ExplainType::kPhysical;
            (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
        }
#line 5112 "parser.cpp"
        break;

        case 169: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1213 "parser.y"
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
#line 5129 "parser.cpp"
        break;

        case 170: /* update_expr_array: update_expr  */
#line 1226 "parser.y"
        {
            (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr *>();
            (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
        }
#line 5138 "parser.cpp"
        break;

        case 171: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1230 "parser.y"
        {
            (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
            (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
        }
#line 5147 "parser.cpp"
        break;

        case 172: /* update_expr: IDENTIFIER '=' expr  */
#line 1235 "parser.y"
        {
            (yyval.update_expr_t) = new infinity::UpdateExpr();
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));
            (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
        }
#line 5159 "parser.cpp"
        break;

        case 173: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1248 "parser.y"
        {
            (yyval.drop_stmt) = new infinity::DropStatement();
            std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

            ParserHelper::ToLower((yyvsp[0].str_value));
            drop_schema_info->schema_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));

            (yyval.drop_stmt)->drop_info_ = drop_schema_info;
            (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
        }
#line 5175 "parser.cpp"
        break;

        case 174: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1261 "parser.y"
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
#line 5193 "parser.cpp"
        break;

        case 175: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1276 "parser.y"
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
#line 5211 "parser.cpp"
        break;

        case 176: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1291 "parser.y"
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
#line 5229 "parser.cpp"
        break;

        case 177: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1306 "parser.y"
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
#line 5252 "parser.cpp"
        break;

        case 178: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1329 "parser.y"
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
#line 5310 "parser.cpp"
        break;

        case 179: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1382 "parser.y"
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
#line 5370 "parser.cpp"
        break;

        case 180: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1437 "parser.y"
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
#line 5422 "parser.cpp"
        break;

        case 181: /* select_statement: select_without_paren  */
#line 1488 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[0].select_stmt);
        }
#line 5430 "parser.cpp"
        break;

        case 182: /* select_statement: select_with_paren  */
#line 1491 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[0].select_stmt);
        }
#line 5438 "parser.cpp"
        break;

        case 183: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1494 "parser.y"
        {
            infinity::SelectStatement *node = (yyvsp[-2].select_stmt);
            while (node->nested_select_ != nullptr) {
                node = node->nested_select_;
            }
            node->set_op_ = (yyvsp[-1].set_operator_t);
            node->nested_select_ = (yyvsp[0].select_stmt);
            (yyval.select_stmt) = (yyvsp[-2].select_stmt);
        }
#line 5452 "parser.cpp"
        break;

        case 184: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1503 "parser.y"
        {
            infinity::SelectStatement *node = (yyvsp[-2].select_stmt);
            while (node->nested_select_ != nullptr) {
                node = node->nested_select_;
            }
            node->set_op_ = (yyvsp[-1].set_operator_t);
            node->nested_select_ = (yyvsp[0].select_stmt);
            (yyval.select_stmt) = (yyvsp[-2].select_stmt);
        }
#line 5466 "parser.cpp"
        break;

        case 185: /* select_with_paren: '(' select_without_paren ')'  */
#line 1513 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5474 "parser.cpp"
        break;

        case 186: /* select_with_paren: '(' select_with_paren ')'  */
#line 1516 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5482 "parser.cpp"
        break;

        case 187: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1520 "parser.y"
        {
            (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
            (yyval.select_stmt) = (yyvsp[0].select_stmt);
        }
#line 5491 "parser.cpp"
        break;

        case 188: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1525 "parser.y"
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
#line 5522 "parser.cpp"
        break;

        case 189: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1552 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5530 "parser.cpp"
        break;

        case 190: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1555 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5538 "parser.cpp"
        break;

        case 191: /* select_clause_without_modifier: SELECT distinct expr_array highlight_clause from_clause search_clause where_clause
                     group_by_clause having_clause  */
#line 1560 "parser.y"
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
#line 5559 "parser.cpp"
        break;

        case 192: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1577 "parser.y"
        {
            (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
        }
#line 5567 "parser.cpp"
        break;

        case 193: /* order_by_clause: %empty  */
#line 1580 "parser.y"
        {
            (yyval.order_by_expr_list_t) = nullptr;
        }
#line 5575 "parser.cpp"
        break;

        case 194: /* order_by_expr_list: order_by_expr  */
#line 1584 "parser.y"
        {
            (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr *>();
            (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
        }
#line 5584 "parser.cpp"
        break;

        case 195: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1588 "parser.y"
        {
            (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
            (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
        }
#line 5593 "parser.cpp"
        break;

        case 196: /* order_by_expr: expr order_by_type  */
#line 1593 "parser.y"
        {
            (yyval.order_by_expr_t) = new infinity::OrderByExpr();
            (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
            (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
        }
#line 5603 "parser.cpp"
        break;

        case 197: /* order_by_type: ASC  */
#line 1599 "parser.y"
        {
            (yyval.order_by_type_t) = infinity::kAsc;
        }
#line 5611 "parser.cpp"
        break;

        case 198: /* order_by_type: DESC  */
#line 1602 "parser.y"
        {
            (yyval.order_by_type_t) = infinity::kDesc;
        }
#line 5619 "parser.cpp"
        break;

        case 199: /* order_by_type: %empty  */
#line 1605 "parser.y"
        {
            (yyval.order_by_type_t) = infinity::kAsc;
        }
#line 5627 "parser.cpp"
        break;

        case 200: /* limit_expr: LIMIT expr  */
#line 1609 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5635 "parser.cpp"
        break;

        case 201: /* limit_expr: %empty  */
#line 1613 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5641 "parser.cpp"
        break;

        case 202: /* offset_expr: OFFSET expr  */
#line 1615 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5649 "parser.cpp"
        break;

        case 203: /* offset_expr: %empty  */
#line 1619 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5655 "parser.cpp"
        break;

        case 204: /* distinct: DISTINCT  */
#line 1621 "parser.y"
        {
            (yyval.bool_value) = true;
        }
#line 5663 "parser.cpp"
        break;

        case 205: /* distinct: %empty  */
#line 1624 "parser.y"
        {
            (yyval.bool_value) = false;
        }
#line 5671 "parser.cpp"
        break;

        case 206: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1628 "parser.y"
        {
            (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
        }
#line 5679 "parser.cpp"
        break;

        case 207: /* highlight_clause: %empty  */
#line 1631 "parser.y"
        {
            (yyval.expr_array_t) = nullptr;
        }
#line 5687 "parser.cpp"
        break;

        case 208: /* from_clause: FROM table_reference  */
#line 1635 "parser.y"
        {
            (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
        }
#line 5695 "parser.cpp"
        break;

        case 209: /* from_clause: %empty  */
#line 1638 "parser.y"
        {
            (yyval.table_reference_t) = nullptr;
        }
#line 5703 "parser.cpp"
        break;

        case 210: /* search_clause: SEARCH sub_search_array  */
#line 1642 "parser.y"
        {
            infinity::SearchExpr *search_expr = new infinity::SearchExpr();
            search_expr->SetExprs((yyvsp[0].expr_array_t));
            (yyval.expr_t) = search_expr;
        }
#line 5713 "parser.cpp"
        break;

        case 211: /* search_clause: %empty  */
#line 1647 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5721 "parser.cpp"
        break;

        case 212: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1651 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5729 "parser.cpp"
        break;

        case 213: /* optional_search_filter_expr: %empty  */
#line 1654 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5737 "parser.cpp"
        break;

        case 214: /* where_clause: WHERE expr  */
#line 1658 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5745 "parser.cpp"
        break;

        case 215: /* where_clause: %empty  */
#line 1661 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5753 "parser.cpp"
        break;

        case 216: /* having_clause: HAVING expr  */
#line 1665 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5761 "parser.cpp"
        break;

        case 217: /* having_clause: %empty  */
#line 1668 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5769 "parser.cpp"
        break;

        case 218: /* group_by_clause: GROUP BY expr_array  */
#line 1672 "parser.y"
        {
            (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
        }
#line 5777 "parser.cpp"
        break;

        case 219: /* group_by_clause: %empty  */
#line 1675 "parser.y"
        {
            (yyval.expr_array_t) = nullptr;
        }
#line 5785 "parser.cpp"
        break;

        case 220: /* set_operator: UNION  */
#line 1679 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
        }
#line 5793 "parser.cpp"
        break;

        case 221: /* set_operator: UNION ALL  */
#line 1682 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
        }
#line 5801 "parser.cpp"
        break;

        case 222: /* set_operator: INTERSECT  */
#line 1685 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
        }
#line 5809 "parser.cpp"
        break;

        case 223: /* set_operator: EXCEPT  */
#line 1688 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
        }
#line 5817 "parser.cpp"
        break;

        case 224: /* table_reference: table_reference_unit  */
#line 1696 "parser.y"
        {
            (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
        }
#line 5825 "parser.cpp"
        break;

        case 225: /* table_reference: table_reference ',' table_reference_unit  */
#line 1699 "parser.y"
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
#line 5843 "parser.cpp"
        break;

        case 228: /* table_reference_name: table_name table_alias  */
#line 1716 "parser.y"
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
#line 5861 "parser.cpp"
        break;

        case 229: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1730 "parser.y"
        {
            infinity::SubqueryReference *subquery_reference = new infinity::SubqueryReference();
            subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
            subquery_reference->alias_ = (yyvsp[0].table_alias_t);
            (yyval.table_reference_t) = subquery_reference;
        }
#line 5872 "parser.cpp"
        break;

        case 230: /* table_name: IDENTIFIER  */
#line 1739 "parser.y"
        {
            (yyval.table_name_t) = new infinity::TableName();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
        }
#line 5882 "parser.cpp"
        break;

        case 231: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1744 "parser.y"
        {
            (yyval.table_name_t) = new infinity::TableName();
            ParserHelper::ToLower((yyvsp[-2].str_value));
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
            (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
        }
#line 5894 "parser.cpp"
        break;

        case 232: /* table_alias: AS IDENTIFIER  */
#line 1753 "parser.y"
        {
            (yyval.table_alias_t) = new infinity::TableAlias();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
        }
#line 5904 "parser.cpp"
        break;

        case 233: /* table_alias: IDENTIFIER  */
#line 1758 "parser.y"
        {
            (yyval.table_alias_t) = new infinity::TableAlias();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
        }
#line 5914 "parser.cpp"
        break;

        case 234: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1763 "parser.y"
        {
            (yyval.table_alias_t) = new infinity::TableAlias();
            ParserHelper::ToLower((yyvsp[-3].str_value));
            (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
            (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
        }
#line 5925 "parser.cpp"
        break;

        case 235: /* table_alias: %empty  */
#line 1769 "parser.y"
        {
            (yyval.table_alias_t) = nullptr;
        }
#line 5933 "parser.cpp"
        break;

        case 236: /* with_clause: WITH with_expr_list  */
#line 1776 "parser.y"
        {
            (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
        }
#line 5941 "parser.cpp"
        break;

        case 237: /* with_clause: %empty  */
#line 1779 "parser.y"
        {
            (yyval.with_expr_list_t) = nullptr;
        }
#line 5949 "parser.cpp"
        break;

        case 238: /* with_expr_list: with_expr  */
#line 1783 "parser.y"
        {
            (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr *>();
            (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
        }
#line 5958 "parser.cpp"
        break;

        case 239: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1786 "parser.y"
        {
            (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
            (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
        }
#line 5967 "parser.cpp"
        break;

        case 240: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1791 "parser.y"
        {
            (yyval.with_expr_t) = new infinity::WithExpr();
            ParserHelper::ToLower((yyvsp[-4].str_value));
            (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
            free((yyvsp[-4].str_value));
            (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
        }
#line 5979 "parser.cpp"
        break;

        case 241: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1803 "parser.y"
        {
            infinity::JoinReference *join_reference = new infinity::JoinReference();
            join_reference->left_ = (yyvsp[-3].table_reference_t);
            join_reference->right_ = (yyvsp[0].table_reference_t);
            join_reference->join_type_ = infinity::JoinType::kNatural;
            (yyval.table_reference_t) = join_reference;
        }
#line 5991 "parser.cpp"
        break;

        case 242: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1810 "parser.y"
        {
            infinity::JoinReference *join_reference = new infinity::JoinReference();
            join_reference->left_ = (yyvsp[-5].table_reference_t);
            join_reference->right_ = (yyvsp[-2].table_reference_t);
            join_reference->join_type_ = (yyvsp[-4].join_type_t);
            join_reference->condition_ = (yyvsp[0].expr_t);
            (yyval.table_reference_t) = join_reference;
        }
#line 6004 "parser.cpp"
        break;

        case 243: /* join_type: INNER  */
#line 1824 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kInner;
        }
#line 6012 "parser.cpp"
        break;

        case 244: /* join_type: LEFT  */
#line 1827 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kLeft;
        }
#line 6020 "parser.cpp"
        break;

        case 245: /* join_type: RIGHT  */
#line 1830 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kRight;
        }
#line 6028 "parser.cpp"
        break;

        case 246: /* join_type: OUTER  */
#line 1833 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kFull;
        }
#line 6036 "parser.cpp"
        break;

        case 247: /* join_type: FULL  */
#line 1836 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kFull;
        }
#line 6044 "parser.cpp"
        break;

        case 248: /* join_type: CROSS  */
#line 1839 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kCross;
        }
#line 6052 "parser.cpp"
        break;

        case 249: /* join_type: %empty  */
#line 1842 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kInner;
        }
#line 6060 "parser.cpp"
        break;

        case 250: /* show_statement: SHOW DATABASES  */
#line 1849 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
        }
#line 6069 "parser.cpp"
        break;

        case 251: /* show_statement: SHOW TABLES  */
#line 1853 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
        }
#line 6078 "parser.cpp"
        break;

        case 252: /* show_statement: SHOW TASKS  */
#line 1857 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTasks;
        }
#line 6087 "parser.cpp"
        break;

        case 253: /* show_statement: SHOW CONFIGS  */
#line 1861 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
        }
#line 6096 "parser.cpp"
        break;

        case 254: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1865 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 6108 "parser.cpp"
        break;

        case 255: /* show_statement: SHOW PROFILES  */
#line 1872 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
        }
#line 6117 "parser.cpp"
        break;

        case 256: /* show_statement: SHOW BUFFER  */
#line 1876 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
        }
#line 6126 "parser.cpp"
        break;

        case 257: /* show_statement: SHOW MEMINDEX  */
#line 1880 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
        }
#line 6135 "parser.cpp"
        break;

        case 258: /* show_statement: SHOW QUERIES  */
#line 1884 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
        }
#line 6144 "parser.cpp"
        break;

        case 259: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1888 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
            (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
        }
#line 6154 "parser.cpp"
        break;

        case 260: /* show_statement: SHOW TRANSACTIONS  */
#line 1893 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
        }
#line 6163 "parser.cpp"
        break;

        case 261: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1897 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
            (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
        }
#line 6173 "parser.cpp"
        break;

        case 262: /* show_statement: SHOW TRANSACTION HISTORY  */
#line 1902 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactionHistory;
        }
#line 6182 "parser.cpp"
        break;

        case 263: /* show_statement: SHOW SESSION VARIABLES  */
#line 1906 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
        }
#line 6191 "parser.cpp"
        break;

        case 264: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1910 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
        }
#line 6200 "parser.cpp"
        break;

        case 265: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1914 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
            (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 6211 "parser.cpp"
        break;

        case 266: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1920 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
            (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 6222 "parser.cpp"
        break;

        case 267: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1926 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 6234 "parser.cpp"
        break;

        case 268: /* show_statement: SHOW TABLE table_name  */
#line 1933 "parser.y"
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
#line 6250 "parser.cpp"
        break;

        case 269: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1944 "parser.y"
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
#line 6266 "parser.cpp"
        break;

        case 270: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1955 "parser.y"
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
#line 6282 "parser.cpp"
        break;

        case 271: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1966 "parser.y"
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
#line 6299 "parser.cpp"
        break;

        case 272: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1978 "parser.y"
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
#line 6316 "parser.cpp"
        break;

        case 273: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1990 "parser.y"
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
#line 6334 "parser.cpp"
        break;

        case 274: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 2003 "parser.y"
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
#line 6353 "parser.cpp"
        break;

        case 275: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 2017 "parser.y"
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
#line 6369 "parser.cpp"
        break;

        case 276: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 2028 "parser.y"
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
#line 6389 "parser.cpp"
        break;

        case 277: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
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

            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.show_stmt)->index_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));

            (yyval.show_stmt)->segment_id_ = (yyvsp[0].long_value);
        }
#line 6411 "parser.cpp"
        break;

        case 278: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNKS  */
#line 2060 "parser.y"
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
#line 6433 "parser.cpp"
        break;

        case 279: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 2077 "parser.y"
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
#line 6456 "parser.cpp"
        break;

        case 280: /* show_statement: SHOW LOGS  */
#line 2095 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
        }
#line 6465 "parser.cpp"
        break;

        case 281: /* show_statement: SHOW CATALOG  */
#line 2099 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalog;
        }
#line 6474 "parser.cpp"
        break;

        case 282: /* show_statement: SHOW CATALOG STRING  */
#line 2103 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCatalogKey;
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.show_stmt)->var_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 6486 "parser.cpp"
        break;

        case 283: /* show_statement: SHOW CATALOG TO file_path  */
#line 2110 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogToFile;
            (yyval.show_stmt)->file_path_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 6497 "parser.cpp"
        break;

        case 284: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2116 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
        }
#line 6506 "parser.cpp"
        break;

        case 285: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2120 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
        }
#line 6515 "parser.cpp"
        break;

        case 286: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2124 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
            (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 6526 "parser.cpp"
        break;

        case 287: /* show_statement: SHOW MEMORY  */
#line 2130 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
        }
#line 6535 "parser.cpp"
        break;

        case 288: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2134 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
        }
#line 6544 "parser.cpp"
        break;

        case 289: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2138 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
        }
#line 6553 "parser.cpp"
        break;

        case 290: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2142 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));
        }
#line 6565 "parser.cpp"
        break;

        case 291: /* show_statement: SHOW SNAPSHOTS  */
#line 2149 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListSnapshots;
        }
#line 6574 "parser.cpp"
        break;

        case 292: /* show_statement: SHOW SNAPSHOT IDENTIFIER  */
#line 2153 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowSnapshot;
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.show_stmt)->snapshot_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 6586 "parser.cpp"
        break;

        case 293: /* show_statement: SHOW CACHES  */
#line 2160 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCaches;
        }
#line 6595 "parser.cpp"
        break;

        case 294: /* show_statement: SHOW CACHE  */
#line 2164 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowCache;
        }
#line 6604 "parser.cpp"
        break;

        case 295: /* show_statement: SHOW COMPACT  */
#line 2168 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCompact;
        }
#line 6613 "parser.cpp"
        break;

        case 296: /* show_statement: SHOW COMPACT NOT NULLABLE  */
#line 2172 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCompact;
            (yyval.show_stmt)->show_nullable_ = false;
        }
#line 6623 "parser.cpp"
        break;

        case 297: /* show_statement: SHOW CHECKPOINT  */
#line 2177 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCheckpoint;
        }
#line 6632 "parser.cpp"
        break;

        case 298: /* show_statement: SHOW CHECKPOINT NOT NULLABLE  */
#line 2181 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCheckpoint;
            (yyval.show_stmt)->show_nullable_ = false;
        }
#line 6642 "parser.cpp"
        break;

        case 299: /* show_statement: SHOW CHECKPOINT LONG_VALUE  */
#line 2186 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowCheckpoint;
            (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
        }
#line 6652 "parser.cpp"
        break;

        case 300: /* show_statement: SHOW OPTIMIZE  */
#line 2191 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListOptimize;
        }
#line 6661 "parser.cpp"
        break;

        case 301: /* show_statement: SHOW OPTIMIZE NOT NULLABLE  */
#line 2195 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListOptimize;
            (yyval.show_stmt)->show_nullable_ = false;
        }
#line 6671 "parser.cpp"
        break;

        case 302: /* show_statement: SHOW IMPORT  */
#line 2200 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListImport;
        }
#line 6680 "parser.cpp"
        break;

        case 303: /* show_statement: SHOW CLEAN  */
#line 2204 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListClean;
        }
#line 6689 "parser.cpp"
        break;

        case 304: /* show_statement: SHOW CLEAN NOT NULLABLE  */
#line 2208 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListClean;
            (yyval.show_stmt)->show_nullable_ = false;
        }
#line 6699 "parser.cpp"
        break;

        case 305: /* show_statement: SHOW CLEAN LONG_VALUE  */
#line 2213 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowClean;
            (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
        }
#line 6709 "parser.cpp"
        break;

        case 306: /* flush_statement: FLUSH DATA  */
#line 2222 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
        }
#line 6718 "parser.cpp"
        break;

        case 307: /* flush_statement: FLUSH LOG  */
#line 2226 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
        }
#line 6727 "parser.cpp"
        break;

        case 308: /* flush_statement: FLUSH BUFFER  */
#line 2230 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
        }
#line 6736 "parser.cpp"
        break;

        case 309: /* flush_statement: FLUSH CATALOG  */
#line 2234 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kCatalog;
        }
#line 6745 "parser.cpp"
        break;

        case 310: /* optimize_statement: OPTIMIZE table_name  */
#line 2242 "parser.y"
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
#line 6760 "parser.cpp"
        break;

        case 311: /* command_statement: USE IDENTIFIER  */
#line 2256 "parser.y"
        {
            (yyval.command_stmt) = new infinity::CommandStatement();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 6771 "parser.cpp"
        break;

        case 312: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2262 "parser.y"
        {
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
            free((yyvsp[0].str_value));
        }
#line 6781 "parser.cpp"
        break;

        case 313: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2267 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
            free((yyvsp[-1].str_value));
        }
#line 6792 "parser.cpp"
        break;

        case 314: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2273 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
            free((yyvsp[-1].str_value));
        }
#line 6803 "parser.cpp"
        break;

        case 315: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2279 "parser.y"
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
#line 6816 "parser.cpp"
        break;

        case 316: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2287 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession,
                                                                                     infinity::SetVarType::kInteger,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].long_value));
            free((yyvsp[-1].str_value));
        }
#line 6827 "parser.cpp"
        break;

        case 317: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2293 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession,
                                                                                     infinity::SetVarType::kDouble,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].double_value));
            free((yyvsp[-1].str_value));
        }
#line 6838 "parser.cpp"
        break;

        case 318: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2299 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
            free((yyvsp[-1].str_value));
        }
#line 6849 "parser.cpp"
        break;

        case 319: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2305 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
            free((yyvsp[-1].str_value));
        }
#line 6860 "parser.cpp"
        break;

        case 320: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2311 "parser.y"
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
#line 6873 "parser.cpp"
        break;

        case 321: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2319 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal,
                                                                                     infinity::SetVarType::kInteger,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].long_value));
            free((yyvsp[-1].str_value));
        }
#line 6884 "parser.cpp"
        break;

        case 322: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2325 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal,
                                                                                     infinity::SetVarType::kDouble,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].double_value));
            free((yyvsp[-1].str_value));
        }
#line 6895 "parser.cpp"
        break;

        case 323: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2331 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
            free((yyvsp[-1].str_value));
        }
#line 6906 "parser.cpp"
        break;

        case 324: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2337 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
            free((yyvsp[-1].str_value));
        }
#line 6917 "parser.cpp"
        break;

        case 325: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2343 "parser.y"
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
#line 6930 "parser.cpp"
        break;

        case 326: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2351 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig,
                                                                                     infinity::SetVarType::kInteger,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].long_value));
            free((yyvsp[-1].str_value));
        }
#line 6941 "parser.cpp"
        break;

        case 327: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2357 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig,
                                                                                     infinity::SetVarType::kDouble,
                                                                                     (yyvsp[-1].str_value),
                                                                                     (yyvsp[0].double_value));
            free((yyvsp[-1].str_value));
        }
#line 6952 "parser.cpp"
        break;

        case 328: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON TABLE IDENTIFIER  */
#line 2363 "parser.y"
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
#line 6965 "parser.cpp"
        break;

        case 329: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON DATABASE IDENTIFIER  */
#line 2371 "parser.y"
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
#line 6978 "parser.cpp"
        break;

        case 330: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON SYSTEM  */
#line 2379 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SnapshotCmd>((yyvsp[-2].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kSystem);
            free((yyvsp[-2].str_value));
        }
#line 6989 "parser.cpp"
        break;

        case 331: /* command_statement: DROP SNAPSHOT IDENTIFIER  */
#line 2385 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kDrop, infinity::SnapshotScope::kIgnore);
            free((yyvsp[0].str_value));
        }
#line 7000 "parser.cpp"
        break;

        case 332: /* command_statement: RESTORE SYSTEM SNAPSHOT IDENTIFIER  */
#line 2391 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kSystem);
            free((yyvsp[0].str_value));
        }
#line 7011 "parser.cpp"
        break;

        case 333: /* command_statement: RESTORE DATABASE SNAPSHOT IDENTIFIER  */
#line 2397 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kDatabase);
            free((yyvsp[0].str_value));
        }
#line 7022 "parser.cpp"
        break;

        case 334: /* command_statement: RESTORE TABLE SNAPSHOT IDENTIFIER  */
#line 2403 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kTable);
            free((yyvsp[0].str_value));
        }
#line 7033 "parser.cpp"
        break;

        case 335: /* command_statement: CLEAN DATA  */
#line 2409 "parser.y"
        {
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::CleanupCmd>();
        }
#line 7042 "parser.cpp"
        break;

        case 336: /* command_statement: DUMP INDEX IDENTIFIER ON table_name  */
#line 2413 "parser.y"
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
#line 7058 "parser.cpp"
        break;

        case 337: /* compact_statement: COMPACT TABLE table_name  */
#line 2425 "parser.y"
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
#line 7075 "parser.cpp"
        break;

        case 338: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2438 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
            (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
            (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
        }
#line 7086 "parser.cpp"
        break;

        case 339: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2444 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
            (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
            (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
            (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
        }
#line 7098 "parser.cpp"
        break;

        case 340: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2451 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
            (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
            (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
            (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
            (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
        }
#line 7111 "parser.cpp"
        break;

        case 341: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2459 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
            (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
            (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
            (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
            (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
            (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
        }
#line 7125 "parser.cpp"
        break;

        case 342: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2468 "parser.y"
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
#line 7140 "parser.cpp"
        break;

        case 343: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2478 "parser.y"
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
#line 7155 "parser.cpp"
        break;

        case 344: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT
                     LONG_VALUE  */
#line 2488 "parser.y"
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
#line 7171 "parser.cpp"
        break;

        case 345: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT
                     LONG_VALUE BLOCKS  */
#line 2499 "parser.y"
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
#line 7187 "parser.cpp"
        break;

        case 346: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT
                     LONG_VALUE BLOCK LONG_VALUE  */
#line 2510 "parser.y"
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
#line 7204 "parser.cpp"
        break;

        case 347: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT
                     LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2522 "parser.y"
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
#line 7221 "parser.cpp"
        break;

        case 348: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2534 "parser.y"
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
#line 7236 "parser.cpp"
        break;

        case 349: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX
                     LONG_VALUE  */
#line 2544 "parser.y"
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
#line 7252 "parser.cpp"
        break;

        case 350: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX
                     LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2555 "parser.y"
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
#line 7269 "parser.cpp"
        break;

        case 351: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX
                     LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2567 "parser.y"
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
#line 7287 "parser.cpp"
        break;

        case 352: /* admin_statement: ADMIN SHOW LOGS  */
#line 2580 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
        }
#line 7296 "parser.cpp"
        break;

        case 353: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2584 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
            (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
        }
#line 7306 "parser.cpp"
        break;

        case 354: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2589 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
            (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
        }
#line 7316 "parser.cpp"
        break;

        case 355: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2594 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
            (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
            (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
        }
#line 7327 "parser.cpp"
        break;

        case 356: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2600 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
        }
#line 7336 "parser.cpp"
        break;

        case 357: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2604 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
        }
#line 7345 "parser.cpp"
        break;

        case 358: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2608 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7357 "parser.cpp"
        break;

        case 359: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2615 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
        }
#line 7366 "parser.cpp"
        break;

        case 360: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2619 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
        }
#line 7375 "parser.cpp"
        break;

        case 361: /* admin_statement: ADMIN SHOW SNAPSHOT IDENTIFIER  */
#line 2623 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSnapshot;
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7387 "parser.cpp"
        break;

        case 362: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2630 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
            (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7398 "parser.cpp"
        break;

        case 363: /* admin_statement: ADMIN EXPORT SNAPSHOT STRING TO STRING  */
#line 2636 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kExportSnapshot;
            (yyval.admin_stmt)->snapshot_name_ = (yyvsp[-2].str_value);
            (yyval.admin_stmt)->export_path_ = (yyvsp[0].str_value);
            free((yyvsp[-2].str_value));
            free((yyvsp[0].str_value));
        }
#line 7411 "parser.cpp"
        break;

        case 364: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2644 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
            (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7422 "parser.cpp"
        break;

        case 365: /* admin_statement: ADMIN SHOW NODES  */
#line 2650 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
        }
#line 7431 "parser.cpp"
        break;

        case 366: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2654 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
            (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7442 "parser.cpp"
        break;

        case 367: /* admin_statement: ADMIN SHOW NODE  */
#line 2660 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
        }
#line 7451 "parser.cpp"
        break;

        case 368: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2664 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
            (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7462 "parser.cpp"
        break;

        case 369: /* admin_statement: ADMIN SET ADMIN  */
#line 2670 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
            (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
        }
#line 7472 "parser.cpp"
        break;

        case 370: /* admin_statement: ADMIN SET STANDALONE  */
#line 2675 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
            (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
        }
#line 7482 "parser.cpp"
        break;

        case 371: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2680 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
            (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLeader;
            (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7494 "parser.cpp"
        break;

        case 372: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2687 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
            (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kFollower;
            (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
            (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
            free((yyvsp[-4].str_value));
            free((yyvsp[0].str_value));
        }
#line 7508 "parser.cpp"
        break;

        case 373: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2696 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
            (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLearner;
            (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
            (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
            free((yyvsp[-4].str_value));
            free((yyvsp[0].str_value));
        }
#line 7522 "parser.cpp"
        break;

        case 374: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2706 "parser.y"
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
#line 7537 "parser.cpp"
        break;

        case 375: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2716 "parser.y"
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
#line 7554 "parser.cpp"
        break;

        case 376: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2728 "parser.y"
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
#line 7570 "parser.cpp"
        break;

        case 377: /* alter_statement: ALTER IDENTIFIER ON table_name with_index_param_list  */
#line 2739 "parser.y"
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
#line 7593 "parser.cpp"
        break;

        case 378: /* check_statement: CHECK SYSTEM  */
#line 2758 "parser.y"
        {
            (yyval.check_stmt) = new infinity::CheckStatement();
            (yyval.check_stmt)->check_type_ = infinity::CheckStmtType::kSystem;
        }
#line 7602 "parser.cpp"
        break;

        case 379: /* check_statement: CHECK TABLE table_name  */
#line 2762 "parser.y"
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
#line 7618 "parser.cpp"
        break;

        case 380: /* expr_array: expr_alias  */
#line 2778 "parser.y"
        {
            (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr *>();
            (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
        }
#line 7627 "parser.cpp"
        break;

        case 381: /* expr_array: expr_array ',' expr_alias  */
#line 2782 "parser.y"
        {
            (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
        }
#line 7636 "parser.cpp"
        break;

        case 382: /* insert_row_list: '(' expr_array ')'  */
#line 2787 "parser.y"
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
#line 7651 "parser.cpp"
        break;

        case 383: /* insert_row_list: insert_row_list ',' '(' expr_array ')'  */
#line 2797 "parser.y"
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
#line 7666 "parser.cpp"
        break;

        case 384: /* expr_alias: expr AS IDENTIFIER  */
#line 2819 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[-2].expr_t);
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7677 "parser.cpp"
        break;

        case 385: /* expr_alias: expr  */
#line 2825 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 7685 "parser.cpp"
        break;

        case 391: /* operand: '(' expr ')'  */
#line 2835 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[-1].expr_t);
        }
#line 7693 "parser.cpp"
        break;

        case 392: /* operand: '(' select_without_paren ')'  */
#line 2838 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 7704 "parser.cpp"
        break;

        case 393: /* operand: constant_expr  */
#line 2844 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].const_expr_t);
        }
#line 7712 "parser.cpp"
        break;

        case 404: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING
                     optional_search_filter_expr ')'  */
#line 2860 "parser.y"
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
#line 7732 "parser.cpp"
        break;

        case 405: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING
                     optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')'  */
#line 2876 "parser.y"
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
#line 7754 "parser.cpp"
        break;

        case 406: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING
                     optional_search_filter_expr ')' IGNORE INDEX  */
#line 2894 "parser.y"
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
#line 7775 "parser.cpp"
        break;

        case 407: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING
                     INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2912 "parser.y"
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
#line 7837 "parser.cpp"
        break;

        case 408: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE
                     INDEX  */
#line 2970 "parser.y"
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
#line 7891 "parser.cpp"
        break;

        case 409: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')'
                     with_index_param_list  */
#line 3020 "parser.y"
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
#line 7949 "parser.cpp"
        break;

        case 410: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3074 "parser.y"
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
#line 8008 "parser.cpp"
        break;

        case 411: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')'
                     USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 3132 "parser.y"
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
#line 8037 "parser.cpp"
        break;

        case 412: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')'
                     IGNORE INDEX  */
#line 3157 "parser.y"
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
#line 8064 "parser.cpp"
        break;

        case 413: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')'
                     with_index_param_list  */
#line 3180 "parser.y"
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
#line 8089 "parser.cpp"
        break;

        case 414: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')'
                     with_index_param_list  */
#line 3201 "parser.y"
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
#line 8114 "parser.cpp"
        break;

        case 415: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3222 "parser.y"
        {
            infinity::MatchExpr *match_text_expr = new infinity::MatchExpr();
            match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
            match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
            match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
            free((yyvsp[-4].str_value));
            free((yyvsp[-2].str_value));
            (yyval.expr_t) = match_text_expr;
        }
#line 8128 "parser.cpp"
        break;

        case 416: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3231 "parser.y"
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
#line 8144 "parser.cpp"
        break;

        case 417: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 3243 "parser.y"
        {
            infinity::MatchExpr *match_text_expr = new infinity::MatchExpr();
            match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
            match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
            free((yyvsp[-2].str_value));
            (yyval.expr_t) = match_text_expr;
        }
#line 8156 "parser.cpp"
        break;

        case 418: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3250 "parser.y"
        {
            infinity::MatchExpr *match_text_expr = new infinity::MatchExpr();
            match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
            match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
            match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
            free((yyvsp[-4].str_value));
            free((yyvsp[-2].str_value));
            (yyval.expr_t) = match_text_expr;
        }
#line 8170 "parser.cpp"
        break;

        case 419: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3260 "parser.y"
        {
            infinity::FusionExpr *fusion_expr = new infinity::FusionExpr();
            fusion_expr->method_ = std::string((yyvsp[-1].str_value));
            free((yyvsp[-1].str_value));
            (yyval.expr_t) = fusion_expr;
        }
#line 8181 "parser.cpp"
        break;

        case 420: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 3266 "parser.y"
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
#line 8197 "parser.cpp"
        break;

        case 421: /* sub_search: match_vector_expr  */
#line 3278 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8205 "parser.cpp"
        break;

        case 422: /* sub_search: match_text_expr  */
#line 3281 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8213 "parser.cpp"
        break;

        case 423: /* sub_search: match_tensor_expr  */
#line 3284 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8221 "parser.cpp"
        break;

        case 424: /* sub_search: match_sparse_expr  */
#line 3287 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8229 "parser.cpp"
        break;

        case 425: /* sub_search: query_expr  */
#line 3290 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8237 "parser.cpp"
        break;

        case 426: /* sub_search: fusion_expr  */
#line 3293 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8245 "parser.cpp"
        break;

        case 427: /* sub_search_array: sub_search  */
#line 3297 "parser.y"
        {
            (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr *>();
            (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
        }
#line 8254 "parser.cpp"
        break;

        case 428: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3301 "parser.y"
        {
            (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
        }
#line 8263 "parser.cpp"
        break;

        case 429: /* function_expr: IDENTIFIER '(' ')'  */
#line 3306 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            ParserHelper::ToLower((yyvsp[-2].str_value));
            func_expr->func_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));
            func_expr->arguments_ = nullptr;
            (yyval.expr_t) = func_expr;
        }
#line 8276 "parser.cpp"
        break;

        case 430: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3314 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            ParserHelper::ToLower((yyvsp[-3].str_value));
            func_expr->func_name_ = (yyvsp[-3].str_value);
            free((yyvsp[-3].str_value));
            func_expr->arguments_ = (yyvsp[-1].expr_array_t);
            (yyval.expr_t) = func_expr;
        }
#line 8289 "parser.cpp"
        break;

        case 431: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 3322 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            ParserHelper::ToLower((yyvsp[-4].str_value));
            func_expr->func_name_ = (yyvsp[-4].str_value);
            free((yyvsp[-4].str_value));
            func_expr->arguments_ = (yyvsp[-1].expr_array_t);
            func_expr->distinct_ = true;
            (yyval.expr_t) = func_expr;
        }
#line 8303 "parser.cpp"
        break;

        case 432: /* function_expr: JSON_EXTRACT '(' column_expr ',' STRING ')'  */
#line 3331 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "json_extract";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));

            infinity::JsonExtraInfo *extra_ptr = new infinity::JsonExtraInfo((yyvsp[-1].str_value));
            if (!extra_ptr->Init()) {
                delete extra_ptr;
                yyerror(&yyloc, scanner, result, "Invalid json_extract format");
                YYERROR;
            }
            func_expr->extra_info_ = std::shared_ptr<infinity::BaseExtraInfo>(extra_ptr);
            (yyval.expr_t) = func_expr;
        }
#line 8323 "parser.cpp"
        break;

        case 433: /* function_expr: YEAR '(' expr ')'  */
#line 3346 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "year";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8335 "parser.cpp"
        break;

        case 434: /* function_expr: MONTH '(' expr ')'  */
#line 3353 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "month";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8347 "parser.cpp"
        break;

        case 435: /* function_expr: DAY '(' expr ')'  */
#line 3360 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "day";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8359 "parser.cpp"
        break;

        case 436: /* function_expr: HOUR '(' expr ')'  */
#line 3367 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "hour";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8371 "parser.cpp"
        break;

        case 437: /* function_expr: MINUTE '(' expr ')'  */
#line 3374 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "minute";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8383 "parser.cpp"
        break;

        case 438: /* function_expr: SECOND '(' expr ')'  */
#line 3381 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "second";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8395 "parser.cpp"
        break;

        case 439: /* function_expr: operand IS NOT NULLABLE  */
#line 3388 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "is_not_null";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8407 "parser.cpp"
        break;

        case 440: /* function_expr: operand IS NULLABLE  */
#line 3395 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "is_null";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8419 "parser.cpp"
        break;

        case 441: /* function_expr: NOT operand  */
#line 3402 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "not";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8431 "parser.cpp"
        break;

        case 442: /* function_expr: '-' operand  */
#line 3409 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "-";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8443 "parser.cpp"
        break;

        case 443: /* function_expr: '+' operand  */
#line 3416 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "+";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8455 "parser.cpp"
        break;

        case 444: /* function_expr: operand '-' operand  */
#line 3423 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "-";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8468 "parser.cpp"
        break;

        case 445: /* function_expr: operand '+' operand  */
#line 3431 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "+";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8481 "parser.cpp"
        break;

        case 446: /* function_expr: operand '*' operand  */
#line 3439 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "*";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8494 "parser.cpp"
        break;

        case 447: /* function_expr: operand '/' operand  */
#line 3447 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "/";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8507 "parser.cpp"
        break;

        case 448: /* function_expr: operand '%' operand  */
#line 3455 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "%";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8520 "parser.cpp"
        break;

        case 449: /* function_expr: operand '=' operand  */
#line 3463 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8533 "parser.cpp"
        break;

        case 450: /* function_expr: operand EQUAL operand  */
#line 3471 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8546 "parser.cpp"
        break;

        case 451: /* function_expr: operand NOT_EQ operand  */
#line 3479 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "<>";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8559 "parser.cpp"
        break;

        case 452: /* function_expr: operand '<' operand  */
#line 3487 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "<";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8572 "parser.cpp"
        break;

        case 453: /* function_expr: operand '>' operand  */
#line 3495 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = ">";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8585 "parser.cpp"
        break;

        case 454: /* function_expr: operand LESS_EQ operand  */
#line 3503 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "<=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8598 "parser.cpp"
        break;

        case 455: /* function_expr: operand GREATER_EQ operand  */
#line 3511 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = ">=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8611 "parser.cpp"
        break;

        case 456: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3519 "parser.y"
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
#line 8646 "parser.cpp"
        break;

        case 457: /* function_expr: operand LIKE operand  */
#line 3549 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "like";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8659 "parser.cpp"
        break;

        case 458: /* function_expr: operand NOT LIKE operand  */
#line 3557 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "not_like";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8672 "parser.cpp"
        break;

        case 459: /* conjunction_expr: expr AND expr  */
#line 3566 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "and";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8685 "parser.cpp"
        break;

        case 460: /* conjunction_expr: expr OR expr  */
#line 3574 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "or";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8698 "parser.cpp"
        break;

        case 461: /* between_expr: operand BETWEEN operand AND operand  */
#line 3583 "parser.y"
        {
            infinity::BetweenExpr *between_expr = new infinity::BetweenExpr();
            between_expr->value_ = (yyvsp[-4].expr_t);
            between_expr->lower_bound_ = (yyvsp[-2].expr_t);
            between_expr->upper_bound_ = (yyvsp[0].expr_t);
            (yyval.expr_t) = between_expr;
        }
#line 8710 "parser.cpp"
        break;

        case 462: /* in_expr: operand IN '(' expr_array ')'  */
#line 3591 "parser.y"
        {
            infinity::InExpr *in_expr = new infinity::InExpr(true);
            in_expr->left_ = (yyvsp[-4].expr_t);
            in_expr->arguments_ = (yyvsp[-1].expr_array_t);
            (yyval.expr_t) = in_expr;
        }
#line 8721 "parser.cpp"
        break;

        case 463: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3597 "parser.y"
        {
            infinity::InExpr *in_expr = new infinity::InExpr(false);
            in_expr->left_ = (yyvsp[-5].expr_t);
            in_expr->arguments_ = (yyvsp[-1].expr_array_t);
            (yyval.expr_t) = in_expr;
        }
#line 8732 "parser.cpp"
        break;

        case 464: /* case_expr: CASE expr case_check_array END  */
#line 3604 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->expr_ = (yyvsp[-2].expr_t);
            case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
            (yyval.expr_t) = case_expr;
        }
#line 8743 "parser.cpp"
        break;

        case 465: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3610 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->expr_ = (yyvsp[-4].expr_t);
            case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
            case_expr->else_expr_ = (yyvsp[-1].expr_t);
            (yyval.expr_t) = case_expr;
        }
#line 8755 "parser.cpp"
        break;

        case 466: /* case_expr: CASE case_check_array END  */
#line 3617 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
            (yyval.expr_t) = case_expr;
        }
#line 8765 "parser.cpp"
        break;

        case 467: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3622 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
            case_expr->else_expr_ = (yyvsp[-1].expr_t);
            (yyval.expr_t) = case_expr;
        }
#line 8776 "parser.cpp"
        break;

        case 468: /* case_check_array: WHEN expr THEN expr  */
#line 3629 "parser.y"
        {
            (yyval.case_check_array_t) = new std::vector<infinity::WhenThen *>();
            infinity::WhenThen *when_then_ptr = new infinity::WhenThen();
            when_then_ptr->when_ = (yyvsp[-2].expr_t);
            when_then_ptr->then_ = (yyvsp[0].expr_t);
            (yyval.case_check_array_t)->emplace_back(when_then_ptr);
        }
#line 8788 "parser.cpp"
        break;

        case 469: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3636 "parser.y"
        {
            infinity::WhenThen *when_then_ptr = new infinity::WhenThen();
            when_then_ptr->when_ = (yyvsp[-2].expr_t);
            when_then_ptr->then_ = (yyvsp[0].expr_t);
            (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
            (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
        }
#line 8800 "parser.cpp"
        break;

        case 470: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3644 "parser.y"
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
#line 8817 "parser.cpp"
        break;

        case 471: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3657 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8828 "parser.cpp"
        break;

        case 472: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3663 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8839 "parser.cpp"
        break;

        case 473: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3669 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
            subquery_expr->left_ = (yyvsp[-4].expr_t);
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8851 "parser.cpp"
        break;

        case 474: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3676 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
            subquery_expr->left_ = (yyvsp[-5].expr_t);
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8863 "parser.cpp"
        break;

        case 475: /* column_expr: IDENTIFIER  */
#line 3684 "parser.y"
        {
            infinity::ColumnExpr *column_expr = new infinity::ColumnExpr();
            ParserHelper::ToLower((yyvsp[0].str_value));
            column_expr->names_.emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
            (yyval.expr_t) = column_expr;
        }
#line 8875 "parser.cpp"
        break;

        case 476: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3691 "parser.y"
        {
            infinity::ColumnExpr *column_expr = (infinity::ColumnExpr *)(yyvsp[-2].expr_t);
            ParserHelper::ToLower((yyvsp[0].str_value));
            column_expr->names_.emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
            (yyval.expr_t) = column_expr;
        }
#line 8887 "parser.cpp"
        break;

        case 477: /* column_expr: '*'  */
#line 3698 "parser.y"
        {
            infinity::ColumnExpr *column_expr = new infinity::ColumnExpr();
            column_expr->star_ = true;
            (yyval.expr_t) = column_expr;
        }
#line 8897 "parser.cpp"
        break;

        case 478: /* column_expr: column_expr '.' '*'  */
#line 3703 "parser.y"
        {
            infinity::ColumnExpr *column_expr = (infinity::ColumnExpr *)(yyvsp[-2].expr_t);
            if (column_expr->star_) {
                yyerror(&yyloc, scanner, result, "Invalid column expression format");
                YYERROR;
            }
            column_expr->star_ = true;
            (yyval.expr_t) = column_expr;
        }
#line 8911 "parser.cpp"
        break;

        case 479: /* constant_expr: STRING  */
#line 3713 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
            const_expr->str_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8921 "parser.cpp"
        break;

        case 480: /* constant_expr: TRUE  */
#line 3718 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
            const_expr->bool_value_ = true;
            (yyval.const_expr_t) = const_expr;
        }
#line 8931 "parser.cpp"
        break;

        case 481: /* constant_expr: FALSE  */
#line 3723 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
            const_expr->bool_value_ = false;
            (yyval.const_expr_t) = const_expr;
        }
#line 8941 "parser.cpp"
        break;

        case 482: /* constant_expr: DOUBLE_VALUE  */
#line 3728 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
            const_expr->double_value_ = (yyvsp[0].double_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8951 "parser.cpp"
        break;

        case 483: /* constant_expr: LONG_VALUE  */
#line 3733 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
            const_expr->integer_value_ = (yyvsp[0].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8961 "parser.cpp"
        break;

        case 484: /* constant_expr: JSON STRING  */
#line 3738 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kJson);
            const_expr->json_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8971 "parser.cpp"
        break;

        case 485: /* constant_expr: DATE STRING  */
#line 3743 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8981 "parser.cpp"
        break;

        case 486: /* constant_expr: TIME STRING  */
#line 3748 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8991 "parser.cpp"
        break;

        case 487: /* constant_expr: DATETIME STRING  */
#line 3753 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9001 "parser.cpp"
        break;

        case 488: /* constant_expr: TIMESTAMP STRING  */
#line 3758 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9011 "parser.cpp"
        break;

        case 489: /* constant_expr: INTERVAL interval_expr  */
#line 3763 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9019 "parser.cpp"
        break;

        case 490: /* constant_expr: interval_expr  */
#line 3766 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9027 "parser.cpp"
        break;

        case 491: /* constant_expr: common_array_expr  */
#line 3769 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9035 "parser.cpp"
        break;

        case 492: /* constant_expr: curly_brackets_expr  */
#line 3772 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9043 "parser.cpp"
        break;

        case 493: /* common_array_expr: array_expr  */
#line 3776 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9051 "parser.cpp"
        break;

        case 494: /* common_array_expr: subarray_array_expr  */
#line 3779 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9059 "parser.cpp"
        break;

        case 495: /* common_array_expr: sparse_array_expr  */
#line 3782 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9067 "parser.cpp"
        break;

        case 496: /* common_array_expr: empty_array_expr  */
#line 3785 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9075 "parser.cpp"
        break;

        case 497: /* common_sparse_array_expr: sparse_array_expr  */
#line 3789 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9083 "parser.cpp"
        break;

        case 498: /* common_sparse_array_expr: array_expr  */
#line 3792 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9091 "parser.cpp"
        break;

        case 499: /* common_sparse_array_expr: empty_array_expr  */
#line 3795 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9099 "parser.cpp"
        break;

        case 500: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3799 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9107 "parser.cpp"
        break;

        case 501: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3803 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
            const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
            (yyval.const_expr_t) = const_expr;
        }
#line 9117 "parser.cpp"
        break;

        case 502: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3808 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9126 "parser.cpp"
        break;

        case 503: /* sparse_array_expr: long_sparse_array_expr  */
#line 3813 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9134 "parser.cpp"
        break;

        case 504: /* sparse_array_expr: double_sparse_array_expr  */
#line 3816 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9142 "parser.cpp"
        break;

        case 505: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3820 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9150 "parser.cpp"
        break;

        case 506: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3824 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
            const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
            const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
            delete (yyvsp[0].int_sparse_ele_t);
            (yyval.const_expr_t) = const_expr;
        }
#line 9162 "parser.cpp"
        break;

        case 507: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3831 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
            (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
            delete (yyvsp[0].int_sparse_ele_t);
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9173 "parser.cpp"
        break;

        case 508: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3838 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9181 "parser.cpp"
        break;

        case 509: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3842 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
            const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
            const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
            delete (yyvsp[0].float_sparse_ele_t);
            (yyval.const_expr_t) = const_expr;
        }
#line 9193 "parser.cpp"
        break;

        case 510: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3849 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
            (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
            delete (yyvsp[0].float_sparse_ele_t);
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9204 "parser.cpp"
        break;

        case 511: /* empty_array_expr: '[' ']'  */
#line 3856 "parser.y"
        {
            (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
        }
#line 9212 "parser.cpp"
        break;

        case 512: /* curly_brackets_expr: unclosed_curly_brackets_expr '}'  */
#line 3860 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9220 "parser.cpp"
        break;

        case 513: /* curly_brackets_expr: '{' '}'  */
#line 3863 "parser.y"
        {
            (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
        }
#line 9228 "parser.cpp"
        break;

        case 514: /* unclosed_curly_brackets_expr: '{' constant_expr  */
#line 3867 "parser.y"
        {
            (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
            (yyval.const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
        }
#line 9237 "parser.cpp"
        break;

        case 515: /* unclosed_curly_brackets_expr: unclosed_curly_brackets_expr ',' constant_expr  */
#line 3871 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9246 "parser.cpp"
        break;

        case 516: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3876 "parser.y"
        {
            (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
        }
#line 9254 "parser.cpp"
        break;

        case 517: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3880 "parser.y"
        {
            (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
        }
#line 9262 "parser.cpp"
        break;

        case 518: /* array_expr: long_array_expr  */
#line 3884 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9270 "parser.cpp"
        break;

        case 519: /* array_expr: double_array_expr  */
#line 3887 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9278 "parser.cpp"
        break;

        case 520: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3891 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9286 "parser.cpp"
        break;

        case 521: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3895 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
            const_expr->long_array_.emplace_back((yyvsp[0].long_value));
            (yyval.const_expr_t) = const_expr;
        }
#line 9296 "parser.cpp"
        break;

        case 522: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3900 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9305 "parser.cpp"
        break;

        case 523: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3905 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9313 "parser.cpp"
        break;

        case 524: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3909 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
            const_expr->double_array_.emplace_back((yyvsp[0].double_value));
            (yyval.const_expr_t) = const_expr;
        }
#line 9323 "parser.cpp"
        break;

        case 525: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3914 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9332 "parser.cpp"
        break;

        case 526: /* interval_expr: LONG_VALUE SECONDS  */
#line 3919 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kSecond;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9343 "parser.cpp"
        break;

        case 527: /* interval_expr: LONG_VALUE SECOND  */
#line 3925 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kSecond;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9354 "parser.cpp"
        break;

        case 528: /* interval_expr: LONG_VALUE MINUTES  */
#line 3931 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMinute;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9365 "parser.cpp"
        break;

        case 529: /* interval_expr: LONG_VALUE MINUTE  */
#line 3937 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMinute;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9376 "parser.cpp"
        break;

        case 530: /* interval_expr: LONG_VALUE HOURS  */
#line 3943 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kHour;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9387 "parser.cpp"
        break;

        case 531: /* interval_expr: LONG_VALUE HOUR  */
#line 3949 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kHour;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9398 "parser.cpp"
        break;

        case 532: /* interval_expr: LONG_VALUE DAYS  */
#line 3955 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kDay;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9409 "parser.cpp"
        break;

        case 533: /* interval_expr: LONG_VALUE DAY  */
#line 3961 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kDay;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9420 "parser.cpp"
        break;

        case 534: /* interval_expr: LONG_VALUE MONTHS  */
#line 3967 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMonth;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9431 "parser.cpp"
        break;

        case 535: /* interval_expr: LONG_VALUE MONTH  */
#line 3973 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMonth;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9442 "parser.cpp"
        break;

        case 536: /* interval_expr: LONG_VALUE YEARS  */
#line 3979 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kYear;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9453 "parser.cpp"
        break;

        case 537: /* interval_expr: LONG_VALUE YEAR  */
#line 3985 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kYear;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9464 "parser.cpp"
        break;

        case 538: /* copy_option_list: copy_option  */
#line 3996 "parser.y"
        {
            (yyval.copy_option_array) = new std::vector<infinity::CopyOption *>();
            (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
        }
#line 9473 "parser.cpp"
        break;

        case 539: /* copy_option_list: copy_option_list ',' copy_option  */
#line 4000 "parser.y"
        {
            (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
            (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
        }
#line 9482 "parser.cpp"
        break;

        case 540: /* copy_option: FORMAT JSON  */
#line 4005 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kFormat;
            (yyval.copy_option_t)->file_type_ = infinity::CopyFileType::kJSON;
        }
#line 9492 "parser.cpp"
        break;

        case 541: /* copy_option: FORMAT IDENTIFIER  */
#line 4010 "parser.y"
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
#line 9526 "parser.cpp"
        break;

        case 542: /* copy_option: DELIMITER STRING  */
#line 4039 "parser.y"
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
#line 9541 "parser.cpp"
        break;

        case 543: /* copy_option: HEADER  */
#line 4049 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
            (yyval.copy_option_t)->header_ = true;
        }
#line 9551 "parser.cpp"
        break;

        case 544: /* copy_option: OFFSET LONG_VALUE  */
#line 4054 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
            (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
        }
#line 9561 "parser.cpp"
        break;

        case 545: /* copy_option: LIMIT LONG_VALUE  */
#line 4059 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
            (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
        }
#line 9571 "parser.cpp"
        break;

        case 546: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 4064 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
            (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
        }
#line 9581 "parser.cpp"
        break;

        case 547: /* file_path: STRING  */
#line 4070 "parser.y"
        {
            (yyval.str_value) = (yyvsp[0].str_value);
        }
#line 9589 "parser.cpp"
        break;

        case 548: /* if_exists: IF EXISTS  */
#line 4074 "parser.y"
        {
            (yyval.bool_value) = true;
        }
#line 9595 "parser.cpp"
        break;

        case 549: /* if_exists: %empty  */
#line 4075 "parser.y"
        {
            (yyval.bool_value) = false;
        }
#line 9601 "parser.cpp"
        break;

        case 550: /* if_not_exists: IF NOT EXISTS  */
#line 4077 "parser.y"
        {
            (yyval.bool_value) = true;
        }
#line 9607 "parser.cpp"
        break;

        case 551: /* if_not_exists: %empty  */
#line 4078 "parser.y"
        {
            (yyval.bool_value) = false;
        }
#line 9613 "parser.cpp"
        break;

        case 554: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 4093 "parser.y"
        {
            (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
            (yyval.if_not_exists_info_t)->exists_ = true;
            (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 9626 "parser.cpp"
        break;

        case 555: /* if_not_exists_info: %empty  */
#line 4101 "parser.y"
        {
            (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
        }
#line 9634 "parser.cpp"
        break;

        case 556: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 4105 "parser.y"
        {
            (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
        }
#line 9642 "parser.cpp"
        break;

        case 557: /* with_index_param_list: %empty  */
#line 4108 "parser.y"
        {
            (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter *>();
        }
#line 9650 "parser.cpp"
        break;

        case 558: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 4112 "parser.y"
        {
            (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
        }
#line 9658 "parser.cpp"
        break;

        case 559: /* optional_table_properties_list: %empty  */
#line 4115 "parser.y"
        {
            (yyval.with_index_param_list_t) = nullptr;
        }
#line 9666 "parser.cpp"
        break;

        case 560: /* index_param_list: index_param  */
#line 4119 "parser.y"
        {
            (yyval.index_param_list_t) = new std::vector<infinity::InitParameter *>();
            (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
        }
#line 9675 "parser.cpp"
        break;

        case 561: /* index_param_list: index_param_list ',' index_param  */
#line 4123 "parser.y"
        {
            (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
            (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
        }
#line 9684 "parser.cpp"
        break;

        case 562: /* index_param: IDENTIFIER  */
#line 4128 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 9695 "parser.cpp"
        break;

        case 563: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 4134 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));

            (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 9710 "parser.cpp"
        break;

        case 564: /* index_param: IDENTIFIER '=' STRING  */
#line 4144 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));

            (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 9725 "parser.cpp"
        break;

        case 565: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 4154 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));

            (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
        }
#line 9738 "parser.cpp"
        break;

        case 566: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 4162 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));

            (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
        }
#line 9751 "parser.cpp"
        break;

        case 567: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 4173 "parser.y"
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
#line 9809 "parser.cpp"
        break;

        case 568: /* index_info: '(' IDENTIFIER ')'  */
#line 4226 "parser.y"
        {
            (yyval.index_info_t) = new infinity::IndexInfo();
            (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
            free((yyvsp[-1].str_value));
        }
#line 9821 "parser.cpp"
        break;

#line 9825 "parser.cpp"

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

#line 4235 "parser.y"

void yyerror(YYLTYPE *llocp, void *lexer, infinity::ParserResult *result, const char *msg) {
    if (result->IsError())
        return;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
    fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

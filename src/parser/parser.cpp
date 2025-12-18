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
    YYSYMBOL_JSON_EXTRACT_STRING = 153,                  /* JSON_EXTRACT_STRING  */
    YYSYMBOL_JSON_EXTRACT_INT = 154,                     /* JSON_EXTRACT_INT  */
    YYSYMBOL_EQUAL = 155,                                /* EQUAL  */
    YYSYMBOL_NOT_EQ = 156,                               /* NOT_EQ  */
    YYSYMBOL_LESS_EQ = 157,                              /* LESS_EQ  */
    YYSYMBOL_GREATER_EQ = 158,                           /* GREATER_EQ  */
    YYSYMBOL_BETWEEN = 159,                              /* BETWEEN  */
    YYSYMBOL_AND = 160,                                  /* AND  */
    YYSYMBOL_OR = 161,                                   /* OR  */
    YYSYMBOL_EXTRACT = 162,                              /* EXTRACT  */
    YYSYMBOL_LIKE = 163,                                 /* LIKE  */
    YYSYMBOL_DATA = 164,                                 /* DATA  */
    YYSYMBOL_LOG = 165,                                  /* LOG  */
    YYSYMBOL_BUFFER = 166,                               /* BUFFER  */
    YYSYMBOL_TRANSACTIONS = 167,                         /* TRANSACTIONS  */
    YYSYMBOL_TRANSACTION = 168,                          /* TRANSACTION  */
    YYSYMBOL_MEMINDEX = 169,                             /* MEMINDEX  */
    YYSYMBOL_USING = 170,                                /* USING  */
    YYSYMBOL_SESSION = 171,                              /* SESSION  */
    YYSYMBOL_GLOBAL = 172,                               /* GLOBAL  */
    YYSYMBOL_OFF = 173,                                  /* OFF  */
    YYSYMBOL_EXPORT = 174,                               /* EXPORT  */
    YYSYMBOL_CONFIGS = 175,                              /* CONFIGS  */
    YYSYMBOL_CONFIG = 176,                               /* CONFIG  */
    YYSYMBOL_PROFILES = 177,                             /* PROFILES  */
    YYSYMBOL_VARIABLES = 178,                            /* VARIABLES  */
    YYSYMBOL_VARIABLE = 179,                             /* VARIABLE  */
    YYSYMBOL_LOGS = 180,                                 /* LOGS  */
    YYSYMBOL_CATALOGS = 181,                             /* CATALOGS  */
    YYSYMBOL_CATALOG = 182,                              /* CATALOG  */
    YYSYMBOL_SEARCH = 183,                               /* SEARCH  */
    YYSYMBOL_MATCH = 184,                                /* MATCH  */
    YYSYMBOL_MAXSIM = 185,                               /* MAXSIM  */
    YYSYMBOL_QUERY = 186,                                /* QUERY  */
    YYSYMBOL_QUERIES = 187,                              /* QUERIES  */
    YYSYMBOL_FUSION = 188,                               /* FUSION  */
    YYSYMBOL_ROWLIMIT = 189,                             /* ROWLIMIT  */
    YYSYMBOL_ADMIN = 190,                                /* ADMIN  */
    YYSYMBOL_LEADER = 191,                               /* LEADER  */
    YYSYMBOL_FOLLOWER = 192,                             /* FOLLOWER  */
    YYSYMBOL_LEARNER = 193,                              /* LEARNER  */
    YYSYMBOL_CONNECT = 194,                              /* CONNECT  */
    YYSYMBOL_STANDALONE = 195,                           /* STANDALONE  */
    YYSYMBOL_NODES = 196,                                /* NODES  */
    YYSYMBOL_NODE = 197,                                 /* NODE  */
    YYSYMBOL_REMOVE = 198,                               /* REMOVE  */
    YYSYMBOL_SNAPSHOT = 199,                             /* SNAPSHOT  */
    YYSYMBOL_SNAPSHOTS = 200,                            /* SNAPSHOTS  */
    YYSYMBOL_RECOVER = 201,                              /* RECOVER  */
    YYSYMBOL_RESTORE = 202,                              /* RESTORE  */
    YYSYMBOL_CACHES = 203,                               /* CACHES  */
    YYSYMBOL_CACHE = 204,                                /* CACHE  */
    YYSYMBOL_PERSISTENCE = 205,                          /* PERSISTENCE  */
    YYSYMBOL_OBJECT = 206,                               /* OBJECT  */
    YYSYMBOL_OBJECTS = 207,                              /* OBJECTS  */
    YYSYMBOL_FILES = 208,                                /* FILES  */
    YYSYMBOL_MEMORY = 209,                               /* MEMORY  */
    YYSYMBOL_ALLOCATION = 210,                           /* ALLOCATION  */
    YYSYMBOL_HISTORY = 211,                              /* HISTORY  */
    YYSYMBOL_CHECK = 212,                                /* CHECK  */
    YYSYMBOL_CLEAN = 213,                                /* CLEAN  */
    YYSYMBOL_CHECKPOINT = 214,                           /* CHECKPOINT  */
    YYSYMBOL_IMPORT = 215,                               /* IMPORT  */
    YYSYMBOL_NUMBER = 216,                               /* NUMBER  */
    YYSYMBOL_217_ = 217,                                 /* '='  */
    YYSYMBOL_218_ = 218,                                 /* '<'  */
    YYSYMBOL_219_ = 219,                                 /* '>'  */
    YYSYMBOL_220_ = 220,                                 /* '+'  */
    YYSYMBOL_221_ = 221,                                 /* '-'  */
    YYSYMBOL_222_ = 222,                                 /* '*'  */
    YYSYMBOL_223_ = 223,                                 /* '/'  */
    YYSYMBOL_224_ = 224,                                 /* '%'  */
    YYSYMBOL_225_ = 225,                                 /* ';'  */
    YYSYMBOL_226_ = 226,                                 /* '('  */
    YYSYMBOL_227_ = 227,                                 /* ')'  */
    YYSYMBOL_228_ = 228,                                 /* ','  */
    YYSYMBOL_229_ = 229,                                 /* '.'  */
    YYSYMBOL_230_ = 230,                                 /* ']'  */
    YYSYMBOL_231_ = 231,                                 /* '['  */
    YYSYMBOL_232_ = 232,                                 /* '}'  */
    YYSYMBOL_233_ = 233,                                 /* '{'  */
    YYSYMBOL_234_ = 234,                                 /* ':'  */
    YYSYMBOL_YYACCEPT = 235,                             /* $accept  */
    YYSYMBOL_input_pattern = 236,                        /* input_pattern  */
    YYSYMBOL_statement_list = 237,                       /* statement_list  */
    YYSYMBOL_statement = 238,                            /* statement  */
    YYSYMBOL_explainable_statement = 239,                /* explainable_statement  */
    YYSYMBOL_create_statement = 240,                     /* create_statement  */
    YYSYMBOL_table_element_array = 241,                  /* table_element_array  */
    YYSYMBOL_column_def_array = 242,                     /* column_def_array  */
    YYSYMBOL_table_element = 243,                        /* table_element  */
    YYSYMBOL_table_column = 244,                         /* table_column  */
    YYSYMBOL_column_type_array = 245,                    /* column_type_array  */
    YYSYMBOL_column_type = 246,                          /* column_type  */
    YYSYMBOL_column_constraints = 247,                   /* column_constraints  */
    YYSYMBOL_column_constraint = 248,                    /* column_constraint  */
    YYSYMBOL_default_expr = 249,                         /* default_expr  */
    YYSYMBOL_table_constraint = 250,                     /* table_constraint  */
    YYSYMBOL_identifier_array = 251,                     /* identifier_array  */
    YYSYMBOL_delete_statement = 252,                     /* delete_statement  */
    YYSYMBOL_insert_statement = 253,                     /* insert_statement  */
    YYSYMBOL_optional_identifier_array = 254,            /* optional_identifier_array  */
    YYSYMBOL_explain_statement = 255,                    /* explain_statement  */
    YYSYMBOL_update_statement = 256,                     /* update_statement  */
    YYSYMBOL_update_expr_array = 257,                    /* update_expr_array  */
    YYSYMBOL_update_expr = 258,                          /* update_expr  */
    YYSYMBOL_drop_statement = 259,                       /* drop_statement  */
    YYSYMBOL_copy_statement = 260,                       /* copy_statement  */
    YYSYMBOL_select_statement = 261,                     /* select_statement  */
    YYSYMBOL_select_with_paren = 262,                    /* select_with_paren  */
    YYSYMBOL_select_without_paren = 263,                 /* select_without_paren  */
    YYSYMBOL_select_clause_with_modifier = 264,          /* select_clause_with_modifier  */
    YYSYMBOL_select_clause_without_modifier_paren = 265, /* select_clause_without_modifier_paren  */
    YYSYMBOL_select_clause_without_modifier = 266,       /* select_clause_without_modifier  */
    YYSYMBOL_order_by_clause = 267,                      /* order_by_clause  */
    YYSYMBOL_order_by_expr_list = 268,                   /* order_by_expr_list  */
    YYSYMBOL_order_by_expr = 269,                        /* order_by_expr  */
    YYSYMBOL_order_by_type = 270,                        /* order_by_type  */
    YYSYMBOL_limit_expr = 271,                           /* limit_expr  */
    YYSYMBOL_offset_expr = 272,                          /* offset_expr  */
    YYSYMBOL_distinct = 273,                             /* distinct  */
    YYSYMBOL_highlight_clause = 274,                     /* highlight_clause  */
    YYSYMBOL_from_clause = 275,                          /* from_clause  */
    YYSYMBOL_search_clause = 276,                        /* search_clause  */
    YYSYMBOL_optional_search_filter_expr = 277,          /* optional_search_filter_expr  */
    YYSYMBOL_where_clause = 278,                         /* where_clause  */
    YYSYMBOL_having_clause = 279,                        /* having_clause  */
    YYSYMBOL_group_by_clause = 280,                      /* group_by_clause  */
    YYSYMBOL_set_operator = 281,                         /* set_operator  */
    YYSYMBOL_table_reference = 282,                      /* table_reference  */
    YYSYMBOL_table_reference_unit = 283,                 /* table_reference_unit  */
    YYSYMBOL_table_reference_name = 284,                 /* table_reference_name  */
    YYSYMBOL_table_name = 285,                           /* table_name  */
    YYSYMBOL_table_alias = 286,                          /* table_alias  */
    YYSYMBOL_with_clause = 287,                          /* with_clause  */
    YYSYMBOL_with_expr_list = 288,                       /* with_expr_list  */
    YYSYMBOL_with_expr = 289,                            /* with_expr  */
    YYSYMBOL_join_clause = 290,                          /* join_clause  */
    YYSYMBOL_join_type = 291,                            /* join_type  */
    YYSYMBOL_show_statement = 292,                       /* show_statement  */
    YYSYMBOL_flush_statement = 293,                      /* flush_statement  */
    YYSYMBOL_optimize_statement = 294,                   /* optimize_statement  */
    YYSYMBOL_command_statement = 295,                    /* command_statement  */
    YYSYMBOL_compact_statement = 296,                    /* compact_statement  */
    YYSYMBOL_admin_statement = 297,                      /* admin_statement  */
    YYSYMBOL_alter_statement = 298,                      /* alter_statement  */
    YYSYMBOL_check_statement = 299,                      /* check_statement  */
    YYSYMBOL_expr_array = 300,                           /* expr_array  */
    YYSYMBOL_insert_row_list = 301,                      /* insert_row_list  */
    YYSYMBOL_expr_alias = 302,                           /* expr_alias  */
    YYSYMBOL_expr = 303,                                 /* expr  */
    YYSYMBOL_operand = 304,                              /* operand  */
    YYSYMBOL_match_tensor_expr = 305,                    /* match_tensor_expr  */
    YYSYMBOL_match_vector_expr = 306,                    /* match_vector_expr  */
    YYSYMBOL_match_sparse_expr = 307,                    /* match_sparse_expr  */
    YYSYMBOL_match_text_expr = 308,                      /* match_text_expr  */
    YYSYMBOL_query_expr = 309,                           /* query_expr  */
    YYSYMBOL_fusion_expr = 310,                          /* fusion_expr  */
    YYSYMBOL_sub_search = 311,                           /* sub_search  */
    YYSYMBOL_sub_search_array = 312,                     /* sub_search_array  */
    YYSYMBOL_function_expr = 313,                        /* function_expr  */
    YYSYMBOL_conjunction_expr = 314,                     /* conjunction_expr  */
    YYSYMBOL_between_expr = 315,                         /* between_expr  */
    YYSYMBOL_in_expr = 316,                              /* in_expr  */
    YYSYMBOL_case_expr = 317,                            /* case_expr  */
    YYSYMBOL_case_check_array = 318,                     /* case_check_array  */
    YYSYMBOL_cast_expr = 319,                            /* cast_expr  */
    YYSYMBOL_subquery_expr = 320,                        /* subquery_expr  */
    YYSYMBOL_column_expr = 321,                          /* column_expr  */
    YYSYMBOL_constant_expr = 322,                        /* constant_expr  */
    YYSYMBOL_common_array_expr = 323,                    /* common_array_expr  */
    YYSYMBOL_common_sparse_array_expr = 324,             /* common_sparse_array_expr  */
    YYSYMBOL_subarray_array_expr = 325,                  /* subarray_array_expr  */
    YYSYMBOL_unclosed_subarray_array_expr = 326,         /* unclosed_subarray_array_expr  */
    YYSYMBOL_sparse_array_expr = 327,                    /* sparse_array_expr  */
    YYSYMBOL_long_sparse_array_expr = 328,               /* long_sparse_array_expr  */
    YYSYMBOL_unclosed_long_sparse_array_expr = 329,      /* unclosed_long_sparse_array_expr  */
    YYSYMBOL_double_sparse_array_expr = 330,             /* double_sparse_array_expr  */
    YYSYMBOL_unclosed_double_sparse_array_expr = 331,    /* unclosed_double_sparse_array_expr  */
    YYSYMBOL_empty_array_expr = 332,                     /* empty_array_expr  */
    YYSYMBOL_curly_brackets_expr = 333,                  /* curly_brackets_expr  */
    YYSYMBOL_unclosed_curly_brackets_expr = 334,         /* unclosed_curly_brackets_expr  */
    YYSYMBOL_int_sparse_ele = 335,                       /* int_sparse_ele  */
    YYSYMBOL_float_sparse_ele = 336,                     /* float_sparse_ele  */
    YYSYMBOL_array_expr = 337,                           /* array_expr  */
    YYSYMBOL_long_array_expr = 338,                      /* long_array_expr  */
    YYSYMBOL_unclosed_long_array_expr = 339,             /* unclosed_long_array_expr  */
    YYSYMBOL_double_array_expr = 340,                    /* double_array_expr  */
    YYSYMBOL_unclosed_double_array_expr = 341,           /* unclosed_double_array_expr  */
    YYSYMBOL_interval_expr = 342,                        /* interval_expr  */
    YYSYMBOL_copy_option_list = 343,                     /* copy_option_list  */
    YYSYMBOL_copy_option = 344,                          /* copy_option  */
    YYSYMBOL_file_path = 345,                            /* file_path  */
    YYSYMBOL_if_exists = 346,                            /* if_exists  */
    YYSYMBOL_if_not_exists = 347,                        /* if_not_exists  */
    YYSYMBOL_semicolon = 348,                            /* semicolon  */
    YYSYMBOL_if_not_exists_info = 349,                   /* if_not_exists_info  */
    YYSYMBOL_with_index_param_list = 350,                /* with_index_param_list  */
    YYSYMBOL_optional_table_properties_list = 351,       /* optional_table_properties_list  */
    YYSYMBOL_index_param_list = 352,                     /* index_param_list  */
    YYSYMBOL_index_param = 353,                          /* index_param  */
    YYSYMBOL_index_info = 354                            /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;

/* Unqualified %code blocks.  */
#line 98 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 481 "parser.cpp"

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
#define YYLAST 1617

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS 235
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS 120
/* YYNRULES -- Number of rules.  */
#define YYNRULES 570
/* YYNSTATES -- Number of states.  */
#define YYNSTATES 1288

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK 471

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX) (0 <= (YYX) && (YYX) <= YYMAXUTOK ? YY_CAST(yysymbol_kind_t, yytranslate[YYX]) : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] = {
    0,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   224, 2,   2,   226, 227, 222, 220, 228, 221, 229, 223, 2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   234, 225, 218, 217, 219, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   231, 2,   230, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   233, 2,   232, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
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
    193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216};

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
    3331, 3346, 3361, 3376, 3383, 3390, 3397, 3404, 3411, 3418, 3425, 3432, 3439, 3446, 3453, 3461, 3469, 3477, 3485, 3493, 3501, 3509, 3517, 3525,
    3533, 3541, 3549, 3579, 3587, 3596, 3604, 3613, 3621, 3627, 3634, 3640, 3647, 3652, 3659, 3666, 3674, 3687, 3693, 3699, 3706, 3714, 3721, 3728,
    3733, 3743, 3748, 3753, 3758, 3763, 3768, 3773, 3778, 3783, 3788, 3793, 3796, 3799, 3802, 3806, 3809, 3812, 3815, 3819, 3822, 3825, 3829, 3833,
    3838, 3843, 3846, 3850, 3854, 3861, 3868, 3872, 3879, 3886, 3890, 3893, 3897, 3901, 3906, 3910, 3914, 3917, 3921, 3925, 3930, 3935, 3939, 3944,
    3949, 3955, 3961, 3967, 3973, 3979, 3985, 3991, 3997, 4003, 4009, 4015, 4026, 4030, 4035, 4040, 4069, 4079, 4084, 4089, 4094, 4100, 4104, 4105,
    4107, 4108, 4110, 4111, 4123, 4131, 4135, 4138, 4142, 4145, 4149, 4153, 4158, 4164, 4174, 4184, 4192, 4203, 4256};
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
                                      "JSON_EXTRACT_STRING",
                                      "JSON_EXTRACT_INT",
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

#define YYPACT_NINF (-797)

#define yypact_value_is_default(Yyn) ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-558)

#define yytable_value_is_error(Yyn) ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] = {
    1161, 43,   44,   104,  83,   128,  83,   286,  248,  1049, 313,  252,  250,  269,  334,  83,   352,  206,  92,   58,   297,  222,  -33,  341,
    189,  -797, -797, -797, -797, -797, -797, -797, -797, 338,  -797, -797, 428,  -797, -797, -797, -797, -797, -797, -797, -797, 356,  356,  356,
    356,  116,  445,  83,   367,  367,  367,  367,  367,  483,  227,  501,  83,   -34,  526,  531,  534,  359,  -797, -797, -797, -797, -797, -797,
    -797, 338,  -797, -797, -797, -797, -797, 294,  459,  470,  542,  83,   -797, -797, -797, -797, -797, 38,   -797, 304,  371,  -797, 544,  -797,
    -797, 42,   551,  -797, 552,  -797, -797, -797, 114,  4,    41,   101,  -797, 497,  83,   83,   560,  -797, -797, -797, -797, -797, -797, 516,
    349,  -797, 573,  402,  417,  303,  478,  438,  593,  432,  523,  447,  451,  460,  83,   -797, -797, 385,  435,  -797, 1136, -797, 621,  -797,
    -797, 12,   579,  -797, 600,  596,  673,  83,   83,   83,   676,  623,  625,  488,  631,  705,  83,   83,   83,   716,  -797, 718,  720,  659,
    723,  723,  775,  64,   107,  129,  -797, 502,  616,  633,  -797, 442,  -797, -797, -797, 764,  -797, 766,  -797, -797, 723,  -797, -797, 770,
    -797, -797, -797, -797, -797, 654,  -797, 660,  83,   422,  -797, 745,  582,  352,  723,  -797, 806,  -797, 642,  -797, 807,  -797, -797, 811,
    -797, 809,  -797, 812,  814,  -797, 815,  763,  816,  622,  825,  827,  829,  -797, -797, -797, -797, -797, 12,   -797, -797, -797, 775,  781,
    768,  767,  707,  -17,  -797, 488,  -797, 83,   374,  837,  55,   -797, -797, -797, -797, -797, 782,  -797, 626,  -27,  -797, 775,  -797, -797,
    772,  773,  619,  -797, -797, 1235, 882,  620,  627,  578,  848,  850,  851,  852,  853,  -797, -797, 855,  636,  637,  638,  640,  641,  646,
    648,  649,  650,  655,  302,  656,  663,  972,  972,  -797, 685,  10,   21,   232,  -797, 6,    1110, -797, -797, -797, -797, -797, -797, -797,
    -797, -797, -797, -797, -797, -797, 630,  -797, -797, -797, 133,  -797, -797, 214,  -797, 242,  -797, -797, -52,  -797, -797, 278,  -797, 284,
    -797, -797, -797, -797, -797, -797, -797, -797, -797, -797, -797, -797, -797, -797, -797, -797, -797, -797, -797, 865,  864,  -797, -797, -797,
    -797, -797, -797, -797, -797, -797, 802,  844,  845,  822,  83,   428,  -797, -797, -797, 888,  54,   -797, 887,  -797, -797, 823,  377,  -797,
    899,  -797, -797, -797, 677,  693,  -29,  775,  775,  857,  -797, 918,  -33,  32,   869,  712,  928,  938,  -797, -797, 346,  717,  -797, 83,
    775,  720,  -797, 430,  719,  724,  414,  -797, -797, -797, -797, -797, -797, -797, -797, -797, -797, -797, -797, 972,  725,  1137, 866,  775,
    775,  182,  419,  -797, -797, -797, -797, -797, 1235, -797, 775,  775,  775,  775,  775,  775,  26,   26,   26,   945,  726,  727,  728,  731,
    954,  956,  253,  253,  715,  -63,  -797, 730,  -797, -797, -797, -797, -797, -797, 885,  775,  963,  775,  775,  -1,   743,  -37,  972,  972,
    972,  972,  972,  972,  972,  972,  972,  972,  972,  972,  972,  972,  28,   739,  -797, 965,  -797, 967,  -797, 13,   -797, 968,  -797, 974,
    -797, 939,  570,  754,  -797, 756,  757,  984,  -797, 761,  -797, 983,  -797, 321,  987,  824,  828,  -797, -797, -797, 775,  916,  765,  -797,
    74,   430,  775,  -797, -797, 67,   1299, 870,  776,  398,  -797, -797, -797, -33,  996,  868,  -797, -797, -797, 1001, 775,  777,  -797, 430,
    -797, 215,  215,  775,  -797, 400,  866,  846,  780,  29,   121,  425,  -797, 775,  775,  137,  140,  152,  154,  164,  178,  -797, 420,  426,
    436,  934,  775,  26,   775,  1007, 784,  443,  -797, -797, 667,  723,  -797, -797, -797, 854,  787,  972,  685,  883,  -797, 177,  177,  245,
    245,  1122, 177,  177,  245,  245,  253,  253,  -797, -797, -797, -797, -797, -797, 783,  -797, 789,  -797, -797, -797, -797, 1010, 1012, -797,
    1022, 837,  1024, -797, -797, -797, 1023, -797, -797, 1027, 1029, 810,  37,   856,  775,  -797, -797, -797, 430,  1033, -797, -797, -797, -797,
    -797, -797, -797, -797, -797, -797, -797, -797, 817,  -797, -797, -797, -797, -797, -797, -797, -797, -797, -797, -797, -797, 820,  821,  830,
    832,  833,  834,  835,  838,  281,  839,  837,  1009, 32,   338,  826,  1037, -797, 456,  843,  1036, 1042, 1045, 82,   -797, 1044, 473,  -797,
    476,  479,  -797, 836,  -797, 1299, 775,  -797, 775,  34,   146,  -797, -797, -797, -797, -797, -797, 1047, 1050, 1063, 972,  -106, 482,  -82,
    849,  61,   858,  -797, 1067, -797, -797, 995,  685,  177,  859,  485,  -797, 972,  1072, 1078, 684,  1041, 871,  517,  -797, 519,  525,  -797,
    1090, -797, -797, -33,  872,  467,  -797, 85,   -797, 354,  659,  -797, -797, 1091, 1299, 1299, 512,  644,  803,  841,  1000, 1210, 969,  989,
    -797, -797, 218,  -797, 994,  837,  527,  891,  986,  -797, 953,  -797, -797, 775,  -797, -797, -797, -797, -797, -797, -797, 215,  -797, -797,
    -797, 902,  430,  148,  -797, 775,  903,  904,  905,  475,  775,  739,  906,  1129, 907,  775,  -797, 911,  913,  914,  529,  -797, -797, 1137,
    1134, -797, 1138, 583,  -797, 1022, -797, -797, 1024, 486,  46,   37,   1085, -797, -797, -797, -797, -797, -797, 1097, -797, 1152, -797, -797,
    -797, -797, -797, -797, -797, -797, 929,  1109, 533,  936,  545,  -797, 941,  951,  952,  958,  960,  971,  973,  976,  978,  1074, 979,  980,
    981,  985,  991,  999,  1002, 1004, 1005, 1006, 1088, 1013, 1014, 1016, 1017, 1018, 1019, 1028, 1031, 1032, 1043, 1089, 1046, 1048, 1053, 1055,
    1056, 1058, 1059, 1060, 1061, 1062, 1103, 1068, 1069, 1070, 1071, 1079, 1083, 1084, 1086, 1092, 1093, 1104, 1094, 1095, 1096, 1108, 1119, 1124,
    1125, 1139, 1140, 1141, 1107, 1142, -797, -797, 13,   -797, 1076, 1077, 549,  -797, 1022, 1198, 1219, 555,  -797, -797, -797, 430,  -797, -797,
    -797, -797, -60,  1143, 17,   1144, -797, -797, -797, 1178, 1157, 1011, 430,  -797, 215,  -797, -797, -797, -797, -797, -797, -797, -797, -797,
    -797, 1222, -797, 85,   467,  37,   37,   1025, 354,  1184, 1186, -797, 1244, -797, -797, 1299, 1251, 1255, 1266, 1269, 1285, 1300, 1307, 1312,
    1313, 1185, 1319, 1344, 1358, 1359, 1360, 1408, 1410, 1411, 1420, 1421, 1200, 1423, 1424, 1425, 1426, 1427, 1428, 1429, 1430, 1431, 1432, 1211,
    1434, 1435, 1436, 1437, 1438, 1439, 1440, 1441, 1442, 1443, 1223, 1444, 1446, 1447, 1448, 1449, 1450, 1451, 1452, 1453, 1454, 1233, 1456, 1457,
    1458, 1459, 1460, 1461, 1462, 1463, 1464, 1465, 1245, 1466, -797, 1470, 1471, -797, 557,  -797, 802,  -797, 1472, 1473, 1474, 144,  1252, -797,
    563,  1475, -797, -797, 1418, 837,  -797, 775,  775,  -797, 1256, -797, 1257, 1258, 1259, 1260, 1261, 1262, 1263, 1264, 1265, 1476, 1267, 1268,
    1270, 1271, 1272, 1273, 1274, 1275, 1276, 1277, 1487, 1278, 1279, 1280, 1281, 1282, 1283, 1284, 1286, 1287, 1288, 1490, 1289, 1290, 1291, 1292,
    1293, 1294, 1295, 1296, 1297, 1298, 1506, 1301, 1302, 1303, 1304, 1305, 1306, 1308, 1309, 1310, 1311, 1520, 1314, 1315, 1316, 1317, 1318, 1320,
    1321, 1322, 1323, 1324, 1521, 1325, -797, -797, -797, -797, 1326, 1327, 1328, 907,  -797, -797, 453,  775,  567,  810,  430,  -797, -797, -797,
    -797, -797, -797, -797, -797, -797, -797, 1330, -797, -797, -797, -797, -797, -797, -797, -797, -797, -797, 1331, -797, -797, -797, -797, -797,
    -797, -797, -797, -797, -797, 1332, -797, -797, -797, -797, -797, -797, -797, -797, -797, -797, 1333, -797, -797, -797, -797, -797, -797, -797,
    -797, -797, -797, 1334, -797, -797, -797, -797, -797, -797, -797, -797, -797, -797, 1335, -797, 1530, 1535, 209,  1336, 1337, 1534, 1540, -797,
    -797, -797, 430,  -797, -797, -797, -797, -797, -797, -797, 1338, 1339, 907,  802,  -797, 1547, 752,  262,  1341, 1561, 1342, -797, 759,  1564,
    -797, 907,  802,  907,  -16,  1565, -797, 1525, 1346, -797, 1347, 1536, 1537, -797, -797, -797, 5,    3,    -797, 1351, 1539, 1541, -797, 1542,
    1543, 1576, -797, 1357, -797, 1361, 1362, 1581, 1582, 802,  1363, 1364, -797, 802,  -797, -797};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] = {
    237, 0,   0,   0,   0,   0,   0,   0,   237, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   237, 0,   555, 3,   5,   10,  12,
    13,  11,  6,   7,   9,   182, 181, 0,   8,   14,  15,  16,  17,  18,  19,  20,  553, 553, 553, 553, 553, 0,   0,   551, 551, 551, 551, 551, 0,
    230, 0,   0,   0,   0,   0,   0,   237, 168, 21,  26,  28,  27,  22,  23,  25,  24,  29,  30,  31,  32,  0,   295, 300, 0,   0,   251, 252, 250,
    256, 260, 0,   257, 0,   0,   253, 0,   255, 280, 281, 0,   258, 0,   291, 293, 294, 0,   287, 303, 297, 302, 0,   0,   0,   0,   306, 307, 308,
    309, 311, 310, 0,   236, 238, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   378, 335, 0,   0,   1,   237, 2,   220, 222, 223,
    0,   205, 187, 193, 0,   0,   0,   0,   0,   0,   0,   0,   166, 0,   0,   0,   0,   0,   0,   331, 0,   0,   215, 0,   0,   0,   0,   0,   0,
    167, 0,   0,   0,   267, 268, 261, 262, 263, 0,   264, 0,   254, 282, 0,   259, 292, 0,   285, 284, 288, 289, 305, 0,   299, 0,   0,   0,   337,
    0,   0,   0,   0,   359, 0,   369, 0,   370, 0,   356, 357, 0,   352, 0,   365, 367, 0,   360, 0,   0,   0,   0,   0,   0,   0,   379, 186, 185,
    4,   221, 0,   183, 184, 204, 0,   0,   201, 0,   34,  0,   35,  166, 556, 0,   0,   0,   237, 550, 173, 175, 174, 176, 0,   231, 0,   215, 170,
    0,   162, 549, 0,   0,   477, 481, 484, 485, 0,   0,   0,   0,   0,   0,   0,   0,   0,   482, 483, 0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   479, 237, 0,   0,   0,   380, 385, 386, 400, 398, 401, 399, 402, 403, 395, 390, 389, 388, 396, 397, 387, 394,
    393, 493, 496, 0,   497, 505, 0,   506, 0,   498, 494, 0,   495, 520, 0,   521, 0,   492, 315, 317, 316, 313, 314, 320, 322, 321, 318, 319, 325,
    327, 326, 323, 324, 290, 296, 301, 0,   0,   270, 269, 275, 265, 266, 283, 286, 304, 298, 559, 0,   0,   0,   0,   0,   239, 312, 362, 0,   353,
    358, 0,   366, 361, 0,   0,   368, 0,   333, 334, 332, 0,   0,   207, 0,   0,   203, 552, 0,   237, 0,   0,   0,   0,   0,   330, 160, 0,   0,
    164, 0,   0,   0,   169, 214, 0,   0,   0,   529, 528, 531, 530, 533, 532, 535, 534, 537, 536, 539, 538, 0,   0,   443, 237, 0,   0,   0,   0,
    486, 487, 488, 489, 490, 0,   491, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   445, 444, 0,   0,   526, 523,
    513, 503, 508, 511, 515, 516, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   502, 0,   507, 0,   510, 0,   514, 0,   522, 0,   525, 276, 271, 0,   377, 0,   0,   0,   336, 0,   371, 0,   354, 0,   0,   0,   0,   364,
    190, 189, 0,   209, 192, 194, 199, 200, 0,   188, 33,  37,  0,   0,   0,   0,   43,  47,  48,  237, 0,   41,  329, 328, 165, 0,   0,   163, 177,
    172, 171, 0,   0,   0,   429, 0,   237, 0,   0,   0,   0,   0,   468, 0,   0,   0,   0,   0,   0,   0,   0,   477, 0,   0,   0,   0,   0,   0,
    0,   0,   213, 0,   392, 391, 0,   0,   381, 384, 461, 462, 0,   0,   237, 0,   442, 452, 453, 456, 457, 0,   459, 451, 454, 455, 447, 446, 448,
    449, 450, 478, 480, 504, 0,   509, 0,   512, 517, 524, 527, 0,   0,   272, 0,   0,   0,   374, 240, 355, 0,   338, 363, 0,   0,   206, 0,   211,
    0,   197, 198, 196, 202, 0,   55,  56,  59,  60,  57,  58,  61,  62,  78,  63,  65,  64,  81,  68,  69,  70,  66,  67,  71,  72,  73,  74,  75,
    76,  77,  0,   0,   0,   0,   0,   0,   0,   0,   559, 0,   0,   561, 0,   40,  0,   0,   161, 0,   0,   0,   0,   0,   0,   545, 0,   0,   540,
    0,   0,   430, 0,   473, 0,   0,   466, 0,   0,   0,   440, 439, 438, 437, 436, 435, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   419, 0,
    519, 518, 0,   237, 460, 0,   0,   441, 0,   0,   0,   277, 273, 564, 0,   562, 0,   0,   45,  339, 372, 373, 237, 208, 224, 226, 235, 227, 0,
    215, 195, 39,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   153, 154, 157, 150, 157, 0,   0,   0,   36,  44,  570, 42,  382, 0,   547,
    546, 544, 543, 542, 548, 180, 0,   178, 431, 474, 0,   470, 0,   469, 0,   0,   0,   0,   0,   0,   0,   0,   0,   213, 0,   417, 0,   0,   0,
    0,   475, 464, 463, 0,   278, 0,   0,   558, 0,   376, 375, 0,   0,   0,   0,   0,   244, 245, 246, 247, 243, 248, 0,   233, 0,   228, 423, 421,
    424, 422, 425, 426, 427, 210, 219, 0,   0,   0,   53,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   155, 152, 0,   151, 50,  49,  0,   159, 0,   0,
    0,   0,   541, 472, 467, 471, 432, 433, 434, 458, 0,   0,   0,   0,   499, 501, 500, 213, 0,   0,   212, 420, 0,   476, 465, 279, 274, 565, 566,
    568, 567, 563, 46,  0,   340, 235, 225, 0,   0,   232, 0,   0,   217, 80,  0,   148, 149, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   156, 0,
    0,   158, 0,   38,  559, 383, 0,   0,   0,   0,   0,   418, 0,   341, 229, 241, 0,   0,   428, 0,   0,   191, 0,   54,  0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   52,  51,  560, 569, 0,   0,   213, 213, 415, 179, 0,   0,   0,   218, 216, 79,  85,  86,  83,  84,  87,  88,  89,  90,  91,  0,
    82,  129, 130, 127, 128, 131, 132, 133, 134, 135, 0,   126, 96,  97,  94,  95,  98,  99,  100, 101, 102, 0,   93,  107, 108, 105, 106, 109, 110,
    111, 112, 113, 0,   104, 140, 141, 138, 139, 142, 143, 144, 145, 146, 0,   137, 118, 119, 116, 117, 120, 121, 122, 123, 124, 0,   115, 0,   0,
    0,   0,   0,   0,   0,   343, 342, 348, 242, 234, 92,  136, 103, 114, 147, 125, 213, 0,   213, 559, 416, 349, 344, 0,   0,   0,   0,   414, 0,
    0,   345, 213, 559, 213, 559, 0,   350, 346, 0,   410, 0,   0,   0,   413, 351, 347, 559, 404, 412, 0,   0,   0,   409, 0,   0,   0,   408, 0,
    406, 0,   0,   0,   0,   559, 0,   0,   411, 559, 405, 407};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] = {
    -797, -797, -797, 1455, 1523, 264,  -797, -797, 912,  -594, -797, -687, -797, 831,  840,  -797, -616, 291,  315,  1349, -797, 319,  -797, 1177,
    328,  343,  -6,   1570, -19,  1225, 1365, -81,  -797, -797, 959,  -797, -797, -797, -797, -797, -797, -797, -796, -252, -797, -797, -797, -797,
    771,  -160, 45,   639,  -797, -797, 1395, -797, -797, 366,  384,  408,  413,  415,  -797, -797, -797, -237, -797, 1132, -261, -262, -732, -729,
    -725, -712, -703, -701, 643,  -797, -797, -797, -797, -797, -797, 1171, -797, -797, -392, -296, -293, -797, -797, -797, 801,  -797, -797, -797,
    -797, 804,  -797, -797, 1111, 1112, 813,  -797, -797, -797, -797, 1329, -541, 818,  -158, 540,  574,  -797, -797, -672, -797, 687,  793,  -797};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] = {0,   23,  24,  25,  66,   26,  537, 742, 538, 539, 850, 677, 770, 771, 922, 540, 404, 27,  28,  250,
                                         29,  30,  259, 260, 31,   32,  33,  34,  35,  147, 235, 148, 240, 526, 527, 641, 393, 531, 238, 525,
                                         637, 754, 722, 262, 1065, 970, 145, 748, 749, 750, 751, 838, 36,  120, 121, 752, 835, 37,  38,  39,
                                         40,  41,  42,  43,  44,   301, 549, 302, 303, 304, 305, 306, 307, 308, 309, 310, 845, 846, 311, 312,
                                         313, 314, 315, 434, 316,  317, 318, 319, 320, 941, 321, 322, 323, 324, 325, 326, 327, 328, 329, 330,
                                         466, 467, 331, 332, 333,  334, 335, 336, 694, 695, 264, 159, 150, 141, 155, 508, 776, 739, 740, 543};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] = {
    411,  390,  73,   138,  469,  772,  465,  410,  429,  741,  947,  265,  433,  696,  793,  462,  463,  267,  268,  269,  146,  839,  462,  463,
    840,  267,  268,  269,  841,  573,  362,  611,  743,  458,  459,  534,  595,  461,  396,  261,  58,   842,  168,  169,  180,  16,   187,  196,
    373,  59,   843,  61,   844,  524,  473,  474,  574,  575,  576,  73,   118,  472,  507,  774,  236,  806,  142,  337,  143,  338,  339,  849,
    851,  144,  592,  45,   46,   47,   473,  474,  51,   48,   49,   507,  701,  785,  58,   142,  836,  143,  131,  132,  405,  515,  144,  596,
    157,  473,  474,  123,  473,  474,  516,  274,  124,  167,  125,  198,  126,  133,  342,  274,  343,  344,  197,  275,  276,  277,  796,  188,
    1259, 278,  802,  275,  276,  277,  340,  807,  179,  278,  528,  529,  347,  16,   348,  349,  52,   53,   54,   1271, 837,  1268, 55,   56,
    639,  640,  804,  551,  1141, 1054, 279,  280,  281,  389,  1260, 201,  202,  924,  279,  280,  281,  535,  593,  536,  585,  429,  473,  474,
    1050, 345,  561,  562,  786,  1272, 199,  1269, 499,  557,  -557, 229,  500,  567,  568,  569,  570,  571,  572,  718,  149,  473,  474,  350,
    170,  22,   473,  474,  243,  244,  245,  471,  613,  409,  643,  618,  60,   253,  254,  255,  702,  397,  807,  194,  590,  591,  195,  1236,
    597,  598,  599,  600,  601,  602,  603,  604,  605,  606,  607,  608,  609,  610,  960,  406,  932,  797,  473,  474,  839,  341,  234,  840,
    464,  299,  50,   841,  299,  366,  300,  464,  297,  181,  612,  65,   299,  468,  300,  1,    842,  2,    3,    4,    5,    6,    7,    747,
    9,    843,  127,  844,  1249, 642,  432,  11,   67,   963,  12,   807,  13,   14,   15,   460,  346,  473,  474,  688,  689,  111,  128,  635,
    1067, 112,  129,  766,  399,  130,  690,  691,  692,  473,  474,  68,   473,  474,  351,  57,   705,  706,  473,  474,  473,  474,  477,  686,
    473,  474,  473,  474,  109,  717,  697,  719,  191,  192,  193,  69,   473,  474,  16,   70,   807,  67,   134,  729,  -558, -558, 480,  481,
    71,   117,  473,  474,  -558, 139,  473,  474,  1219, 1220, 110,  767,  135,  768,  769,  72,   920,  630,  766,  119,  68,   731,  142,  507,
    143,  493,  631,  494,  707,  144,  1,    708,  2,    3,    4,    5,    6,    7,    74,   9,    1137, 528,  477,  709,  69,   710,  11,   122,
    70,   12,   136,  13,   14,   15,   533,  711,  75,   71,   -558, 485,  486,  487,  488,  489,  490,  491,  -558, -558, 693,  712,  400,  401,
    72,   1056, 767,  560,  768,  769,  140,  512,  76,   266,  267,  268,  269,  77,   17,   78,   452,  402,  453,  454,  149,  727,  455,  74,
    367,  113,  114,  115,  146,  16,   1242, 158,  1244, 794,  495,  795,  496,  1146, 368,  369,  156,  75,   19,   116,  550,  1256, 801,  1258,
    165,  62,   63,   470,  471,  21,   64,   -558, -558, 487,  488,  489,  490,  491,  497,  815,  498,  76,   22,   489,  490,  491,  77,   555,
    78,   355,  182,  183,  356,  357,  164,  270,  271,  358,  359,  812,  1221, 209,  210,  1222, 1223, 272,  211,  273,  1224, 1225, 847,  564,
    274,  565,  501,  566,  502,  703,  939,  704,  503,  566,  504,  166,  275,  276,  277,  961,  175,  962,  278,  828,  -249, 829,  830,  831,
    832,  171,  833,  834,  176,  17,   172,  682,  933,  173,  292,  699,  293,  938,  294,  177,  929,  178,  948,  186,  559,  184,  185,  279,
    280,  281,  282,  190,  283,  189,  284,  200,  285,  19,   286,  203,  287,  1245, 288,  289,  290,  519,  520,  204,  21,   546,  547,  730,
    291,  205,  1257, 206,  1261, 266,  267,  268,  269,  22,   955,  956,  957,  958,  473,  474,  1270, 160,  161,  162,  163,  223,  292,  225,
    293,  207,  294,  852,  853,  854,  855,  856,  477,  1284, 857,  858,  230,  1287, 622,  623,  208,  859,  860,  861,  151,  152,  153,  154,
    1042, 680,  681,  698,  471,  224,  478,  479,  480,  481,  295,  296,  297,  222,  483,  862,  298,  556,  233,  212,  237,  299,  226,  300,
    713,  492,  227,  270,  271,  213,  714,  492,  214,  215,  216,  228,  217,  272,  231,  273,  715,  492,  432,  239,  274,  241,  723,  724,
    725,  726,  218,  219,  242,  220,  221,  246,  275,  276,  277,  780,  471,  247,  278,  248,  266,  267,  268,  269,  484,  485,  486,  487,
    488,  489,  490,  491,  788,  789,  937,  790,  789,  251,  791,  471,  252,  811,  803,  492,  814,  471,  249,  279,  280,  281,  282,  256,
    283,  257,  284,  258,  285,  261,  286,  263,  287,  352,  288,  289,  290,  816,  817,  863,  864,  865,  866,  867,  291,  826,  868,  869,
    820,  821,  822,  547,  353,  870,  871,  872,  823,  824,  925,  547,  952,  471,  270,  271,  971,  972,  292,  16,   293,  354,  294,  360,
    272,  361,  273,  873,  974,  975,  363,  274,  1045, 547,  266,  267,  268,  269,  1049, 471,  1136, 821,  364,  275,  276,  277,  1143, 789,
    365,  278,  1227, 547,  1247, 1248, 295,  296,  297,  1253, 1254, 1148, 298,  1059, 1060, 370,  371,  299,  374,  300,  375,  376,  377,  378,
    379,  380,  382,  381,  383,  384,  279,  280,  281,  282,  1147, 283,  385,  284,  386,  285,  387,  286,  391,  287,  392,  288,  289,  290,
    403,  394,  395,  408,  407,  414,  430,  291,  270,  271,  412,  413,  435,  431,  436,  437,  438,  439,  272,  492,  273,  440,  442,  443,
    444,  274,  445,  446,  505,  292,  506,  293,  447,  294,  448,  449,  450,  275,  276,  277,  507,  451,  456,  278,  1226, 266,  267,  268,
    269,  457,  509,  510,  514,  517,  874,  875,  876,  877,  878,  511,  518,  879,  880,  521,  522,  295,  296,  297,  881,  882,  883,  298,
    279,  280,  281,  282,  299,  283,  300,  284,  523,  285,  532,  286,  541,  287,  530,  288,  289,  290,  884,  544,  885,  886,  887,  888,
    889,  291,  542,  890,  891,  545,  584,  548,  16,   553,  892,  893,  894,  577,  554,  558,  578,  579,  580,  427,  428,  581,  582,  292,
    583,  293,  587,  294,  586,  272,  589,  273,  895,  594,  299,  614,  274,  616,  619,  266,  267,  268,  269,  620,  624,  621,  625,  626,
    275,  276,  277,  627,  628,  629,  278,  632,  636,  638,  633,  295,  296,  297,  634,  683,  678,  298,  679,  684,  685,  687,  299,  700,
    300,  593,  716,  720,  721,  728,  473,  732,  736,  734,  737,  279,  280,  281,  282,  735,  283,  738,  284,  534,  285,  744,  286,  745,
    287,  746,  288,  289,  290,  756,  471,  753,  775,  779,  782,  757,  291,  427,  758,  759,  783,  784,  787,  798,  79,   778,  799,  272,
    760,  273,  761,  762,  763,  764,  274,  792,  765,  773,  292,  800,  293,  781,  294,  809,  80,   810,  275,  276,  277,  805,  726,  81,
    278,  82,   83,   725,  84,   808,  813,  818,  819,  85,   86,   896,  897,  898,  899,  900,  825,  848,  901,  902,  827,  918,  295,  296,
    297,  903,  904,  905,  298,  279,  280,  281,  282,  299,  283,  300,  284,  926,  285,  919,  286,  927,  287,  928,  288,  289,  290,  906,
    920,  931,  934,  935,  936,  945,  291,  946,  -554, 940,  949,  950,  953,  951,  965,  1,    954,  2,    3,    4,    5,    6,    7,    8,
    9,    10,   966,  967,  292,  968,  293,  11,   294,  969,  12,   973,  13,   14,   15,   985,  1,    976,  2,    3,    4,    5,    6,    7,
    8,    9,    10,   977,  978,  996,  1007, 475,  11,   476,  979,  12,   980,  13,   14,   15,   295,  296,  297,  559,  1018, 1029, 298,  981,
    1040, 982,  1047, 299,  983,  300,  984,  986,  987,  988,  559,  1043, 1044, 989,  16,   87,   88,   89,   90,   990,  91,   92,   1048, 807,
    93,   94,   95,   991,  1057, 96,   992,  97,   993,  994,  995,  98,   99,   1063, 1055, 16,   1064, 997,  998,  477,  999,  1000, 1001, 1002,
    100,  101,  1066, 1061, 102,  103,  104,  477,  1003, 1068, 105,  1004, 1005, 1069, 106,  107,  108,  478,  479,  480,  481,  482,  477,  1006,
    1070, 483,  1008, 1071, 1009, 478,  479,  480,  481,  1010, 733,  1011, 1012, 483,  1013, 1014, 1015, 1016, 1017, 1072, 478,  479,  480,  481,
    1019, 1020, 1021, 1022, 483,  907,  908,  909,  910,  911,  1073, 1023, 912,  913,  17,   1024, 1025, 1074, 1026, 914,  915,  916,  1075, 1076,
    1027, 1028, 1030, 1031, 1032, 1078, 18,   484,  485,  486,  487,  488,  489,  490,  491,  17,   1033, 917,  19,   484,  485,  486,  487,  488,
    489,  490,  491,  1034, 20,   21,   1079, 18,   1035, 1036, 484,  485,  486,  487,  488,  489,  490,  491,  22,   19,   1080, 1081, 1082, 1037,
    1038, 1039, 1041, 1051, 1052, 20,   21,   415,  416,  417,  418,  419,  420,  421,  422,  423,  424,  425,  426,  22,   644,  645,  646,  647,
    648,  649,  650,  651,  652,  653,  654,  655,  656,  657,  658,  659,  660,  661,  1053, 662,  663,  664,  665,  666,  667,  1077, 1083, 668,
    1084, 1085, 669,  670,  671,  672,  673,  674,  675,  676,  1086, 1087, 1088, 1089, 1090, 1091, 1092, 1093, 1094, 1095, 1096, 1097, 1098, 1099,
    1100, 1101, 1102, 1103, 1104, 1105, 1106, 1107, 1108, 1109, 1111, 1110, 1112, 1113, 1114, 1115, 1116, 1117, 1118, 1119, 1120, 1121, 1122, 1123,
    1124, 1125, 1126, 1127, 1128, 1129, 1130, 1131, 1133, 1132, 1134, 1135, 1138, 1139, 1140, 1142, 1145, 1144, 1159, 1149, 1150, 1151, 1152, 1153,
    1154, 1155, 1156, 1157, 1158, 1170, 1160, 1161, 1181, 1162, 1163, 1164, 1165, 1166, 1167, 1168, 1169, 1171, 1172, 1173, 1174, 1175, 1176, 1177,
    1192, 1178, 1179, 1180, 1182, 1183, 1184, 1185, 1186, 1187, 1188, 1189, 1190, 1191, 1203, 1214, 1193, 1194, 1195, 1196, 1197, 1198, 1234, 1199,
    1200, 1201, 1202, 1235, 1239, 1204, 1205, 1206, 1207, 1208, 1240, 1209, 1210, 1211, 1212, 1213, 1215, 1246, 1216, 1217, 1218, 1228, 1229, 1230,
    1231, 1232, 1233, 1237, 1238, 1251, 1241, 1243, 1250, 1252, 1255, 1262, 1263, 1264, 1265, 1266, 1267, 1273, 1274, 1278, 1275, 1276, 1277, 1279,
    1282, 1283, 552,  1280, 174,  1281, 1285, 1286, 137,  777,  398,  232,  513,  755,  964,  388,  372,  921,  1058, 588,  563,  942,  615,  930,
    943,  617,  441,  1062, 923,  1046, 959,  0,    0,    944};

static const yytype_int16 yycheck[] = {
    261, 238, 8,   22,  300, 677, 299,  259, 270,  625, 806, 169,  273, 554,  701, 5,    6,    4,   5,   6,    8,    753,  5,   6,    753, 4,    5,
    6,   753, 3,   188, 3,   626, 295,  296, 3,    73,  298, 55,   66,  3,    753, 76,   77,   6,   78,  4,    6,    206,  4,   753,  6,   753,  82,
    160, 161, 448, 449, 450, 65,  15,   55,  78,   679, 145, 4,    20,  3,    22,  5,    6,    758, 759, 27,   75,   32,   33,  34,   160, 161,  36,
    38,  39,  78,  55,  3,   3,   20,   3,   22,   32,  33,  37,   39,  27,   132, 51,   160,  161, 7,   160,  161,  48,   90,  12,   60,  14,   6,
    16,  51,  3,   90,  5,   6,   73,   102, 103,  104, 84,  77,   136, 108,  228, 102,  103,  104, 62,  66,   83,   108,  391, 392,  3,   78,   5,
    6,   32,  33,  34,  136, 55,  136,  38,  39,   70,  71,  228,  408, 4,    945, 137,  138,  139, 234, 170,  110,  111,  773, 137,  138, 139,  129,
    163, 131, 227, 427, 160, 161, 228,  62,  431,  432, 90,  170,  73,  170,  228, 414,  62,   134, 232, 442,  443,  444,  445, 446,  447, 579,  72,
    160, 161, 62,  226, 226, 160, 161,  151, 152,  153, 228, 493,  228, 135,  499, 76,   160,  161, 162, 87,   226,  66,   207, 473,  474, 210,  6,
    478, 479, 480, 481, 482, 483, 484,  485, 486,  487, 488, 489,  490, 491,  824, 250,  84,   87,  160, 161,  968,  173,  226, 968,  230, 231,  199,
    968, 231, 200, 233, 230, 222, 211,  222, 3,    231, 232, 233,  7,   968,  9,   10,   11,   12,  13,  14,   226,  16,   968, 174,  968, 6,    530,
    88,  23,  8,   227, 26,  66,  28,   29,  30,   298, 173, 160,  161, 68,   69,  33,   194,  524, 975, 39,   198,  73,   247, 201,  79,  80,   81,
    160, 161, 8,   160, 161, 173, 199,  565, 566,  160, 161, 160,  161, 133,  548, 160,  161,  160, 161, 3,    578,  555,  580, 206,  207, 208,  8,
    160, 161, 78,  8,   66,  65,  33,   593, 155,  156, 157, 158,  8,   3,    160, 161,  163,  0,   160, 161,  1140, 1141, 33,  129,  51,  131,  132,
    8,   134, 32,  73,  3,   65,  594,  20,  78,   22,  228, 41,   230, 227,  27,  7,    227,  9,   10,  11,   12,   13,   14,  8,    16,  1048, 638,
    133, 227, 65,  227, 23,  177, 65,   26,  164,  28,  29,  30,   396, 227,  8,   65,   217,  218, 219, 220,  221,  222,  223, 224,  157, 158,  189,
    227, 32,  33,  65,  950, 129, 430,  131, 132,  225, 370, 8,    3,   4,    5,   6,    8,    174, 8,   122,  51,   124,  125, 72,   587, 128,  65,
    10,  164, 165, 166, 8,   78,  1234, 72,  1236, 702, 228, 704,  230, 1061, 24,  25,   3,    65,  202, 182,  407,  1249, 716, 1251, 229, 171,  172,
    227, 228, 213, 176, 218, 219, 220,  221, 222,  223, 224, 228,  733, 230,  65,  226,  222,  223, 224, 65,   65,   65,   39,  178,  179, 42,   43,
    3,   73,  74,  47,  48,  728, 39,   190, 191,  42,  43,  83,   195, 85,   47,  48,   754,  84,  90,  86,   228,  88,   230, 84,   803, 86,   228,
    88,  230, 14,  102, 103, 104, 33,   226, 35,   108, 56,  57,   58,  59,   60,  61,   3,    63,  64,  73,   174,  3,    541, 797,  3,   184,  558,
    186, 802, 188, 73,  781, 3,   807,  3,   73,   178, 179, 137,  138, 139,  140, 3,    142,  6,   144, 62,   146,  202,  148, 3,    150, 1237, 152,
    153, 154, 192, 193, 55,  213, 227,  228, 594,  162, 228, 1250, 6,   1252, 3,   4,    5,    6,   226, 3,    4,    5,    6,   160,  161, 1264, 53,
    54,  55,  56,  4,   184, 76,  186,  199, 188,  91,  92,  93,   94,  95,   133, 1281, 98,   99,  227, 1285, 44,   45,   199, 105,  106, 107,  46,
    47,  48,  49,  920, 227, 228, 227,  228, 197,  155, 156, 157,  158, 220,  221, 222,  199,  163, 127, 226,  227,  21,   165, 65,   231, 199,  233,
    228, 229, 199, 73,  74,  175, 228,  229, 178,  179, 180, 199,  182, 83,   227, 85,   228,  229, 88,  67,   90,   73,   227, 228,  5,   6,    196,
    197, 3,   199, 200, 3,   102, 103,  104, 227,  228, 62,  108,  62,  3,    4,   5,    6,    217, 218, 219,  220,  221,  222, 223,  224, 227,  228,
    227, 227, 228, 74,  227, 228, 3,    728, 228,  229, 227, 228,  226, 137,  138, 139,  140,  3,   142, 3,    144,  3,    146, 66,   148, 4,    150,
    227, 152, 153, 154, 49,  50,  91,   92,  93,   94,  95,  162,  747, 98,   99,  227,  228,  227, 228, 132,  105,  106,  107, 227,  228, 227,  228,
    227, 228, 73,  74,  227, 228, 184,  78,  186,  132, 188, 3,    83,  3,    85,  127,  227,  228, 4,   90,   227,  228,  3,   4,    5,   6,    227,
    228, 227, 228, 132, 102, 103, 104,  227, 228,  132, 108, 227,  228, 44,   45,  220,  221,  222, 42,  43,   1064, 226,  965, 966,  62,  226,  231,
    4,   233, 170, 6,   3,   6,   4,    3,   55,   4,   4,   199,  137, 138,  139, 140,  1063, 142, 3,   144,  3,    146,  3,   148,  53,  150,  68,
    152, 153, 154, 3,   74,  135, 217,  62,  226,  226, 162, 73,   74,  78,   78,  4,    226,  4,   4,   4,    4,    83,   229, 85,   6,   226,  226,
    226, 90,  226, 226, 3,   184, 6,    186, 226,  188, 226, 226,  226, 102,  103, 104,  78,   226, 226, 108,  1145, 3,    4,   5,    6,   226,  46,
    46,  4,   6,   91,  92,  93,  94,   95,  77,   77,  98,  99,   4,   227,  220, 221,  222,  105, 106, 107,  226,  137,  138, 139,  140, 231,  142,
    233, 144, 227, 146, 4,   148, 55,   150, 69,   152, 153, 154,  127, 3,    91,  92,   93,   94,  95,  162,  226,  98,   99,  3,    227, 226,  78,
    226, 105, 106, 107, 4,   226, 226,  226, 226,  226, 73,  74,   226, 4,    184, 4,    186,  77,  188, 234,  83,   3,    85,  127,  226, 231,  6,
    90,  6,   6,   3,   4,   5,   6,    5,   226,  42,  226, 226,  102, 103,  104, 3,    227,  6,   108, 4,    76,   228,  170, 220,  221, 222,  170,
    3,   130, 226, 226, 135, 3,   228,  231, 227,  233, 163, 76,   4,   228,  226, 160,  132,  6,   234, 6,    137,  138,  139, 140,  234, 142,  3,
    144, 3,   146, 6,   148, 4,   150,  4,   152,  153, 154, 4,    228, 183,  31,  4,    6,    226, 162, 73,   226,  226,  6,   4,    6,   4,    3,
    227, 4,   83,  226, 85,  226, 226,  226, 226,  90,  227, 226,  226, 184,  4,   186,  226,  188, 4,   23,   78,   102,  103, 104,  228, 6,    30,
    108, 32,  33,  5,   35,  227, 227,  46,  217,  40,  41,  91,   92,  93,   94,  95,   6,    6,   98,  99,   228,  132,  220, 221,  222, 105,  106,
    107, 226, 137, 138, 139, 140, 231,  142, 233,  144, 226, 146,  130, 148,  135, 150,  170,  152, 153, 154,  127,  134,  227, 227,  227, 227,  4,
    162, 228, 0,   231, 227, 226, 6,    227, 57,   7,   6,   9,    10,  11,   12,  13,   14,   15,  16,  17,   57,   3,    184, 228,  186, 23,   188,
    52,  26,  227, 28,  29,  30,  93,   7,   228,  9,   10,  11,   12,  13,   14,  15,   16,   17,  228, 228,  93,   93,   73,  23,   75,  228,  26,
    228, 28,  29,  30,  220, 221, 222,  73,  93,   93,  226, 228,  93,  228,  4,   231,  228,  233, 228, 228,  228,  228,  73,  135,  135, 228,  78,
    166, 167, 168, 169, 228, 171, 172,  3,   66,   175, 176, 177,  228, 6,    180, 228,  182,  228, 228, 228,  186,  187,  53,  227,  78,  54,   228,
    228, 133, 228, 228, 228, 228, 199,  200, 6,    226, 203, 204,  205, 133,  228, 6,    209,  228, 228, 6,    213,  214,  215, 155,  156, 157,  158,
    159, 133, 228, 6,   163, 228, 6,    228, 155,  156, 157, 158,  228, 160,  228, 228,  163,  228, 228, 228,  228,  228,  6,   155,  156, 157,  158,
    228, 228, 228, 228, 163, 91,  92,   93,  94,   95,  6,   228,  98,  99,   174, 228,  228,  6,   228, 105,  106,  107,  6,   6,    228, 228,  228,
    228, 228, 6,   190, 217, 218, 219,  220, 221,  222, 223, 224,  174, 228,  127, 202,  217,  218, 219, 220,  221,  222,  223, 224,  228, 212,  213,
    6,   190, 228, 228, 217, 218, 219,  220, 221,  222, 223, 224,  226, 202,  6,   6,    6,    228, 228, 228,  228,  228,  228, 212,  213, 140,  141,
    142, 143, 144, 145, 146, 147, 148,  149, 150,  151, 226, 89,   90,  91,   92,  93,   94,   95,  96,  97,   98,   99,   100, 101,  102, 103,  104,
    105, 106, 228, 108, 109, 110, 111,  112, 113,  228, 6,   116,  6,   6,    119, 120,  121,  122, 123, 124,  125,  126,  6,   6,    228, 6,    6,
    6,   6,   6,   6,   6,   6,   6,    6,   228,  6,   6,   6,    6,   6,    6,   6,    6,    6,   6,   6,    228,  6,    6,   6,    6,   6,    6,
    6,   6,   6,   228, 6,   6,   6,    6,   6,    6,   6,   6,    6,   6,    6,   228,  4,    4,   4,   4,    4,    227,  62,  6,    6,   227,  227,
    227, 227, 227, 227, 227, 227, 227,  227, 6,    227, 227, 6,    227, 227,  227, 227,  227,  227, 227, 227,  227,  227,  227, 227,  227, 227,  227,
    6,   227, 227, 227, 227, 227, 227,  227, 227,  227, 227, 227,  227, 227,  6,   6,    227,  227, 227, 227,  227,  227,  4,   227,  227, 227,  227,
    4,   6,   227, 227, 227, 227, 227,  6,   227,  227, 227, 227,  227, 227,  6,   228,  228,  228, 227, 227,  227,  227,  227, 227,  227, 227,  4,
    228, 228, 227, 227, 6,   6,   47,   227, 227,  39,  39,  226,  39,  3,    39,  39,   39,   226, 3,   3,    409,  226,  65,  227,  227, 227,  22,
    681, 245, 140, 371, 638, 827, 234,  205, 770,  963, 471, 433,  804, 495,  789, 804,  497,  281, 968, 772,  926,  821,  -1,  -1,   804};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] = {
    0,   7,   9,   10,  11,  12,  13,  14,  15,  16,  17,  23,  26,  28,  29,  30,  78,  174, 190, 202, 212, 213, 226, 236, 237, 238, 240, 252, 253,
    255, 256, 259, 260, 261, 262, 263, 287, 292, 293, 294, 295, 296, 297, 298, 299, 32,  33,  34,  38,  39,  199, 36,  32,  33,  34,  38,  39,  199,
    3,   285, 76,  285, 171, 172, 176, 3,   239, 240, 252, 253, 256, 259, 260, 261, 292, 293, 294, 295, 296, 3,   23,  30,  32,  33,  35,  40,  41,
    166, 167, 168, 169, 171, 172, 175, 176, 177, 180, 182, 186, 187, 199, 200, 203, 204, 205, 209, 213, 214, 215, 3,   33,  33,  39,  164, 165, 166,
    182, 3,   285, 3,   288, 289, 177, 7,   12,  14,  16,  174, 194, 198, 201, 32,  33,  51,  33,  51,  164, 262, 263, 0,   225, 348, 20,  22,  27,
    281, 8,   264, 266, 72,  347, 347, 347, 347, 347, 349, 3,   285, 72,  346, 346, 346, 346, 346, 3,   229, 14,  285, 76,  77,  226, 3,   3,   3,
    239, 226, 73,  73,  3,   285, 6,   211, 178, 179, 178, 179, 3,   4,   77,  6,   3,   206, 207, 208, 207, 210, 6,   73,  6,   73,  62,  285, 285,
    3,   55,  228, 6,   199, 199, 190, 191, 195, 165, 175, 178, 179, 180, 182, 196, 197, 199, 200, 199, 4,   197, 76,  199, 199, 199, 285, 227, 227,
    238, 21,  226, 265, 266, 65,  273, 67,  267, 73,  3,   285, 285, 285, 3,   62,  62,  226, 254, 74,  3,   285, 285, 285, 3,   3,   3,   257, 258,
    66,  278, 4,   345, 345, 3,   4,   5,   6,   73,  74,  83,  85,  90,  102, 103, 104, 108, 137, 138, 139, 140, 142, 144, 146, 148, 150, 152, 153,
    154, 162, 184, 186, 188, 220, 221, 222, 226, 231, 233, 300, 302, 303, 304, 305, 306, 307, 308, 309, 310, 313, 314, 315, 316, 317, 319, 320, 321,
    322, 323, 325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 337, 338, 339, 340, 341, 342, 3,   5,   6,   62,  173, 3,   5,   6,   62,  173, 3,
    5,   6,   62,  173, 227, 132, 132, 39,  42,  43,  47,  48,  3,   3,   345, 4,   132, 132, 285, 10,  24,  25,  62,  226, 289, 345, 4,   170, 6,
    3,   6,   4,   3,   4,   55,  4,   199, 3,   3,   3,   265, 266, 300, 53,  68,  271, 74,  135, 55,  226, 254, 285, 32,  33,  51,  3,   251, 37,
    263, 62,  217, 228, 278, 303, 78,  78,  226, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 73,  74,  304, 226, 226, 88,  303, 318,
    4,   4,   4,   4,   4,   6,   342, 226, 226, 226, 226, 226, 226, 226, 226, 226, 226, 122, 124, 125, 128, 226, 226, 304, 304, 263, 303, 5,   6,
    230, 323, 335, 336, 232, 322, 227, 228, 55,  160, 161, 73,  75,  133, 155, 156, 157, 158, 159, 163, 217, 218, 219, 220, 221, 222, 223, 224, 229,
    228, 230, 228, 230, 228, 230, 228, 232, 228, 230, 228, 230, 3,   6,   78,  350, 46,  46,  77,  285, 264, 4,   39,  48,  6,   77,  192, 193, 4,
    227, 227, 82,  274, 268, 269, 303, 303, 69,  272, 4,   261, 3,   129, 131, 241, 243, 244, 250, 55,  226, 354, 3,   3,   227, 228, 226, 301, 285,
    303, 258, 226, 226, 65,  227, 300, 226, 73,  263, 303, 303, 318, 84,  86,  88,  303, 303, 303, 303, 303, 303, 3,   321, 321, 321, 4,   226, 226,
    226, 226, 4,   4,   227, 227, 234, 77,  302, 3,   303, 303, 75,  163, 226, 73,  132, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304,
    304, 304, 3,   222, 323, 6,   335, 6,   336, 322, 6,   5,   42,  44,  45,  226, 226, 226, 3,   227, 6,   32,  41,  4,   170, 170, 300, 76,  275,
    228, 70,  71,  270, 303, 135, 89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 108, 109, 110, 111, 112,
    113, 116, 119, 120, 121, 122, 123, 124, 125, 126, 246, 130, 226, 227, 228, 261, 3,   135, 3,   300, 228, 68,  69,  79,  80,  81,  189, 343, 344,
    343, 300, 227, 263, 227, 55,  87,  84,  86,  303, 303, 227, 227, 227, 227, 227, 227, 228, 228, 228, 76,  303, 321, 303, 4,   228, 277, 227, 228,
    5,   6,   345, 226, 304, 263, 300, 132, 160, 234, 234, 6,   6,   3,   352, 353, 251, 242, 244, 6,   4,   4,   226, 282, 283, 284, 285, 290, 183,
    276, 269, 4,   226, 226, 226, 226, 226, 226, 226, 226, 226, 73,  129, 131, 132, 247, 248, 350, 226, 251, 31,  351, 243, 227, 4,   227, 226, 6,
    6,   4,   3,   90,  6,   227, 228, 227, 227, 227, 246, 303, 303, 84,  87,  4,   4,   4,   304, 228, 228, 228, 228, 4,   66,  227, 4,   78,  263,
    300, 227, 227, 304, 49,  50,  46,  217, 227, 228, 227, 227, 228, 6,   261, 228, 56,  58,  59,  60,  61,  63,  64,  291, 3,   55,  286, 305, 306,
    307, 308, 309, 310, 311, 312, 278, 6,   246, 245, 246, 91,  92,  93,  94,  95,  98,  99,  105, 106, 107, 127, 91,  92,  93,  94,  95,  98,  99,
    105, 106, 107, 127, 91,  92,  93,  94,  95,  98,  99,  105, 106, 107, 127, 91,  92,  93,  94,  95,  98,  99,  105, 106, 107, 127, 91,  92,  93,
    94,  95,  98,  99,  105, 106, 107, 127, 91,  92,  93,  94,  95,  98,  99,  105, 106, 107, 127, 132, 130, 134, 248, 249, 249, 251, 227, 226, 135,
    170, 300, 344, 227, 84,  303, 227, 227, 227, 227, 303, 323, 231, 324, 327, 332, 337, 4,   228, 277, 303, 227, 226, 227, 227, 6,   6,   3,   4,
    5,   6,   353, 244, 33,  35,  227, 283, 57,  57,  3,   228, 52,  280, 227, 228, 227, 227, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 93,
    228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 93,  228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 93,  228, 228, 228, 228, 228, 228, 228,
    228, 228, 228, 93,  228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 93,  228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 93,  228, 322, 135,
    135, 227, 352, 4,   3,   227, 228, 228, 228, 228, 277, 227, 343, 6,   286, 284, 284, 226, 311, 53,  54,  279, 6,   246, 6,   6,   6,   6,   6,
    6,   6,   6,   6,   228, 6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   228, 6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   228, 6,   6,
    6,   6,   6,   6,   6,   6,   6,   6,   228, 6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   228, 6,   6,   6,   6,   6,   6,   6,   6,   6,
    6,   228, 6,   4,   4,   227, 350, 4,   4,   4,   4,   227, 227, 6,   62,  251, 300, 303, 227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 6,
    227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 6,   227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 6,   227, 227, 227, 227, 227, 227, 227,
    227, 227, 227, 6,   227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 6,   227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 6,   227, 228, 228,
    228, 277, 277, 39,  42,  43,  47,  48,  303, 227, 227, 227, 227, 227, 227, 227, 4,   4,   6,   227, 227, 6,   6,   228, 277, 228, 277, 350, 6,
    44,  45,  6,   227, 4,   227, 42,  43,  6,   277, 350, 277, 136, 170, 350, 6,   47,  227, 227, 39,  39,  136, 170, 350, 136, 170, 226, 39,  39,
    39,  39,  3,   226, 226, 227, 3,   3,   350, 227, 227, 350};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] = {
    0,   235, 236, 237, 237, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 239, 239, 239, 239, 239, 239, 239, 239,
    239, 239, 239, 239, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 241, 241, 242, 242, 243, 243, 244, 244, 244, 244, 245, 245, 246, 246, 246,
    246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246,
    246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246,
    246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246,
    246, 246, 246, 246, 246, 247, 247, 248, 248, 248, 248, 249, 249, 250, 250, 251, 251, 252, 253, 253, 254, 254, 255, 255, 256, 257, 257, 258, 259,
    259, 259, 259, 259, 260, 260, 260, 261, 261, 261, 261, 262, 262, 263, 264, 265, 265, 266, 267, 267, 268, 268, 269, 270, 270, 270, 271, 271, 272,
    272, 273, 273, 274, 274, 275, 275, 276, 276, 277, 277, 278, 278, 279, 279, 280, 280, 281, 281, 281, 281, 282, 282, 283, 283, 284, 284, 285, 285,
    286, 286, 286, 286, 287, 287, 288, 288, 289, 290, 290, 291, 291, 291, 291, 291, 291, 291, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292,
    292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292,
    292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 292, 293, 293, 293, 293, 294, 295, 295, 295, 295, 295, 295, 295, 295,
    295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 295, 296, 297, 297, 297, 297, 297, 297, 297, 297, 297, 297,
    297, 297, 297, 297, 297, 297, 297, 297, 297, 297, 297, 297, 297, 297, 297, 297, 297, 297, 297, 297, 297, 297, 297, 297, 297, 297, 298, 298, 298,
    298, 299, 299, 300, 300, 301, 301, 302, 302, 303, 303, 303, 303, 303, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 305, 305,
    305, 306, 306, 306, 306, 307, 307, 307, 307, 308, 308, 309, 309, 310, 310, 311, 311, 311, 311, 311, 311, 312, 312, 313, 313, 313, 313, 313, 313,
    313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 313, 314, 314, 315,
    316, 316, 317, 317, 317, 317, 318, 318, 319, 320, 320, 320, 320, 321, 321, 321, 321, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322,
    322, 322, 323, 323, 323, 323, 324, 324, 324, 325, 326, 326, 327, 327, 328, 329, 329, 330, 331, 331, 332, 333, 333, 334, 334, 335, 336, 337, 337,
    338, 339, 339, 340, 341, 341, 342, 342, 342, 342, 342, 342, 342, 342, 342, 342, 342, 342, 343, 343, 344, 344, 344, 344, 344, 344, 344, 345, 346,
    346, 347, 347, 348, 348, 349, 349, 350, 350, 351, 351, 352, 352, 353, 353, 353, 353, 353, 354, 354};

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
    13, 18, 14, 13, 11, 8, 10, 5, 7, 4, 6, 1, 1, 1,  1,  1,  1,  1,  3,  3,  4,  5,  6,  6,  6, 4, 4, 4, 4, 4, 4, 4, 3, 2, 2, 2,  3,  3,  3,  3,  3,
    3,  3,  3,  3,  3,  3, 3,  6, 3, 4, 3, 3, 5, 5,  6,  4,  6,  3,  5,  4,  5,  6,  4,  5,  5, 6, 1, 3, 1, 3, 1, 1, 1, 1, 1, 2,  2,  2,  2,  2,  2,
    1,  1,  1,  1,  1,  1, 1,  1, 1, 1, 2, 2, 3, 1,  1,  2,  2,  3,  2,  2,  3,  2,  2,  2,  2, 3, 3, 3, 1, 1, 2, 2, 3, 2, 2, 3,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2, 2,  1, 3, 2, 2, 2, 1, 2,  2,  2,  1,  2,  0,  3,  0,  1,  0,  2,  0, 4, 0, 4, 0, 1, 3, 1, 3, 3, 3, 3,  6,  3};

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
#line 2516 "parser.cpp"
        break;

        case YYSYMBOL_STRING: /* STRING  */
#line 330 "parser.y"
        {
            free(((*yyvaluep).str_value));
        }
#line 2524 "parser.cpp"
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
#line 2538 "parser.cpp"
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
#line 2552 "parser.cpp"
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
#line 2566 "parser.cpp"
        break;

        case YYSYMBOL_column_type_array: /* column_type_array  */
#line 191 "parser.y"
        {
            fprintf(stderr, "destroy column_type_array\n");
            delete (((*yyvaluep).column_type_array_t));
        }
#line 2575 "parser.cpp"
        break;

        case YYSYMBOL_column_type: /* column_type  */
#line 186 "parser.y"
        {
            fprintf(stderr, "destroy column_type\n");
            delete (((*yyvaluep).column_type_t));
        }
#line 2584 "parser.cpp"
        break;

        case YYSYMBOL_column_constraints: /* column_constraints  */
#line 323 "parser.y"
        {
            fprintf(stderr, "destroy constraints\n");
            if ((((*yyvaluep).column_constraints_t)) != nullptr) {
                delete (((*yyvaluep).column_constraints_t));
            }
        }
#line 2595 "parser.cpp"
        break;

        case YYSYMBOL_default_expr: /* default_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 2603 "parser.cpp"
        break;

        case YYSYMBOL_identifier_array: /* identifier_array  */
#line 334 "parser.y"
        {
            fprintf(stderr, "destroy identifier array\n");
            delete (((*yyvaluep).identifier_array_t));
        }
#line 2612 "parser.cpp"
        break;

        case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 334 "parser.y"
        {
            fprintf(stderr, "destroy identifier array\n");
            delete (((*yyvaluep).identifier_array_t));
        }
#line 2621 "parser.cpp"
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
#line 2635 "parser.cpp"
        break;

        case YYSYMBOL_update_expr: /* update_expr  */
#line 287 "parser.y"
        {
            fprintf(stderr, "destroy update expr\n");
            if (((*yyvaluep).update_expr_t) != nullptr) {
                delete ((*yyvaluep).update_expr_t);
            }
        }
#line 2646 "parser.cpp"
        break;

        case YYSYMBOL_select_statement: /* select_statement  */
#line 369 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2656 "parser.cpp"
        break;

        case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 369 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2666 "parser.cpp"
        break;

        case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 369 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2676 "parser.cpp"
        break;

        case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 369 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2686 "parser.cpp"
        break;

        case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 369 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2696 "parser.cpp"
        break;

        case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 369 "parser.y"
        {
            if (((*yyvaluep).select_stmt) != nullptr) {
                delete ((*yyvaluep).select_stmt);
            }
        }
#line 2706 "parser.cpp"
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
#line 2720 "parser.cpp"
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
#line 2734 "parser.cpp"
        break;

        case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 357 "parser.y"
        {
            fprintf(stderr, "destroy order by expr\n");
            delete ((*yyvaluep).order_by_expr_t)->expr_;
            delete ((*yyvaluep).order_by_expr_t);
        }
#line 2744 "parser.cpp"
        break;

        case YYSYMBOL_limit_expr: /* limit_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2752 "parser.cpp"
        break;

        case YYSYMBOL_offset_expr: /* offset_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2760 "parser.cpp"
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
#line 2774 "parser.cpp"
        break;

        case YYSYMBOL_from_clause: /* from_clause  */
#line 352 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2783 "parser.cpp"
        break;

        case YYSYMBOL_search_clause: /* search_clause  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2791 "parser.cpp"
        break;

        case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2799 "parser.cpp"
        break;

        case YYSYMBOL_where_clause: /* where_clause  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2807 "parser.cpp"
        break;

        case YYSYMBOL_having_clause: /* having_clause  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2815 "parser.cpp"
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
#line 2829 "parser.cpp"
        break;

        case YYSYMBOL_table_reference: /* table_reference  */
#line 352 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2838 "parser.cpp"
        break;

        case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 352 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2847 "parser.cpp"
        break;

        case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 352 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2856 "parser.cpp"
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
#line 2869 "parser.cpp"
        break;

        case YYSYMBOL_table_alias: /* table_alias  */
#line 347 "parser.y"
        {
            fprintf(stderr, "destroy table alias\n");
            delete (((*yyvaluep).table_alias_t));
        }
#line 2878 "parser.cpp"
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
#line 2892 "parser.cpp"
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
#line 2906 "parser.cpp"
        break;

        case YYSYMBOL_with_expr: /* with_expr  */
#line 363 "parser.y"
        {
            fprintf(stderr, "destroy with expr\n");
            delete ((*yyvaluep).with_expr_t)->select_;
            delete ((*yyvaluep).with_expr_t);
        }
#line 2916 "parser.cpp"
        break;

        case YYSYMBOL_join_clause: /* join_clause  */
#line 352 "parser.y"
        {
            fprintf(stderr, "destroy table reference\n");
            delete (((*yyvaluep).table_reference_t));
        }
#line 2925 "parser.cpp"
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
#line 2939 "parser.cpp"
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
#line 2953 "parser.cpp"
        break;

        case YYSYMBOL_expr_alias: /* expr_alias  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2961 "parser.cpp"
        break;

        case YYSYMBOL_expr: /* expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2969 "parser.cpp"
        break;

        case YYSYMBOL_operand: /* operand  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2977 "parser.cpp"
        break;

        case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2985 "parser.cpp"
        break;

        case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 2993 "parser.cpp"
        break;

        case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3001 "parser.cpp"
        break;

        case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3009 "parser.cpp"
        break;

        case YYSYMBOL_query_expr: /* query_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3017 "parser.cpp"
        break;

        case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3025 "parser.cpp"
        break;

        case YYSYMBOL_sub_search: /* sub_search  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3033 "parser.cpp"
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
#line 3047 "parser.cpp"
        break;

        case YYSYMBOL_function_expr: /* function_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3055 "parser.cpp"
        break;

        case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3063 "parser.cpp"
        break;

        case YYSYMBOL_between_expr: /* between_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3071 "parser.cpp"
        break;

        case YYSYMBOL_in_expr: /* in_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3079 "parser.cpp"
        break;

        case YYSYMBOL_case_expr: /* case_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3087 "parser.cpp"
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
#line 3100 "parser.cpp"
        break;

        case YYSYMBOL_cast_expr: /* cast_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3108 "parser.cpp"
        break;

        case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3116 "parser.cpp"
        break;

        case YYSYMBOL_column_expr: /* column_expr  */
#line 339 "parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 3124 "parser.cpp"
        break;

        case YYSYMBOL_constant_expr: /* constant_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3132 "parser.cpp"
        break;

        case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3140 "parser.cpp"
        break;

        case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3148 "parser.cpp"
        break;

        case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3156 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3164 "parser.cpp"
        break;

        case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3172 "parser.cpp"
        break;

        case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3180 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3188 "parser.cpp"
        break;

        case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3196 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3204 "parser.cpp"
        break;

        case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3212 "parser.cpp"
        break;

        case YYSYMBOL_curly_brackets_expr: /* curly_brackets_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3220 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_curly_brackets_expr: /* unclosed_curly_brackets_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3228 "parser.cpp"
        break;

        case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 384 "parser.y"
        {
            delete (((*yyvaluep).int_sparse_ele_t));
        }
#line 3236 "parser.cpp"
        break;

        case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 388 "parser.y"
        {
            delete (((*yyvaluep).float_sparse_ele_t));
        }
#line 3244 "parser.cpp"
        break;

        case YYSYMBOL_array_expr: /* array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3252 "parser.cpp"
        break;

        case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3260 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3268 "parser.cpp"
        break;

        case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3276 "parser.cpp"
        break;

        case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3284 "parser.cpp"
        break;

        case YYSYMBOL_interval_expr: /* interval_expr  */
#line 343 "parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 3292 "parser.cpp"
        break;

        case YYSYMBOL_file_path: /* file_path  */
#line 330 "parser.y"
        {
            free(((*yyvaluep).str_value));
        }
#line 3300 "parser.cpp"
        break;

        case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 220 "parser.y"
        {
            fprintf(stderr, "destroy if not exists info\n");
            if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
                delete (((*yyvaluep).if_not_exists_info_t));
            }
        }
#line 3311 "parser.cpp"
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
#line 3325 "parser.cpp"
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
#line 3339 "parser.cpp"
        break;

        case YYSYMBOL_index_info: /* index_info  */
#line 196 "parser.y"
        {
            fprintf(stderr, "destroy index info\n");
            if ((((*yyvaluep).index_info_t)) != nullptr) {
                delete (((*yyvaluep).index_info_t));
            }
        }
#line 3350 "parser.cpp"
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

#line 3458 "parser.cpp"

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
#line 3673 "parser.cpp"
        break;

        case 3: /* statement_list: statement  */
#line 519 "parser.y"
        {
            (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
            yylloc.string_length = 0;
            (yyval.stmt_array) = new std::vector<infinity::BaseStatement *>();
            (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
        }
#line 3684 "parser.cpp"
        break;

        case 4: /* statement_list: statement_list ';' statement  */
#line 525 "parser.y"
        {
            (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
            yylloc.string_length = 0;
            (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
            (yyval.stmt_array) = (yyvsp[-2].stmt_array);
        }
#line 3695 "parser.cpp"
        break;

        case 5: /* statement: create_statement  */
#line 532 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].create_stmt);
        }
#line 3701 "parser.cpp"
        break;

        case 6: /* statement: drop_statement  */
#line 533 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].drop_stmt);
        }
#line 3707 "parser.cpp"
        break;

        case 7: /* statement: copy_statement  */
#line 534 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].copy_stmt);
        }
#line 3713 "parser.cpp"
        break;

        case 8: /* statement: show_statement  */
#line 535 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].show_stmt);
        }
#line 3719 "parser.cpp"
        break;

        case 9: /* statement: select_statement  */
#line 536 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].select_stmt);
        }
#line 3725 "parser.cpp"
        break;

        case 10: /* statement: delete_statement  */
#line 537 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].delete_stmt);
        }
#line 3731 "parser.cpp"
        break;

        case 11: /* statement: update_statement  */
#line 538 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].update_stmt);
        }
#line 3737 "parser.cpp"
        break;

        case 12: /* statement: insert_statement  */
#line 539 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].insert_stmt);
        }
#line 3743 "parser.cpp"
        break;

        case 13: /* statement: explain_statement  */
#line 540 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].explain_stmt);
        }
#line 3749 "parser.cpp"
        break;

        case 14: /* statement: flush_statement  */
#line 541 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].flush_stmt);
        }
#line 3755 "parser.cpp"
        break;

        case 15: /* statement: optimize_statement  */
#line 542 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].optimize_stmt);
        }
#line 3761 "parser.cpp"
        break;

        case 16: /* statement: command_statement  */
#line 543 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].command_stmt);
        }
#line 3767 "parser.cpp"
        break;

        case 17: /* statement: compact_statement  */
#line 544 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].compact_stmt);
        }
#line 3773 "parser.cpp"
        break;

        case 18: /* statement: admin_statement  */
#line 545 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].admin_stmt);
        }
#line 3779 "parser.cpp"
        break;

        case 19: /* statement: alter_statement  */
#line 546 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].alter_stmt);
        }
#line 3785 "parser.cpp"
        break;

        case 20: /* statement: check_statement  */
#line 547 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].check_stmt);
        }
#line 3791 "parser.cpp"
        break;

        case 21: /* explainable_statement: create_statement  */
#line 549 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].create_stmt);
        }
#line 3797 "parser.cpp"
        break;

        case 22: /* explainable_statement: drop_statement  */
#line 550 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].drop_stmt);
        }
#line 3803 "parser.cpp"
        break;

        case 23: /* explainable_statement: copy_statement  */
#line 551 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].copy_stmt);
        }
#line 3809 "parser.cpp"
        break;

        case 24: /* explainable_statement: show_statement  */
#line 552 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].show_stmt);
        }
#line 3815 "parser.cpp"
        break;

        case 25: /* explainable_statement: select_statement  */
#line 553 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].select_stmt);
        }
#line 3821 "parser.cpp"
        break;

        case 26: /* explainable_statement: delete_statement  */
#line 554 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].delete_stmt);
        }
#line 3827 "parser.cpp"
        break;

        case 27: /* explainable_statement: update_statement  */
#line 555 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].update_stmt);
        }
#line 3833 "parser.cpp"
        break;

        case 28: /* explainable_statement: insert_statement  */
#line 556 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].insert_stmt);
        }
#line 3839 "parser.cpp"
        break;

        case 29: /* explainable_statement: flush_statement  */
#line 557 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].flush_stmt);
        }
#line 3845 "parser.cpp"
        break;

        case 30: /* explainable_statement: optimize_statement  */
#line 558 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].optimize_stmt);
        }
#line 3851 "parser.cpp"
        break;

        case 31: /* explainable_statement: command_statement  */
#line 559 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].command_stmt);
        }
#line 3857 "parser.cpp"
        break;

        case 32: /* explainable_statement: compact_statement  */
#line 560 "parser.y"
        {
            (yyval.base_stmt) = (yyvsp[0].compact_stmt);
        }
#line 3863 "parser.cpp"
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
#line 3885 "parser.cpp"
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
#line 3905 "parser.cpp"
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
#line 3923 "parser.cpp"
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
#line 3956 "parser.cpp"
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
#line 3976 "parser.cpp"
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
#line 4012 "parser.cpp"
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
#line 4034 "parser.cpp"
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
#line 4055 "parser.cpp"
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
#line 4088 "parser.cpp"
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
#line 4123 "parser.cpp"
        break;

        case 43: /* table_element_array: table_element  */
#line 788 "parser.y"
        {
            (yyval.table_element_array_t) = new std::vector<infinity::TableElement *>();
            (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
        }
#line 4132 "parser.cpp"
        break;

        case 44: /* table_element_array: table_element_array ',' table_element  */
#line 792 "parser.y"
        {
            (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
            (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
        }
#line 4141 "parser.cpp"
        break;

        case 45: /* column_def_array: table_column  */
#line 797 "parser.y"
        {
            (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef *>();
            (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
        }
#line 4150 "parser.cpp"
        break;

        case 46: /* column_def_array: column_def_array ',' table_column  */
#line 801 "parser.y"
        {
            (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
            (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
        }
#line 4159 "parser.cpp"
        break;

        case 47: /* table_element: table_column  */
#line 807 "parser.y"
        {
            (yyval.table_element_t) = (yyvsp[0].table_column_t);
        }
#line 4167 "parser.cpp"
        break;

        case 48: /* table_element: table_constraint  */
#line 810 "parser.y"
        {
            (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
        }
#line 4175 "parser.cpp"
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
#line 4200 "parser.cpp"
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
#line 4227 "parser.cpp"
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
#line 4256 "parser.cpp"
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
#line 4286 "parser.cpp"
        break;

        case 53: /* column_type_array: column_type  */
#line 909 "parser.y"
        {
            (yyval.column_type_array_t) = new std::vector<std::unique_ptr<infinity::ColumnType>>();
            (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
        }
#line 4295 "parser.cpp"
        break;

        case 54: /* column_type_array: column_type_array ',' column_type  */
#line 913 "parser.y"
        {
            (yyval.column_type_array_t) = (yyvsp[-2].column_type_array_t);
            (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
        }
#line 4304 "parser.cpp"
        break;

        case 55: /* column_type: BOOLEAN  */
#line 919 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4310 "parser.cpp"
        break;

        case 56: /* column_type: JSON  */
#line 920 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kJson, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4316 "parser.cpp"
        break;

        case 57: /* column_type: TINYINT  */
#line 921 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4322 "parser.cpp"
        break;

        case 58: /* column_type: SMALLINT  */
#line 922 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4328 "parser.cpp"
        break;

        case 59: /* column_type: INTEGER  */
#line 923 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4334 "parser.cpp"
        break;

        case 60: /* column_type: INT  */
#line 924 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4340 "parser.cpp"
        break;

        case 61: /* column_type: BIGINT  */
#line 925 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4346 "parser.cpp"
        break;

        case 62: /* column_type: HUGEINT  */
#line 926 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4352 "parser.cpp"
        break;

        case 63: /* column_type: FLOAT  */
#line 927 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4358 "parser.cpp"
        break;

        case 64: /* column_type: REAL  */
#line 928 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4364 "parser.cpp"
        break;

        case 65: /* column_type: DOUBLE  */
#line 929 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4370 "parser.cpp"
        break;

        case 66: /* column_type: FLOAT16  */
#line 930 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4376 "parser.cpp"
        break;

        case 67: /* column_type: BFLOAT16  */
#line 931 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4382 "parser.cpp"
        break;

        case 68: /* column_type: DATE  */
#line 932 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4388 "parser.cpp"
        break;

        case 69: /* column_type: TIME  */
#line 933 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4394 "parser.cpp"
        break;

        case 70: /* column_type: DATETIME  */
#line 934 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4400 "parser.cpp"
        break;

        case 71: /* column_type: TIMESTAMP  */
#line 935 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4406 "parser.cpp"
        break;

        case 72: /* column_type: UUID  */
#line 936 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4412 "parser.cpp"
        break;

        case 73: /* column_type: POINT  */
#line 937 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4418 "parser.cpp"
        break;

        case 74: /* column_type: LINE  */
#line 938 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4424 "parser.cpp"
        break;

        case 75: /* column_type: LSEG  */
#line 939 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4430 "parser.cpp"
        break;

        case 76: /* column_type: BOX  */
#line 940 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4436 "parser.cpp"
        break;

        case 77: /* column_type: CIRCLE  */
#line 943 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4442 "parser.cpp"
        break;

        case 78: /* column_type: VARCHAR  */
#line 945 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4448 "parser.cpp"
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
#line 4454 "parser.cpp"
        break;

        case 80: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 947 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4460 "parser.cpp"
        break;

        case 81: /* column_type: DECIMAL  */
#line 948 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
        }
#line 4466 "parser.cpp"
        break;

        case 82: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 951 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4472 "parser.cpp"
        break;

        case 83: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 952 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4478 "parser.cpp"
        break;

        case 84: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 953 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4484 "parser.cpp"
        break;

        case 85: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 954 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4490 "parser.cpp"
        break;

        case 86: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 955 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4496 "parser.cpp"
        break;

        case 87: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 956 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4502 "parser.cpp"
        break;

        case 88: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 957 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4508 "parser.cpp"
        break;

        case 89: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 958 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4514 "parser.cpp"
        break;

        case 90: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 959 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4520 "parser.cpp"
        break;

        case 91: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 960 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4526 "parser.cpp"
        break;

        case 92: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 961 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4532 "parser.cpp"
        break;

        case 93: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 962 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4538 "parser.cpp"
        break;

        case 94: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 963 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4544 "parser.cpp"
        break;

        case 95: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 964 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4550 "parser.cpp"
        break;

        case 96: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 965 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4556 "parser.cpp"
        break;

        case 97: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 966 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4562 "parser.cpp"
        break;

        case 98: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 967 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4568 "parser.cpp"
        break;

        case 99: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 968 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4574 "parser.cpp"
        break;

        case 100: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 969 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4580 "parser.cpp"
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
#line 4586 "parser.cpp"
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
#line 4592 "parser.cpp"
        break;

        case 103: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 972 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4598 "parser.cpp"
        break;

        case 104: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 973 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4604 "parser.cpp"
        break;

        case 105: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 974 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4610 "parser.cpp"
        break;

        case 106: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 975 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4616 "parser.cpp"
        break;

        case 107: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 976 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4622 "parser.cpp"
        break;

        case 108: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 977 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4628 "parser.cpp"
        break;

        case 109: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 978 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4634 "parser.cpp"
        break;

        case 110: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 979 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4640 "parser.cpp"
        break;

        case 111: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 980 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4646 "parser.cpp"
        break;

        case 112: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 981 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4652 "parser.cpp"
        break;

        case 113: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 982 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4658 "parser.cpp"
        break;

        case 114: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 983 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4664 "parser.cpp"
        break;

        case 115: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 984 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4670 "parser.cpp"
        break;

        case 116: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 985 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4676 "parser.cpp"
        break;

        case 117: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 986 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4682 "parser.cpp"
        break;

        case 118: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 987 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4688 "parser.cpp"
        break;

        case 119: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 988 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4694 "parser.cpp"
        break;

        case 120: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 989 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4700 "parser.cpp"
        break;

        case 121: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 990 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4706 "parser.cpp"
        break;

        case 122: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 991 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4712 "parser.cpp"
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
#line 4718 "parser.cpp"
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
#line 4724 "parser.cpp"
        break;

        case 125: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 994 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4730 "parser.cpp"
        break;

        case 126: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 995 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4736 "parser.cpp"
        break;

        case 127: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 996 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4742 "parser.cpp"
        break;

        case 128: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 997 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4748 "parser.cpp"
        break;

        case 129: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 998 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4754 "parser.cpp"
        break;

        case 130: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 999 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4760 "parser.cpp"
        break;

        case 131: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1000 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4766 "parser.cpp"
        break;

        case 132: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1001 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4772 "parser.cpp"
        break;

        case 133: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1002 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4778 "parser.cpp"
        break;

        case 134: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1003 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4784 "parser.cpp"
        break;

        case 135: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1004 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4790 "parser.cpp"
        break;

        case 136: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1005 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4796 "parser.cpp"
        break;

        case 137: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1006 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit};
        }
#line 4802 "parser.cpp"
        break;

        case 138: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1007 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8};
        }
#line 4808 "parser.cpp"
        break;

        case 139: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1008 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16};
        }
#line 4814 "parser.cpp"
        break;

        case 140: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1009 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4820 "parser.cpp"
        break;

        case 141: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32};
        }
#line 4826 "parser.cpp"
        break;

        case 142: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1011 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64};
        }
#line 4832 "parser.cpp"
        break;

        case 143: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1012 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat};
        }
#line 4838 "parser.cpp"
        break;

        case 144: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1013 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble};
        }
#line 4844 "parser.cpp"
        break;

        case 145: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1014 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16};
        }
#line 4850 "parser.cpp"
        break;

        case 146: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16};
        }
#line 4856 "parser.cpp"
        break;

        case 147: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
        {
            (yyval.column_type_t) =
                new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8};
        }
#line 4862 "parser.cpp"
        break;

        case 148: /* column_type: ARRAY '(' column_type ')'  */
#line 1017 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kArray, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
            (yyval.column_type_t)->element_types_.emplace_back((yyvsp[-1].column_type_t));
        }
#line 4871 "parser.cpp"
        break;

        case 149: /* column_type: TUPLE '(' column_type_array ')'  */
#line 1021 "parser.y"
        {
            (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTuple, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
            (yyval.column_type_t)->element_types_ = std::move(*((yyvsp[-1].column_type_array_t)));
            delete (yyvsp[-1].column_type_array_t);
        }
#line 4881 "parser.cpp"
        break;

        case 150: /* column_constraints: column_constraint  */
#line 1044 "parser.y"
        {
            (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
            (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
        }
#line 4890 "parser.cpp"
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
#line 4904 "parser.cpp"
        break;

        case 152: /* column_constraint: PRIMARY KEY  */
#line 1058 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
        }
#line 4912 "parser.cpp"
        break;

        case 153: /* column_constraint: UNIQUE  */
#line 1061 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
        }
#line 4920 "parser.cpp"
        break;

        case 154: /* column_constraint: NULLABLE  */
#line 1064 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
        }
#line 4928 "parser.cpp"
        break;

        case 155: /* column_constraint: NOT NULLABLE  */
#line 1067 "parser.y"
        {
            (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
        }
#line 4936 "parser.cpp"
        break;

        case 156: /* default_expr: DEFAULT constant_expr  */
#line 1071 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 4944 "parser.cpp"
        break;

        case 157: /* default_expr: %empty  */
#line 1074 "parser.y"
        {
            (yyval.const_expr_t) = nullptr;
        }
#line 4952 "parser.cpp"
        break;

        case 158: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1079 "parser.y"
        {
            (yyval.table_constraint_t) = new infinity::TableConstraint();
            (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
            (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
        }
#line 4962 "parser.cpp"
        break;

        case 159: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1084 "parser.y"
        {
            (yyval.table_constraint_t) = new infinity::TableConstraint();
            (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
            (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
        }
#line 4972 "parser.cpp"
        break;

        case 160: /* identifier_array: IDENTIFIER  */
#line 1091 "parser.y"
        {
            (yyval.identifier_array_t) = new std::vector<std::string>();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 4983 "parser.cpp"
        break;

        case 161: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1097 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
            (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
        }
#line 4994 "parser.cpp"
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
#line 5011 "parser.cpp"
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
#line 5054 "parser.cpp"
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
#line 5074 "parser.cpp"
        break;

        case 165: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1177 "parser.y"
        {
            (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
        }
#line 5082 "parser.cpp"
        break;

        case 166: /* optional_identifier_array: %empty  */
#line 1180 "parser.y"
        {
            (yyval.identifier_array_t) = nullptr;
        }
#line 5090 "parser.cpp"
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
#line 5113 "parser.cpp"
        break;

        case 168: /* explain_statement: EXPLAIN explainable_statement  */
#line 1204 "parser.y"
        {
            (yyval.explain_stmt) = new infinity::ExplainStatement();
            (yyval.explain_stmt)->type_ = infinity::ExplainType::kPhysical;
            (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
        }
#line 5123 "parser.cpp"
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
#line 5140 "parser.cpp"
        break;

        case 170: /* update_expr_array: update_expr  */
#line 1226 "parser.y"
        {
            (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr *>();
            (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
        }
#line 5149 "parser.cpp"
        break;

        case 171: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1230 "parser.y"
        {
            (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
            (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
        }
#line 5158 "parser.cpp"
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
#line 5170 "parser.cpp"
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
#line 5186 "parser.cpp"
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
#line 5204 "parser.cpp"
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
#line 5222 "parser.cpp"
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
#line 5240 "parser.cpp"
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
#line 5263 "parser.cpp"
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
#line 5321 "parser.cpp"
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
#line 5381 "parser.cpp"
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
#line 5433 "parser.cpp"
        break;

        case 181: /* select_statement: select_without_paren  */
#line 1488 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[0].select_stmt);
        }
#line 5441 "parser.cpp"
        break;

        case 182: /* select_statement: select_with_paren  */
#line 1491 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[0].select_stmt);
        }
#line 5449 "parser.cpp"
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
#line 5463 "parser.cpp"
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
#line 5477 "parser.cpp"
        break;

        case 185: /* select_with_paren: '(' select_without_paren ')'  */
#line 1513 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5485 "parser.cpp"
        break;

        case 186: /* select_with_paren: '(' select_with_paren ')'  */
#line 1516 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5493 "parser.cpp"
        break;

        case 187: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1520 "parser.y"
        {
            (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
            (yyval.select_stmt) = (yyvsp[0].select_stmt);
        }
#line 5502 "parser.cpp"
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
#line 5533 "parser.cpp"
        break;

        case 189: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1552 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5541 "parser.cpp"
        break;

        case 190: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1555 "parser.y"
        {
            (yyval.select_stmt) = (yyvsp[-1].select_stmt);
        }
#line 5549 "parser.cpp"
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
#line 5570 "parser.cpp"
        break;

        case 192: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1577 "parser.y"
        {
            (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
        }
#line 5578 "parser.cpp"
        break;

        case 193: /* order_by_clause: %empty  */
#line 1580 "parser.y"
        {
            (yyval.order_by_expr_list_t) = nullptr;
        }
#line 5586 "parser.cpp"
        break;

        case 194: /* order_by_expr_list: order_by_expr  */
#line 1584 "parser.y"
        {
            (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr *>();
            (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
        }
#line 5595 "parser.cpp"
        break;

        case 195: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1588 "parser.y"
        {
            (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
            (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
        }
#line 5604 "parser.cpp"
        break;

        case 196: /* order_by_expr: expr order_by_type  */
#line 1593 "parser.y"
        {
            (yyval.order_by_expr_t) = new infinity::OrderByExpr();
            (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
            (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
        }
#line 5614 "parser.cpp"
        break;

        case 197: /* order_by_type: ASC  */
#line 1599 "parser.y"
        {
            (yyval.order_by_type_t) = infinity::kAsc;
        }
#line 5622 "parser.cpp"
        break;

        case 198: /* order_by_type: DESC  */
#line 1602 "parser.y"
        {
            (yyval.order_by_type_t) = infinity::kDesc;
        }
#line 5630 "parser.cpp"
        break;

        case 199: /* order_by_type: %empty  */
#line 1605 "parser.y"
        {
            (yyval.order_by_type_t) = infinity::kAsc;
        }
#line 5638 "parser.cpp"
        break;

        case 200: /* limit_expr: LIMIT expr  */
#line 1609 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5646 "parser.cpp"
        break;

        case 201: /* limit_expr: %empty  */
#line 1613 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5652 "parser.cpp"
        break;

        case 202: /* offset_expr: OFFSET expr  */
#line 1615 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5660 "parser.cpp"
        break;

        case 203: /* offset_expr: %empty  */
#line 1619 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5666 "parser.cpp"
        break;

        case 204: /* distinct: DISTINCT  */
#line 1621 "parser.y"
        {
            (yyval.bool_value) = true;
        }
#line 5674 "parser.cpp"
        break;

        case 205: /* distinct: %empty  */
#line 1624 "parser.y"
        {
            (yyval.bool_value) = false;
        }
#line 5682 "parser.cpp"
        break;

        case 206: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1628 "parser.y"
        {
            (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
        }
#line 5690 "parser.cpp"
        break;

        case 207: /* highlight_clause: %empty  */
#line 1631 "parser.y"
        {
            (yyval.expr_array_t) = nullptr;
        }
#line 5698 "parser.cpp"
        break;

        case 208: /* from_clause: FROM table_reference  */
#line 1635 "parser.y"
        {
            (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
        }
#line 5706 "parser.cpp"
        break;

        case 209: /* from_clause: %empty  */
#line 1638 "parser.y"
        {
            (yyval.table_reference_t) = nullptr;
        }
#line 5714 "parser.cpp"
        break;

        case 210: /* search_clause: SEARCH sub_search_array  */
#line 1642 "parser.y"
        {
            infinity::SearchExpr *search_expr = new infinity::SearchExpr();
            search_expr->SetExprs((yyvsp[0].expr_array_t));
            (yyval.expr_t) = search_expr;
        }
#line 5724 "parser.cpp"
        break;

        case 211: /* search_clause: %empty  */
#line 1647 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5732 "parser.cpp"
        break;

        case 212: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1651 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5740 "parser.cpp"
        break;

        case 213: /* optional_search_filter_expr: %empty  */
#line 1654 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5748 "parser.cpp"
        break;

        case 214: /* where_clause: WHERE expr  */
#line 1658 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5756 "parser.cpp"
        break;

        case 215: /* where_clause: %empty  */
#line 1661 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5764 "parser.cpp"
        break;

        case 216: /* having_clause: HAVING expr  */
#line 1665 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 5772 "parser.cpp"
        break;

        case 217: /* having_clause: %empty  */
#line 1668 "parser.y"
        {
            (yyval.expr_t) = nullptr;
        }
#line 5780 "parser.cpp"
        break;

        case 218: /* group_by_clause: GROUP BY expr_array  */
#line 1672 "parser.y"
        {
            (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
        }
#line 5788 "parser.cpp"
        break;

        case 219: /* group_by_clause: %empty  */
#line 1675 "parser.y"
        {
            (yyval.expr_array_t) = nullptr;
        }
#line 5796 "parser.cpp"
        break;

        case 220: /* set_operator: UNION  */
#line 1679 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
        }
#line 5804 "parser.cpp"
        break;

        case 221: /* set_operator: UNION ALL  */
#line 1682 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
        }
#line 5812 "parser.cpp"
        break;

        case 222: /* set_operator: INTERSECT  */
#line 1685 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
        }
#line 5820 "parser.cpp"
        break;

        case 223: /* set_operator: EXCEPT  */
#line 1688 "parser.y"
        {
            (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
        }
#line 5828 "parser.cpp"
        break;

        case 224: /* table_reference: table_reference_unit  */
#line 1696 "parser.y"
        {
            (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
        }
#line 5836 "parser.cpp"
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
#line 5854 "parser.cpp"
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
#line 5872 "parser.cpp"
        break;

        case 229: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1730 "parser.y"
        {
            infinity::SubqueryReference *subquery_reference = new infinity::SubqueryReference();
            subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
            subquery_reference->alias_ = (yyvsp[0].table_alias_t);
            (yyval.table_reference_t) = subquery_reference;
        }
#line 5883 "parser.cpp"
        break;

        case 230: /* table_name: IDENTIFIER  */
#line 1739 "parser.y"
        {
            (yyval.table_name_t) = new infinity::TableName();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
        }
#line 5893 "parser.cpp"
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
#line 5905 "parser.cpp"
        break;

        case 232: /* table_alias: AS IDENTIFIER  */
#line 1753 "parser.y"
        {
            (yyval.table_alias_t) = new infinity::TableAlias();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
        }
#line 5915 "parser.cpp"
        break;

        case 233: /* table_alias: IDENTIFIER  */
#line 1758 "parser.y"
        {
            (yyval.table_alias_t) = new infinity::TableAlias();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
        }
#line 5925 "parser.cpp"
        break;

        case 234: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1763 "parser.y"
        {
            (yyval.table_alias_t) = new infinity::TableAlias();
            ParserHelper::ToLower((yyvsp[-3].str_value));
            (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
            (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
        }
#line 5936 "parser.cpp"
        break;

        case 235: /* table_alias: %empty  */
#line 1769 "parser.y"
        {
            (yyval.table_alias_t) = nullptr;
        }
#line 5944 "parser.cpp"
        break;

        case 236: /* with_clause: WITH with_expr_list  */
#line 1776 "parser.y"
        {
            (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
        }
#line 5952 "parser.cpp"
        break;

        case 237: /* with_clause: %empty  */
#line 1779 "parser.y"
        {
            (yyval.with_expr_list_t) = nullptr;
        }
#line 5960 "parser.cpp"
        break;

        case 238: /* with_expr_list: with_expr  */
#line 1783 "parser.y"
        {
            (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr *>();
            (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
        }
#line 5969 "parser.cpp"
        break;

        case 239: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1786 "parser.y"
        {
            (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
            (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
        }
#line 5978 "parser.cpp"
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
#line 5990 "parser.cpp"
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
#line 6002 "parser.cpp"
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
#line 6015 "parser.cpp"
        break;

        case 243: /* join_type: INNER  */
#line 1824 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kInner;
        }
#line 6023 "parser.cpp"
        break;

        case 244: /* join_type: LEFT  */
#line 1827 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kLeft;
        }
#line 6031 "parser.cpp"
        break;

        case 245: /* join_type: RIGHT  */
#line 1830 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kRight;
        }
#line 6039 "parser.cpp"
        break;

        case 246: /* join_type: OUTER  */
#line 1833 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kFull;
        }
#line 6047 "parser.cpp"
        break;

        case 247: /* join_type: FULL  */
#line 1836 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kFull;
        }
#line 6055 "parser.cpp"
        break;

        case 248: /* join_type: CROSS  */
#line 1839 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kCross;
        }
#line 6063 "parser.cpp"
        break;

        case 249: /* join_type: %empty  */
#line 1842 "parser.y"
        {
            (yyval.join_type_t) = infinity::JoinType::kInner;
        }
#line 6071 "parser.cpp"
        break;

        case 250: /* show_statement: SHOW DATABASES  */
#line 1849 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
        }
#line 6080 "parser.cpp"
        break;

        case 251: /* show_statement: SHOW TABLES  */
#line 1853 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
        }
#line 6089 "parser.cpp"
        break;

        case 252: /* show_statement: SHOW TASKS  */
#line 1857 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTasks;
        }
#line 6098 "parser.cpp"
        break;

        case 253: /* show_statement: SHOW CONFIGS  */
#line 1861 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
        }
#line 6107 "parser.cpp"
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
#line 6119 "parser.cpp"
        break;

        case 255: /* show_statement: SHOW PROFILES  */
#line 1872 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
        }
#line 6128 "parser.cpp"
        break;

        case 256: /* show_statement: SHOW BUFFER  */
#line 1876 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
        }
#line 6137 "parser.cpp"
        break;

        case 257: /* show_statement: SHOW MEMINDEX  */
#line 1880 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
        }
#line 6146 "parser.cpp"
        break;

        case 258: /* show_statement: SHOW QUERIES  */
#line 1884 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
        }
#line 6155 "parser.cpp"
        break;

        case 259: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1888 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
            (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
        }
#line 6165 "parser.cpp"
        break;

        case 260: /* show_statement: SHOW TRANSACTIONS  */
#line 1893 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
        }
#line 6174 "parser.cpp"
        break;

        case 261: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1897 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
            (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
        }
#line 6184 "parser.cpp"
        break;

        case 262: /* show_statement: SHOW TRANSACTION HISTORY  */
#line 1902 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactionHistory;
        }
#line 6193 "parser.cpp"
        break;

        case 263: /* show_statement: SHOW SESSION VARIABLES  */
#line 1906 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
        }
#line 6202 "parser.cpp"
        break;

        case 264: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1910 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
        }
#line 6211 "parser.cpp"
        break;

        case 265: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1914 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
            (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 6222 "parser.cpp"
        break;

        case 266: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1920 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
            (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 6233 "parser.cpp"
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
#line 6245 "parser.cpp"
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
#line 6261 "parser.cpp"
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
#line 6277 "parser.cpp"
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
#line 6293 "parser.cpp"
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
#line 6310 "parser.cpp"
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
#line 6327 "parser.cpp"
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
#line 6345 "parser.cpp"
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
#line 6364 "parser.cpp"
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
#line 6380 "parser.cpp"
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
#line 6400 "parser.cpp"
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
#line 6422 "parser.cpp"
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
#line 6444 "parser.cpp"
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
#line 6467 "parser.cpp"
        break;

        case 280: /* show_statement: SHOW LOGS  */
#line 2095 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
        }
#line 6476 "parser.cpp"
        break;

        case 281: /* show_statement: SHOW CATALOG  */
#line 2099 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalog;
        }
#line 6485 "parser.cpp"
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
#line 6497 "parser.cpp"
        break;

        case 283: /* show_statement: SHOW CATALOG TO file_path  */
#line 2110 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogToFile;
            (yyval.show_stmt)->file_path_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 6508 "parser.cpp"
        break;

        case 284: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2116 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
        }
#line 6517 "parser.cpp"
        break;

        case 285: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2120 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
        }
#line 6526 "parser.cpp"
        break;

        case 286: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2124 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
            (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 6537 "parser.cpp"
        break;

        case 287: /* show_statement: SHOW MEMORY  */
#line 2130 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
        }
#line 6546 "parser.cpp"
        break;

        case 288: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2134 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
        }
#line 6555 "parser.cpp"
        break;

        case 289: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2138 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
        }
#line 6564 "parser.cpp"
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
#line 6576 "parser.cpp"
        break;

        case 291: /* show_statement: SHOW SNAPSHOTS  */
#line 2149 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListSnapshots;
        }
#line 6585 "parser.cpp"
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
#line 6597 "parser.cpp"
        break;

        case 293: /* show_statement: SHOW CACHES  */
#line 2160 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCaches;
        }
#line 6606 "parser.cpp"
        break;

        case 294: /* show_statement: SHOW CACHE  */
#line 2164 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowCache;
        }
#line 6615 "parser.cpp"
        break;

        case 295: /* show_statement: SHOW COMPACT  */
#line 2168 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCompact;
        }
#line 6624 "parser.cpp"
        break;

        case 296: /* show_statement: SHOW COMPACT NOT NULLABLE  */
#line 2172 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCompact;
            (yyval.show_stmt)->show_nullable_ = false;
        }
#line 6634 "parser.cpp"
        break;

        case 297: /* show_statement: SHOW CHECKPOINT  */
#line 2177 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCheckpoint;
        }
#line 6643 "parser.cpp"
        break;

        case 298: /* show_statement: SHOW CHECKPOINT NOT NULLABLE  */
#line 2181 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCheckpoint;
            (yyval.show_stmt)->show_nullable_ = false;
        }
#line 6653 "parser.cpp"
        break;

        case 299: /* show_statement: SHOW CHECKPOINT LONG_VALUE  */
#line 2186 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowCheckpoint;
            (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
        }
#line 6663 "parser.cpp"
        break;

        case 300: /* show_statement: SHOW OPTIMIZE  */
#line 2191 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListOptimize;
        }
#line 6672 "parser.cpp"
        break;

        case 301: /* show_statement: SHOW OPTIMIZE NOT NULLABLE  */
#line 2195 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListOptimize;
            (yyval.show_stmt)->show_nullable_ = false;
        }
#line 6682 "parser.cpp"
        break;

        case 302: /* show_statement: SHOW IMPORT  */
#line 2200 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListImport;
        }
#line 6691 "parser.cpp"
        break;

        case 303: /* show_statement: SHOW CLEAN  */
#line 2204 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListClean;
        }
#line 6700 "parser.cpp"
        break;

        case 304: /* show_statement: SHOW CLEAN NOT NULLABLE  */
#line 2208 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListClean;
            (yyval.show_stmt)->show_nullable_ = false;
        }
#line 6710 "parser.cpp"
        break;

        case 305: /* show_statement: SHOW CLEAN LONG_VALUE  */
#line 2213 "parser.y"
        {
            (yyval.show_stmt) = new infinity::ShowStatement();
            (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowClean;
            (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
        }
#line 6720 "parser.cpp"
        break;

        case 306: /* flush_statement: FLUSH DATA  */
#line 2222 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
        }
#line 6729 "parser.cpp"
        break;

        case 307: /* flush_statement: FLUSH LOG  */
#line 2226 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
        }
#line 6738 "parser.cpp"
        break;

        case 308: /* flush_statement: FLUSH BUFFER  */
#line 2230 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
        }
#line 6747 "parser.cpp"
        break;

        case 309: /* flush_statement: FLUSH CATALOG  */
#line 2234 "parser.y"
        {
            (yyval.flush_stmt) = new infinity::FlushStatement();
            (yyval.flush_stmt)->type_ = infinity::FlushType::kCatalog;
        }
#line 6756 "parser.cpp"
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
#line 6771 "parser.cpp"
        break;

        case 311: /* command_statement: USE IDENTIFIER  */
#line 2256 "parser.y"
        {
            (yyval.command_stmt) = new infinity::CommandStatement();
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
        }
#line 6782 "parser.cpp"
        break;

        case 312: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2262 "parser.y"
        {
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ =
                std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
            free((yyvsp[0].str_value));
        }
#line 6792 "parser.cpp"
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
#line 6803 "parser.cpp"
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
#line 6814 "parser.cpp"
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
#line 6827 "parser.cpp"
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
#line 6838 "parser.cpp"
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
#line 6849 "parser.cpp"
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
#line 6860 "parser.cpp"
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
#line 6871 "parser.cpp"
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
#line 6884 "parser.cpp"
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
#line 6895 "parser.cpp"
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
#line 6906 "parser.cpp"
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
#line 6917 "parser.cpp"
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
#line 6928 "parser.cpp"
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
#line 6941 "parser.cpp"
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
#line 6952 "parser.cpp"
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
#line 6963 "parser.cpp"
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
#line 6976 "parser.cpp"
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
#line 6989 "parser.cpp"
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
#line 7000 "parser.cpp"
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
#line 7011 "parser.cpp"
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
#line 7022 "parser.cpp"
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
#line 7033 "parser.cpp"
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
#line 7044 "parser.cpp"
        break;

        case 335: /* command_statement: CLEAN DATA  */
#line 2409 "parser.y"
        {
            (yyval.command_stmt) = new infinity::CommandStatement();
            (yyval.command_stmt)->command_info_ = std::make_shared<infinity::CleanupCmd>();
        }
#line 7053 "parser.cpp"
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
#line 7069 "parser.cpp"
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
#line 7086 "parser.cpp"
        break;

        case 338: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2438 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
            (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
            (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
        }
#line 7097 "parser.cpp"
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
#line 7109 "parser.cpp"
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
#line 7122 "parser.cpp"
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
#line 7136 "parser.cpp"
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
#line 7151 "parser.cpp"
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
#line 7166 "parser.cpp"
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
#line 7182 "parser.cpp"
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
#line 7198 "parser.cpp"
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
#line 7215 "parser.cpp"
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
#line 7232 "parser.cpp"
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
#line 7247 "parser.cpp"
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
#line 7263 "parser.cpp"
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
#line 7280 "parser.cpp"
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
#line 7298 "parser.cpp"
        break;

        case 352: /* admin_statement: ADMIN SHOW LOGS  */
#line 2580 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
        }
#line 7307 "parser.cpp"
        break;

        case 353: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2584 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
            (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
        }
#line 7317 "parser.cpp"
        break;

        case 354: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2589 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
            (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
        }
#line 7327 "parser.cpp"
        break;

        case 355: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2594 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
            (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
            (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
        }
#line 7338 "parser.cpp"
        break;

        case 356: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2600 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
        }
#line 7347 "parser.cpp"
        break;

        case 357: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2604 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
        }
#line 7356 "parser.cpp"
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
#line 7368 "parser.cpp"
        break;

        case 359: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2615 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
        }
#line 7377 "parser.cpp"
        break;

        case 360: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2619 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
        }
#line 7386 "parser.cpp"
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
#line 7398 "parser.cpp"
        break;

        case 362: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2630 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
            (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7409 "parser.cpp"
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
#line 7422 "parser.cpp"
        break;

        case 364: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2644 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
            (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7433 "parser.cpp"
        break;

        case 365: /* admin_statement: ADMIN SHOW NODES  */
#line 2650 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
        }
#line 7442 "parser.cpp"
        break;

        case 366: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2654 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
            (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7453 "parser.cpp"
        break;

        case 367: /* admin_statement: ADMIN SHOW NODE  */
#line 2660 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
        }
#line 7462 "parser.cpp"
        break;

        case 368: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2664 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
            (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7473 "parser.cpp"
        break;

        case 369: /* admin_statement: ADMIN SET ADMIN  */
#line 2670 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
            (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
        }
#line 7483 "parser.cpp"
        break;

        case 370: /* admin_statement: ADMIN SET STANDALONE  */
#line 2675 "parser.y"
        {
            (yyval.admin_stmt) = new infinity::AdminStatement();
            (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
            (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
        }
#line 7493 "parser.cpp"
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
#line 7505 "parser.cpp"
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
#line 7519 "parser.cpp"
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
#line 7533 "parser.cpp"
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
#line 7548 "parser.cpp"
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
#line 7565 "parser.cpp"
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
#line 7581 "parser.cpp"
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
#line 7604 "parser.cpp"
        break;

        case 378: /* check_statement: CHECK SYSTEM  */
#line 2758 "parser.y"
        {
            (yyval.check_stmt) = new infinity::CheckStatement();
            (yyval.check_stmt)->check_type_ = infinity::CheckStmtType::kSystem;
        }
#line 7613 "parser.cpp"
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
#line 7629 "parser.cpp"
        break;

        case 380: /* expr_array: expr_alias  */
#line 2778 "parser.y"
        {
            (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr *>();
            (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
        }
#line 7638 "parser.cpp"
        break;

        case 381: /* expr_array: expr_array ',' expr_alias  */
#line 2782 "parser.y"
        {
            (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
        }
#line 7647 "parser.cpp"
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
#line 7662 "parser.cpp"
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
#line 7677 "parser.cpp"
        break;

        case 384: /* expr_alias: expr AS IDENTIFIER  */
#line 2819 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[-2].expr_t);
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 7688 "parser.cpp"
        break;

        case 385: /* expr_alias: expr  */
#line 2825 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 7696 "parser.cpp"
        break;

        case 391: /* operand: '(' expr ')'  */
#line 2835 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[-1].expr_t);
        }
#line 7704 "parser.cpp"
        break;

        case 392: /* operand: '(' select_without_paren ')'  */
#line 2838 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 7715 "parser.cpp"
        break;

        case 393: /* operand: constant_expr  */
#line 2844 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].const_expr_t);
        }
#line 7723 "parser.cpp"
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
#line 7743 "parser.cpp"
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
#line 7765 "parser.cpp"
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
#line 7786 "parser.cpp"
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
#line 7848 "parser.cpp"
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
#line 7902 "parser.cpp"
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
#line 7960 "parser.cpp"
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
#line 8019 "parser.cpp"
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
#line 8048 "parser.cpp"
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
#line 8075 "parser.cpp"
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
#line 8100 "parser.cpp"
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
#line 8125 "parser.cpp"
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
#line 8139 "parser.cpp"
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
#line 8155 "parser.cpp"
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
#line 8167 "parser.cpp"
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
#line 8181 "parser.cpp"
        break;

        case 419: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3260 "parser.y"
        {
            infinity::FusionExpr *fusion_expr = new infinity::FusionExpr();
            fusion_expr->method_ = std::string((yyvsp[-1].str_value));
            free((yyvsp[-1].str_value));
            (yyval.expr_t) = fusion_expr;
        }
#line 8192 "parser.cpp"
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
#line 8208 "parser.cpp"
        break;

        case 421: /* sub_search: match_vector_expr  */
#line 3278 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8216 "parser.cpp"
        break;

        case 422: /* sub_search: match_text_expr  */
#line 3281 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8224 "parser.cpp"
        break;

        case 423: /* sub_search: match_tensor_expr  */
#line 3284 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8232 "parser.cpp"
        break;

        case 424: /* sub_search: match_sparse_expr  */
#line 3287 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8240 "parser.cpp"
        break;

        case 425: /* sub_search: query_expr  */
#line 3290 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8248 "parser.cpp"
        break;

        case 426: /* sub_search: fusion_expr  */
#line 3293 "parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 8256 "parser.cpp"
        break;

        case 427: /* sub_search_array: sub_search  */
#line 3297 "parser.y"
        {
            (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr *>();
            (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
        }
#line 8265 "parser.cpp"
        break;

        case 428: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3301 "parser.y"
        {
            (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
        }
#line 8274 "parser.cpp"
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
#line 8287 "parser.cpp"
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
#line 8300 "parser.cpp"
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
#line 8314 "parser.cpp"
        break;

        case 432: /* function_expr: JSON_EXTRACT '(' column_expr ',' STRING ')'  */
#line 3331 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "json_extract";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));

            infinity::JsonExtraInfo *extra_ptr = new infinity::JsonExtraInfo(infinity::ExtraInfoType::kJsonToString, (yyvsp[-1].str_value));
            if (!extra_ptr->Init()) {
                delete extra_ptr;
                yyerror(&yyloc, scanner, result, "Invalid json_extract format");
                YYERROR;
            }
            func_expr->extra_info_ = std::shared_ptr<infinity::BaseExtraInfo>(extra_ptr);
            (yyval.expr_t) = func_expr;
        }
#line 8334 "parser.cpp"
        break;

        case 433: /* function_expr: JSON_EXTRACT_STRING '(' column_expr ',' STRING ')'  */
#line 3346 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "json_extract_string";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));

            infinity::JsonExtraInfo *extra_ptr = new infinity::JsonExtraInfo(infinity::ExtraInfoType::kJsonToString, (yyvsp[-1].str_value));
            if (!extra_ptr->Init()) {
                delete extra_ptr;
                yyerror(&yyloc, scanner, result, "Invalid json_extract format");
                YYERROR;
            }
            func_expr->extra_info_ = std::shared_ptr<infinity::BaseExtraInfo>(extra_ptr);
            (yyval.expr_t) = func_expr;
        }
#line 8354 "parser.cpp"
        break;

        case 434: /* function_expr: JSON_EXTRACT_INT '(' column_expr ',' STRING ')'  */
#line 3361 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "json_extract_int";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));

            infinity::JsonExtraInfo *extra_ptr = new infinity::JsonExtraInfo(infinity::ExtraInfoType::kJsonToInt, (yyvsp[-1].str_value));
            if (!extra_ptr->Init()) {
                delete extra_ptr;
                yyerror(&yyloc, scanner, result, "Invalid json_extract format");
                YYERROR;
            }
            func_expr->extra_info_ = std::shared_ptr<infinity::BaseExtraInfo>(extra_ptr);
            (yyval.expr_t) = func_expr;
        }
#line 8374 "parser.cpp"
        break;

        case 435: /* function_expr: YEAR '(' expr ')'  */
#line 3376 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "year";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8386 "parser.cpp"
        break;

        case 436: /* function_expr: MONTH '(' expr ')'  */
#line 3383 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "month";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8398 "parser.cpp"
        break;

        case 437: /* function_expr: DAY '(' expr ')'  */
#line 3390 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "day";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8410 "parser.cpp"
        break;

        case 438: /* function_expr: HOUR '(' expr ')'  */
#line 3397 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "hour";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8422 "parser.cpp"
        break;

        case 439: /* function_expr: MINUTE '(' expr ')'  */
#line 3404 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "minute";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8434 "parser.cpp"
        break;

        case 440: /* function_expr: SECOND '(' expr ')'  */
#line 3411 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "second";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8446 "parser.cpp"
        break;

        case 441: /* function_expr: operand IS NOT NULLABLE  */
#line 3418 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "is_not_null";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8458 "parser.cpp"
        break;

        case 442: /* function_expr: operand IS NULLABLE  */
#line 3425 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "is_null";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8470 "parser.cpp"
        break;

        case 443: /* function_expr: NOT operand  */
#line 3432 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "not";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8482 "parser.cpp"
        break;

        case 444: /* function_expr: '-' operand  */
#line 3439 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "-";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8494 "parser.cpp"
        break;

        case 445: /* function_expr: '+' operand  */
#line 3446 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "+";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8506 "parser.cpp"
        break;

        case 446: /* function_expr: operand '-' operand  */
#line 3453 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "-";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8519 "parser.cpp"
        break;

        case 447: /* function_expr: operand '+' operand  */
#line 3461 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "+";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8532 "parser.cpp"
        break;

        case 448: /* function_expr: operand '*' operand  */
#line 3469 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "*";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8545 "parser.cpp"
        break;

        case 449: /* function_expr: operand '/' operand  */
#line 3477 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "/";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8558 "parser.cpp"
        break;

        case 450: /* function_expr: operand '%' operand  */
#line 3485 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "%";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8571 "parser.cpp"
        break;

        case 451: /* function_expr: operand '=' operand  */
#line 3493 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8584 "parser.cpp"
        break;

        case 452: /* function_expr: operand EQUAL operand  */
#line 3501 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8597 "parser.cpp"
        break;

        case 453: /* function_expr: operand NOT_EQ operand  */
#line 3509 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "<>";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8610 "parser.cpp"
        break;

        case 454: /* function_expr: operand '<' operand  */
#line 3517 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "<";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8623 "parser.cpp"
        break;

        case 455: /* function_expr: operand '>' operand  */
#line 3525 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = ">";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8636 "parser.cpp"
        break;

        case 456: /* function_expr: operand LESS_EQ operand  */
#line 3533 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "<=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8649 "parser.cpp"
        break;

        case 457: /* function_expr: operand GREATER_EQ operand  */
#line 3541 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = ">=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8662 "parser.cpp"
        break;

        case 458: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3549 "parser.y"
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
#line 8697 "parser.cpp"
        break;

        case 459: /* function_expr: operand LIKE operand  */
#line 3579 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "like";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8710 "parser.cpp"
        break;

        case 460: /* function_expr: operand NOT LIKE operand  */
#line 3587 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "not_like";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8723 "parser.cpp"
        break;

        case 461: /* conjunction_expr: expr AND expr  */
#line 3596 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "and";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8736 "parser.cpp"
        break;

        case 462: /* conjunction_expr: expr OR expr  */
#line 3604 "parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "or";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 8749 "parser.cpp"
        break;

        case 463: /* between_expr: operand BETWEEN operand AND operand  */
#line 3613 "parser.y"
        {
            infinity::BetweenExpr *between_expr = new infinity::BetweenExpr();
            between_expr->value_ = (yyvsp[-4].expr_t);
            between_expr->lower_bound_ = (yyvsp[-2].expr_t);
            between_expr->upper_bound_ = (yyvsp[0].expr_t);
            (yyval.expr_t) = between_expr;
        }
#line 8761 "parser.cpp"
        break;

        case 464: /* in_expr: operand IN '(' expr_array ')'  */
#line 3621 "parser.y"
        {
            infinity::InExpr *in_expr = new infinity::InExpr(true);
            in_expr->left_ = (yyvsp[-4].expr_t);
            in_expr->arguments_ = (yyvsp[-1].expr_array_t);
            (yyval.expr_t) = in_expr;
        }
#line 8772 "parser.cpp"
        break;

        case 465: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3627 "parser.y"
        {
            infinity::InExpr *in_expr = new infinity::InExpr(false);
            in_expr->left_ = (yyvsp[-5].expr_t);
            in_expr->arguments_ = (yyvsp[-1].expr_array_t);
            (yyval.expr_t) = in_expr;
        }
#line 8783 "parser.cpp"
        break;

        case 466: /* case_expr: CASE expr case_check_array END  */
#line 3634 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->expr_ = (yyvsp[-2].expr_t);
            case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
            (yyval.expr_t) = case_expr;
        }
#line 8794 "parser.cpp"
        break;

        case 467: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3640 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->expr_ = (yyvsp[-4].expr_t);
            case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
            case_expr->else_expr_ = (yyvsp[-1].expr_t);
            (yyval.expr_t) = case_expr;
        }
#line 8806 "parser.cpp"
        break;

        case 468: /* case_expr: CASE case_check_array END  */
#line 3647 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
            (yyval.expr_t) = case_expr;
        }
#line 8816 "parser.cpp"
        break;

        case 469: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3652 "parser.y"
        {
            infinity::CaseExpr *case_expr = new infinity::CaseExpr();
            case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
            case_expr->else_expr_ = (yyvsp[-1].expr_t);
            (yyval.expr_t) = case_expr;
        }
#line 8827 "parser.cpp"
        break;

        case 470: /* case_check_array: WHEN expr THEN expr  */
#line 3659 "parser.y"
        {
            (yyval.case_check_array_t) = new std::vector<infinity::WhenThen *>();
            infinity::WhenThen *when_then_ptr = new infinity::WhenThen();
            when_then_ptr->when_ = (yyvsp[-2].expr_t);
            when_then_ptr->then_ = (yyvsp[0].expr_t);
            (yyval.case_check_array_t)->emplace_back(when_then_ptr);
        }
#line 8839 "parser.cpp"
        break;

        case 471: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3666 "parser.y"
        {
            infinity::WhenThen *when_then_ptr = new infinity::WhenThen();
            when_then_ptr->when_ = (yyvsp[-2].expr_t);
            when_then_ptr->then_ = (yyvsp[0].expr_t);
            (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
            (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
        }
#line 8851 "parser.cpp"
        break;

        case 472: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3674 "parser.y"
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
#line 8868 "parser.cpp"
        break;

        case 473: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3687 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8879 "parser.cpp"
        break;

        case 474: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3693 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8890 "parser.cpp"
        break;

        case 475: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3699 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
            subquery_expr->left_ = (yyvsp[-4].expr_t);
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8902 "parser.cpp"
        break;

        case 476: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3706 "parser.y"
        {
            infinity::SubqueryExpr *subquery_expr = new infinity::SubqueryExpr();
            subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
            subquery_expr->left_ = (yyvsp[-5].expr_t);
            subquery_expr->select_ = (yyvsp[-1].select_stmt);
            (yyval.expr_t) = subquery_expr;
        }
#line 8914 "parser.cpp"
        break;

        case 477: /* column_expr: IDENTIFIER  */
#line 3714 "parser.y"
        {
            infinity::ColumnExpr *column_expr = new infinity::ColumnExpr();
            ParserHelper::ToLower((yyvsp[0].str_value));
            column_expr->names_.emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
            (yyval.expr_t) = column_expr;
        }
#line 8926 "parser.cpp"
        break;

        case 478: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3721 "parser.y"
        {
            infinity::ColumnExpr *column_expr = (infinity::ColumnExpr *)(yyvsp[-2].expr_t);
            ParserHelper::ToLower((yyvsp[0].str_value));
            column_expr->names_.emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
            (yyval.expr_t) = column_expr;
        }
#line 8938 "parser.cpp"
        break;

        case 479: /* column_expr: '*'  */
#line 3728 "parser.y"
        {
            infinity::ColumnExpr *column_expr = new infinity::ColumnExpr();
            column_expr->star_ = true;
            (yyval.expr_t) = column_expr;
        }
#line 8948 "parser.cpp"
        break;

        case 480: /* column_expr: column_expr '.' '*'  */
#line 3733 "parser.y"
        {
            infinity::ColumnExpr *column_expr = (infinity::ColumnExpr *)(yyvsp[-2].expr_t);
            if (column_expr->star_) {
                yyerror(&yyloc, scanner, result, "Invalid column expression format");
                YYERROR;
            }
            column_expr->star_ = true;
            (yyval.expr_t) = column_expr;
        }
#line 8962 "parser.cpp"
        break;

        case 481: /* constant_expr: STRING  */
#line 3743 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
            const_expr->str_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 8972 "parser.cpp"
        break;

        case 482: /* constant_expr: TRUE  */
#line 3748 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
            const_expr->bool_value_ = true;
            (yyval.const_expr_t) = const_expr;
        }
#line 8982 "parser.cpp"
        break;

        case 483: /* constant_expr: FALSE  */
#line 3753 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
            const_expr->bool_value_ = false;
            (yyval.const_expr_t) = const_expr;
        }
#line 8992 "parser.cpp"
        break;

        case 484: /* constant_expr: DOUBLE_VALUE  */
#line 3758 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
            const_expr->double_value_ = (yyvsp[0].double_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9002 "parser.cpp"
        break;

        case 485: /* constant_expr: LONG_VALUE  */
#line 3763 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
            const_expr->integer_value_ = (yyvsp[0].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9012 "parser.cpp"
        break;

        case 486: /* constant_expr: JSON STRING  */
#line 3768 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kJson);
            const_expr->json_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9022 "parser.cpp"
        break;

        case 487: /* constant_expr: DATE STRING  */
#line 3773 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9032 "parser.cpp"
        break;

        case 488: /* constant_expr: TIME STRING  */
#line 3778 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9042 "parser.cpp"
        break;

        case 489: /* constant_expr: DATETIME STRING  */
#line 3783 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9052 "parser.cpp"
        break;

        case 490: /* constant_expr: TIMESTAMP STRING  */
#line 3788 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9062 "parser.cpp"
        break;

        case 491: /* constant_expr: INTERVAL interval_expr  */
#line 3793 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9070 "parser.cpp"
        break;

        case 492: /* constant_expr: interval_expr  */
#line 3796 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9078 "parser.cpp"
        break;

        case 493: /* constant_expr: common_array_expr  */
#line 3799 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9086 "parser.cpp"
        break;

        case 494: /* constant_expr: curly_brackets_expr  */
#line 3802 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9094 "parser.cpp"
        break;

        case 495: /* common_array_expr: array_expr  */
#line 3806 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9102 "parser.cpp"
        break;

        case 496: /* common_array_expr: subarray_array_expr  */
#line 3809 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9110 "parser.cpp"
        break;

        case 497: /* common_array_expr: sparse_array_expr  */
#line 3812 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9118 "parser.cpp"
        break;

        case 498: /* common_array_expr: empty_array_expr  */
#line 3815 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9126 "parser.cpp"
        break;

        case 499: /* common_sparse_array_expr: sparse_array_expr  */
#line 3819 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9134 "parser.cpp"
        break;

        case 500: /* common_sparse_array_expr: array_expr  */
#line 3822 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9142 "parser.cpp"
        break;

        case 501: /* common_sparse_array_expr: empty_array_expr  */
#line 3825 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9150 "parser.cpp"
        break;

        case 502: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3829 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9158 "parser.cpp"
        break;

        case 503: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3833 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
            const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
            (yyval.const_expr_t) = const_expr;
        }
#line 9168 "parser.cpp"
        break;

        case 504: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3838 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9177 "parser.cpp"
        break;

        case 505: /* sparse_array_expr: long_sparse_array_expr  */
#line 3843 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9185 "parser.cpp"
        break;

        case 506: /* sparse_array_expr: double_sparse_array_expr  */
#line 3846 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9193 "parser.cpp"
        break;

        case 507: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3850 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9201 "parser.cpp"
        break;

        case 508: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3854 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
            const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
            const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
            delete (yyvsp[0].int_sparse_ele_t);
            (yyval.const_expr_t) = const_expr;
        }
#line 9213 "parser.cpp"
        break;

        case 509: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3861 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
            (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
            delete (yyvsp[0].int_sparse_ele_t);
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9224 "parser.cpp"
        break;

        case 510: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3868 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9232 "parser.cpp"
        break;

        case 511: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3872 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
            const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
            const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
            delete (yyvsp[0].float_sparse_ele_t);
            (yyval.const_expr_t) = const_expr;
        }
#line 9244 "parser.cpp"
        break;

        case 512: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3879 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
            (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
            delete (yyvsp[0].float_sparse_ele_t);
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9255 "parser.cpp"
        break;

        case 513: /* empty_array_expr: '[' ']'  */
#line 3886 "parser.y"
        {
            (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
        }
#line 9263 "parser.cpp"
        break;

        case 514: /* curly_brackets_expr: unclosed_curly_brackets_expr '}'  */
#line 3890 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9271 "parser.cpp"
        break;

        case 515: /* curly_brackets_expr: '{' '}'  */
#line 3893 "parser.y"
        {
            (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
        }
#line 9279 "parser.cpp"
        break;

        case 516: /* unclosed_curly_brackets_expr: '{' constant_expr  */
#line 3897 "parser.y"
        {
            (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
            (yyval.const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
        }
#line 9288 "parser.cpp"
        break;

        case 517: /* unclosed_curly_brackets_expr: unclosed_curly_brackets_expr ',' constant_expr  */
#line 3901 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9297 "parser.cpp"
        break;

        case 518: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3906 "parser.y"
        {
            (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
        }
#line 9305 "parser.cpp"
        break;

        case 519: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3910 "parser.y"
        {
            (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
        }
#line 9313 "parser.cpp"
        break;

        case 520: /* array_expr: long_array_expr  */
#line 3914 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9321 "parser.cpp"
        break;

        case 521: /* array_expr: double_array_expr  */
#line 3917 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 9329 "parser.cpp"
        break;

        case 522: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3921 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9337 "parser.cpp"
        break;

        case 523: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3925 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
            const_expr->long_array_.emplace_back((yyvsp[0].long_value));
            (yyval.const_expr_t) = const_expr;
        }
#line 9347 "parser.cpp"
        break;

        case 524: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3930 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9356 "parser.cpp"
        break;

        case 525: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3935 "parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 9364 "parser.cpp"
        break;

        case 526: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3939 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
            const_expr->double_array_.emplace_back((yyvsp[0].double_value));
            (yyval.const_expr_t) = const_expr;
        }
#line 9374 "parser.cpp"
        break;

        case 527: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3944 "parser.y"
        {
            (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 9383 "parser.cpp"
        break;

        case 528: /* interval_expr: LONG_VALUE SECONDS  */
#line 3949 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kSecond;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9394 "parser.cpp"
        break;

        case 529: /* interval_expr: LONG_VALUE SECOND  */
#line 3955 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kSecond;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9405 "parser.cpp"
        break;

        case 530: /* interval_expr: LONG_VALUE MINUTES  */
#line 3961 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMinute;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9416 "parser.cpp"
        break;

        case 531: /* interval_expr: LONG_VALUE MINUTE  */
#line 3967 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMinute;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9427 "parser.cpp"
        break;

        case 532: /* interval_expr: LONG_VALUE HOURS  */
#line 3973 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kHour;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9438 "parser.cpp"
        break;

        case 533: /* interval_expr: LONG_VALUE HOUR  */
#line 3979 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kHour;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9449 "parser.cpp"
        break;

        case 534: /* interval_expr: LONG_VALUE DAYS  */
#line 3985 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kDay;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9460 "parser.cpp"
        break;

        case 535: /* interval_expr: LONG_VALUE DAY  */
#line 3991 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kDay;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9471 "parser.cpp"
        break;

        case 536: /* interval_expr: LONG_VALUE MONTHS  */
#line 3997 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMonth;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9482 "parser.cpp"
        break;

        case 537: /* interval_expr: LONG_VALUE MONTH  */
#line 4003 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMonth;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9493 "parser.cpp"
        break;

        case 538: /* interval_expr: LONG_VALUE YEARS  */
#line 4009 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kYear;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9504 "parser.cpp"
        break;

        case 539: /* interval_expr: LONG_VALUE YEAR  */
#line 4015 "parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kYear;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 9515 "parser.cpp"
        break;

        case 540: /* copy_option_list: copy_option  */
#line 4026 "parser.y"
        {
            (yyval.copy_option_array) = new std::vector<infinity::CopyOption *>();
            (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
        }
#line 9524 "parser.cpp"
        break;

        case 541: /* copy_option_list: copy_option_list ',' copy_option  */
#line 4030 "parser.y"
        {
            (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
            (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
        }
#line 9533 "parser.cpp"
        break;

        case 542: /* copy_option: FORMAT JSON  */
#line 4035 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kFormat;
            (yyval.copy_option_t)->file_type_ = infinity::CopyFileType::kJSON;
        }
#line 9543 "parser.cpp"
        break;

        case 543: /* copy_option: FORMAT IDENTIFIER  */
#line 4040 "parser.y"
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
#line 9577 "parser.cpp"
        break;

        case 544: /* copy_option: DELIMITER STRING  */
#line 4069 "parser.y"
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
#line 9592 "parser.cpp"
        break;

        case 545: /* copy_option: HEADER  */
#line 4079 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
            (yyval.copy_option_t)->header_ = true;
        }
#line 9602 "parser.cpp"
        break;

        case 546: /* copy_option: OFFSET LONG_VALUE  */
#line 4084 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
            (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
        }
#line 9612 "parser.cpp"
        break;

        case 547: /* copy_option: LIMIT LONG_VALUE  */
#line 4089 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
            (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
        }
#line 9622 "parser.cpp"
        break;

        case 548: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 4094 "parser.y"
        {
            (yyval.copy_option_t) = new infinity::CopyOption();
            (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
            (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
        }
#line 9632 "parser.cpp"
        break;

        case 549: /* file_path: STRING  */
#line 4100 "parser.y"
        {
            (yyval.str_value) = (yyvsp[0].str_value);
        }
#line 9640 "parser.cpp"
        break;

        case 550: /* if_exists: IF EXISTS  */
#line 4104 "parser.y"
        {
            (yyval.bool_value) = true;
        }
#line 9646 "parser.cpp"
        break;

        case 551: /* if_exists: %empty  */
#line 4105 "parser.y"
        {
            (yyval.bool_value) = false;
        }
#line 9652 "parser.cpp"
        break;

        case 552: /* if_not_exists: IF NOT EXISTS  */
#line 4107 "parser.y"
        {
            (yyval.bool_value) = true;
        }
#line 9658 "parser.cpp"
        break;

        case 553: /* if_not_exists: %empty  */
#line 4108 "parser.y"
        {
            (yyval.bool_value) = false;
        }
#line 9664 "parser.cpp"
        break;

        case 556: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 4123 "parser.y"
        {
            (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
            (yyval.if_not_exists_info_t)->exists_ = true;
            (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 9677 "parser.cpp"
        break;

        case 557: /* if_not_exists_info: %empty  */
#line 4131 "parser.y"
        {
            (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
        }
#line 9685 "parser.cpp"
        break;

        case 558: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 4135 "parser.y"
        {
            (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
        }
#line 9693 "parser.cpp"
        break;

        case 559: /* with_index_param_list: %empty  */
#line 4138 "parser.y"
        {
            (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter *>();
        }
#line 9701 "parser.cpp"
        break;

        case 560: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 4142 "parser.y"
        {
            (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
        }
#line 9709 "parser.cpp"
        break;

        case 561: /* optional_table_properties_list: %empty  */
#line 4145 "parser.y"
        {
            (yyval.with_index_param_list_t) = nullptr;
        }
#line 9717 "parser.cpp"
        break;

        case 562: /* index_param_list: index_param  */
#line 4149 "parser.y"
        {
            (yyval.index_param_list_t) = new std::vector<infinity::InitParameter *>();
            (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
        }
#line 9726 "parser.cpp"
        break;

        case 563: /* index_param_list: index_param_list ',' index_param  */
#line 4153 "parser.y"
        {
            (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
            (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
        }
#line 9735 "parser.cpp"
        break;

        case 564: /* index_param: IDENTIFIER  */
#line 4158 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 9746 "parser.cpp"
        break;

        case 565: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 4164 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));

            (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 9761 "parser.cpp"
        break;

        case 566: /* index_param: IDENTIFIER '=' STRING  */
#line 4174 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));

            (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 9776 "parser.cpp"
        break;

        case 567: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 4184 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));

            (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
        }
#line 9789 "parser.cpp"
        break;

        case 568: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 4192 "parser.y"
        {
            ParserHelper::ToLower((yyvsp[-2].str_value));
            (yyval.index_param_t) = new infinity::InitParameter();
            (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));

            (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
        }
#line 9802 "parser.cpp"
        break;

        case 569: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 4203 "parser.y"
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
#line 9860 "parser.cpp"
        break;

        case 570: /* index_info: '(' IDENTIFIER ')'  */
#line 4256 "parser.y"
        {
            (yyval.index_info_t) = new infinity::IndexInfo();
            (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
            ParserHelper::ToLower((yyvsp[-1].str_value));
            (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
            free((yyvsp[-1].str_value));
        }
#line 9872 "parser.cpp"
        break;

#line 9876 "parser.cpp"

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

#line 4265 "parser.y"

void yyerror(YYLTYPE *llocp, void *lexer, infinity::ParserResult *result, const char *msg) {
    if (result->IsError())
        return;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
    fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}


%{
#include <stdint.h>
#include <stdio.h>
#include "parser.h"
#include "lexer.h"

void yyerror(YYLTYPE * llocp, void* lexer, ParserResult* result, const char* msg);
%}

%code requires {

#include "statement.h"
#include "expression.h"
#include "parser_result.h"
#include "defer_operation.h"
#include "parser/table_reference/join_reference.h"
#include "parser/table_reference/cross_product_reference.h"
#include "parser/table_reference/table_reference.h"
#include "parser/table_reference/subquery_reference.h"
#include "parser_helper.h"

#include <vector>

using namespace infinity;

#define YYSTYPE SQLSTYPE
#define YYLTYPE SQLLTYPE

struct SQL_LTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;

    int total_column;
    int string_length;

    std::vector<void*> parameters;
};

#define SQLLTYPE SQL_LTYPE
#define SQLLTYPE_IS_DECLARED 1

#define YY_USER_ACTION \
    yylloc->first_line = yylloc->last_line;      \
    yylloc->first_column = yylloc->last_column;  \
    for(int i = 0; yytext[i] != '\0'; ++ i) {   \
        yylloc->total_column++;                   \
        yylloc->string_length++;                  \
        if (yytext[i] == '\n') {                  \
            yylloc->last_line++;                    \
            yylloc->last_column = 0;                \
        } else {                                  \
            yylloc->last_column++;                  \
        }                                         \
    }
}


%define api.pure full

%define parse.trace
%define parse.error verbose

%define api.prefix {sql}

%lex-param {void *scanner}
%parse-param {void *scanner} {ParserResult* result}

%locations
%initial-action {
  // Initialize
  @$.first_column = 0;
  @$.last_column = 0;
  @$.first_line = 0;
  @$.last_line = 0;
  @$.total_column = 0;
  @$.string_length = 0;
};

%union {
    bool    bool_value;
    char*   str_value;
    double  double_value;
    int64_t long_value;

    BaseStatement*   base_stmt;
    SelectStatement* select_stmt;
    CopyStatement*   copy_stmt;
    InsertStatement* insert_stmt;
    UpdateStatement* update_stmt;
    DeleteStatement* delete_stmt;
    CreateStatement* create_stmt;
    DropStatement*   drop_stmt;
    PrepareStatement* prepare_stmt;
    ExecuteStatement* execute_stmt;
    AlterStatement*   alter_stmt;
    ShowStatement*    show_stmt;
    ExplainStatement* explain_stmt;

    Vector<BaseStatement*>* stmt_array;

    Vector<TableElement*>*  table_element_array_t;
    TableElement*           table_element_t;
    ColumnDef*              table_column_t;
    ColumnType              column_type_t;
    ConstraintType          column_constraint_t;
    HashSet<ConstraintType>* column_constraints_t;
    Vector<String>*         identifier_array_t;
    TableConstraint*        table_constraint_t;

    BaseTableReference*     table_reference_t;
    TableAlias *            table_alias_t;
    JoinType                join_type_t;

    OrderByExpr*            order_by_expr_t;
    Vector<OrderByExpr*>*   order_by_expr_list_t;
    OrderType               order_by_type_t;

    WithExpr*               with_expr_t;
    Vector<WithExpr*>*      with_expr_list_t;

    SetOperatorType         set_operator_t;

    ExplainType             explain_type_t;

    ParsedExpr*             expr_t;
    Vector<ParsedExpr*>*    expr_array_t;

    Vector<WhenThen*>*     case_check_array_t;

    UpdateExpr*             update_expr_t;
    Vector<UpdateExpr*>*    update_expr_array_t;

    TableName* table_name_t;
    CopyOption* copy_option_t;
    Vector<CopyOption*>* copy_option_array;
}

%destructor {
    fprintf(stderr, "destroy table element array\n");
    if (($$) != nullptr) {
        for (auto ptr : *($$)) {
            delete ptr;
        }
        delete ($$);
    }
} <table_element_array_t>

%destructor {
    fprintf(stderr, "destroy statement array\n");
    if (($$) != nullptr) {
        for (auto ptr : *($$)) {
            delete ptr;
        }
        delete ($$);
    }
} <stmt_array>

%destructor {
    fprintf(stderr, "destroy expression array\n");
    if (($$) != nullptr) {
        for (auto ptr : *($$)) {
            delete ptr;
        }
        delete ($$);
    }
} <expr_array_t>

%destructor {
    fprintf(stderr, "destroy order by expr list\n");
    if (($$) != nullptr) {
        for (auto ptr : *($$)) {
            delete ptr;
        }
        delete ($$);
    }
} <order_by_expr_list_t>

%destructor {
    fprintf(stderr, "destroy update expr\n");
    if($$ != nullptr) {
        delete $$;
    }
} <update_expr_t>

%destructor {
    fprintf(stderr, "destroy update expr array\n");
    if (($$) != nullptr) {
        for (auto ptr : *($$)) {
            delete ptr;
        }
        delete ($$);
    }
} <update_expr_array_t>

%destructor {
    fprintf(stderr, "destroy with expr list\n");
    if (($$) != nullptr) {
        for (auto ptr : *($$)) {
            delete ptr;
        }
        delete ($$);
    }
} <with_expr_list_t>

%destructor {
    fprintf(stderr, "destroy table table_name\n");
    if (($$) != nullptr) {
        free($$->schema_name_ptr_);
        free($$->table_name_ptr_);
        delete ($$);
    }
} <table_name_t>

%destructor {
    fprintf(stderr, "destroy constraints\n");
    if (($$) != nullptr) {
        delete ($$);
    }
} <column_constraints_t>

%destructor {
    free($$);
} <str_value>

%destructor {
    fprintf(stderr, "destroy identifier array\n");
    delete ($$);
} <identifier_array_t>

%destructor {
    delete ($$);
} <expr_t>

%destructor {
    fprintf(stderr, "destroy table alias\n");
    delete ($$);
} <table_alias_t>

%destructor {
    fprintf(stderr, "destroy table reference\n");
    delete ($$);
} <table_reference_t>

%destructor {
    fprintf(stderr, "destroy order by expr\n");
    delete $$->expr_;
    delete $$;
} <order_by_expr_t>

%destructor {
    fprintf(stderr, "destroy with expr\n");
    delete $$->select_;
    delete $$;
} <with_expr_t>

%destructor {
    if($$ != nullptr) {
        delete $$;
    }
} <select_stmt>

%destructor {
    fprintf(stderr, "destroy case check array\n");
    if($$ != nullptr) {
        for(auto ptr: *($$)) {
            delete ptr;
        }
    }
} <case_check_array_t>

%token <str_value>      IDENTIFIER STRING
%token <double_value>   DOUBLE_VALUE
%token <long_value>     LONG_VALUE

/* SQL keywords */

%token CREATE SELECT INSERT DROP UPDATE DELETE COPY SET EXPLAIN SHOW ALTER EXECUTE PREPARE DESCRIBE UNION ALL INTERSECT
%token EXCEPT
%token SCHEMA TABLE COLLECTION TABLES INTO VALUES AST PIPELINE UNOPT OPT LOGICAL PHYSICAL VIEW INDEX ANALYZE
%token GROUP BY HAVING AS NATURAL JOIN LEFT RIGHT OUTER FULL ON INNER CROSS DISTINCT WHERE ORDER LIMIT OFFSET ASC DESC
%token IF NOT EXISTS IN FROM TO WITH DELIMITER FORMAT HEADER CAST END CASE ELSE THEN WHEN
%token BOOLEAN INTEGER TINYINT SMALLINT BIGINT HUGEINT CHAR VARCHAR FLOAT DOUBLE REAL DECIMAL DATE TIME DATETIME
%token TIMESTAMP UUID POINT LINE LSEG BOX PATH POLYGON CIRCLE BLOB BITMAP EMBEDDING VECTOR BIT
%token PRIMARY KEY UNIQUE NULLABLE IS
%token TRUE FALSE INTERVAL SECOND SECONDS MINUTE MINUTES HOUR HOURS DAY DAYS MONTH MONTHS YEAR YEARS
%token EQUAL NOT_EQ LESS_EQ GREATER_EQ BETWEEN AND OR EXTRACT LIKE

%token NUMBER

/* nonterminal symbol */

%type <base_stmt>         statement explainable_statement
%type <create_stmt>       create_statement
%type <drop_stmt>         drop_statement
%type <copy_stmt>         copy_statement
%type <show_stmt>         show_statement
%type <select_stmt>       select_clause_without_modifier select_without_paren select_with_paren select_statement
%type <select_stmt>       select_clause_without_modifier_paren select_clause_with_modifier
%type <delete_stmt>       delete_statement
%type <update_stmt>       update_statement
%type <insert_stmt>       insert_statement
%type <explain_stmt>      explain_statement

%type <stmt_array>        statement_list

%type <table_element_t>         table_element
%type <table_column_t>          table_column
%type <column_type_t>           column_type
%type <identifier_array_t>      identifier_array optional_identifier_array
%type <table_constraint_t>      table_constraint
%type <column_constraint_t>     column_constraint
%type <column_constraints_t>    column_constraints

%type <table_reference_t>       table_reference table_reference_unit table_reference_name from_clause join_clause
%type <table_alias_t>           table_alias
%type <join_type_t>             join_type
%type <order_by_type_t>         order_by_type
%type <order_by_expr_t>         order_by_expr
%type <order_by_expr_list_t>    order_by_expr_list order_by_clause
%type <with_expr_t>             with_expr
%type <with_expr_list_t>        with_expr_list with_clause
%type <set_operator_t>          set_operator
%type <explain_type_t>          explain_type

%type <expr_t>                  expr expr_alias constant_expr interval_expr column_expr function_expr subquery_expr
%type <expr_t>                  having_clause where_clause limit_expr offset_expr operand in_expr between_expr
%type <expr_t>                  conjunction_expr cast_expr case_expr
%type <expr_array_t>            expr_array group_by_clause
%type <update_expr_t>           update_expr;
%type <update_expr_array_t>     update_expr_array;
%type <case_check_array_t>      case_check_array;

%type <table_element_array_t>   table_element_array

%type <table_name_t>      table_name
%type <copy_option_array> copy_option_list
%type <copy_option_t>     copy_option

%type <str_value>         file_path

%type <bool_value>        if_not_exists if_exists distinct

/*
 * Operator precedence, low to high
 */

%left       OR
%left       AND
%left       BETWEEN CASE WHEN THEN ELSE
%right      NOT

%nonassoc   '=' EQUAL NOT_EQ LIKE
%nonassoc   '<' '>' LESS_EQ GREATER_EQ

%nonassoc   IS
%left       '+' '-'
%left       '*' '/' '%'

%left       '[' ']'
%left       '(' ')'
%left       '.'
%left       JOIN

%%

input_pattern : statement_list semicolon {
    result->statements_ptr_ = $1;
};

statement_list : statement {
    $1->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    $$ = new Vector<BaseStatement*>();
    $$->push_back($1);
}
| statement_list ';' statement {
    $3->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    $1->push_back($3);
    $$ = $1;
};

statement : create_statement { $$ = $1; }
| drop_statement { $$ = $1; }
| copy_statement { $$ = $1; }
| show_statement { $$ = $1; }
| select_statement { $$ = $1; }
| delete_statement { $$ = $1; }
| update_statement { $$ = $1; }
| insert_statement { $$ = $1; }
| explain_statement { $$ = $1; }

explainable_statement : create_statement { $$ = $1; }
| drop_statement { $$ = $1; }
| copy_statement { $$ = $1; }
| show_statement { $$ = $1; }
| select_statement { $$ = $1; }
| delete_statement { $$ = $1; }
| update_statement { $$ = $1; }
| insert_statement { $$ = $1; }

/*
 * CREATE STATEMENT
 */

/* CREATE SCHEMA schema_name; */
create_statement : CREATE SCHEMA if_not_exists IDENTIFIER {
    $$ = new CreateStatement();
    UniquePtr<CreateSchemaInfo> create_schema_info = MakeUnique<CreateSchemaInfo>();

    ParserHelper::ToLower($4);
    create_schema_info->schema_name_ = $4;
    free($4);

    $$->create_info_ = std::move(create_schema_info);
    $$->create_info_->conflict_type_ = $3 ? ConflictType::kIgnore : ConflictType::kError;
}

/* CREATE COLLECTION collection_name; */
| CREATE COLLECTION if_not_exists table_name {
    $$ = new CreateStatement();
    UniquePtr<CreateCollectionInfo> create_collection_info = std::make_unique<CreateCollectionInfo>();
    if($4->schema_name_ptr_ != nullptr) {
        create_collection_info->schema_name_ = $4->schema_name_ptr_;
        free($4->schema_name_ptr_);
    }
    create_collection_info->collection_name_ = $4->table_name_ptr_;
    free($4->table_name_ptr_);
    $$->create_info_ = std::move(create_collection_info);
    $$->create_info_->conflict_type_ = $3 ? ConflictType::kIgnore : ConflictType::kError;
    delete $4;
}

/* CREATE TABLE table_name ( column list ); */
| CREATE TABLE if_not_exists table_name '(' table_element_array ')' {
    $$ = new CreateStatement();
    UniquePtr<CreateTableInfo> create_table_info = MakeUnique<CreateTableInfo>();
    if($4->schema_name_ptr_ != nullptr) {
        create_table_info->schema_name_ = $4->schema_name_ptr_;
        free($4->schema_name_ptr_);
    }
    create_table_info->table_name_ = $4->table_name_ptr_;
    free($4->table_name_ptr_);
    delete $4;

    for (TableElement*& element : *$6) {
        if(element->type_ == TableElementType::kColumn) {
            create_table_info->column_defs_.emplace_back((ColumnDef*)element);
        } else {
            create_table_info->constraints_.emplace_back((TableConstraint*)element);
        }
    }
    delete $6;

    $$->create_info_ = std::move(create_table_info);
    $$->create_info_->conflict_type_ = $3 ? ConflictType::kIgnore : ConflictType::kError;
}
/* CREATE TABLE table_name AS SELECT .... ; */
| CREATE TABLE if_not_exists table_name AS select_statement {
    $$ = new CreateStatement();
    UniquePtr<CreateTableInfo> create_table_info = MakeUnique<CreateTableInfo>();
    if($4->schema_name_ptr_ != nullptr) {
        create_table_info->schema_name_ = $4->schema_name_ptr_;
        free($4->schema_name_ptr_);
    }
    create_table_info->table_name_ = $4->table_name_ptr_;
    free($4->table_name_ptr_);
    delete $4;

    create_table_info->conflict_type_ = $3 ? ConflictType::kIgnore : ConflictType::kError;
    create_table_info->select_ = $6;
    $$->create_info_ = std::move(create_table_info);
}
/* CREATE VIEW table_name AS SELECT .... ; */
| CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement {
    $$ = new CreateStatement();
    UniquePtr<CreateViewInfo> create_view_info = MakeUnique<CreateViewInfo>();
    if($4->schema_name_ptr_ != nullptr) {
        create_view_info->schema_name_ = $4->schema_name_ptr_;
        free($4->schema_name_ptr_);
    }
    create_view_info->view_name_ = $4->table_name_ptr_;
    free($4->table_name_ptr_);
    delete $4;

    create_view_info->view_columns_ = $5;
    create_view_info->select_ = $7;
    create_view_info->conflict_type_ = $3 ? ConflictType::kIgnore : ConflictType::kError;
    $$->create_info_ = std::move(create_view_info);
}
/* CREATE INDEX index_name ON table_name (column1) ; */
| CREATE INDEX if_not_exists IDENTIFIER ON table_name '(' identifier_array ')' {
    $$ = new CreateStatement();
    UniquePtr<CreateIndexInfo> create_index_info = MakeUnique<CreateIndexInfo>();
    if($6->schema_name_ptr_ != nullptr) {
        create_index_info->schema_name_ = $6->schema_name_ptr_;
        free($6->schema_name_ptr_);
    }
    create_index_info->table_name_ = $6->table_name_ptr_;
    free($6->table_name_ptr_);
    delete $6;

    ParserHelper::ToLower($4);
    create_index_info->index_name_ = $4;
    free($4);

    create_index_info->column_names_ = $8;
    create_index_info->conflict_type_ = $3 ? ConflictType::kIgnore : ConflictType::kError;
    $$->create_info_ = std::move(create_index_info);
};

table_element_array : table_element {
    $$ = new Vector<TableElement*>();
    $$->push_back($1);
}
| table_element_array ',' table_element {
    $1->push_back($3);
    $$ = $1;
};


table_element : table_column {
    $$ = $1;
}
| table_constraint {
    $$ = $1;
};


table_column :
IDENTIFIER column_type {
    SharedPtr<TypeInfo> type_info_ptr{nullptr};
    switch($2.logical_type_) {
        case LogicalType::kChar: {
            type_info_ptr = CharInfo::Make($2.width);
            break;
        }
        case LogicalType::kVarchar: {
            type_info_ptr = VarcharInfo::Make($2.width);
            break;
        }
        case LogicalType::kDecimal64: {
            type_info_ptr = Decimal64Info::Make($2.precision, $2.scale);
            break;
        }
        case LogicalType::kBlob: {
            type_info_ptr = BlobInfo::Make($2.width);
            break;
        }
        case LogicalType::kBitmap: {
            type_info_ptr = BitmapInfo::Make($2.width);
            break;
        }
        case LogicalType::kEmbedding: {
            type_info_ptr = EmbeddingInfo::Make($2.embedding_type_, $2.width);
            break;
        }
    }
    $$ = new ColumnDef($2.logical_type_, type_info_ptr);

    ParserHelper::ToLower($1);
    $$->name_ = $1;
    free($1);
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
};
| IDENTIFIER column_type column_constraints {
    SharedPtr<TypeInfo> type_info_ptr{nullptr};
    switch($2.logical_type_) {
        case LogicalType::kChar: {
            type_info_ptr = CharInfo::Make($2.width);
            break;
        }
        case LogicalType::kVarchar: {
            type_info_ptr = VarcharInfo::Make($2.width);
            break;
        }
        case LogicalType::kDecimal64: {
            type_info_ptr = Decimal64Info::Make($2.precision, $2.scale);
            break;
        }
        case LogicalType::kBlob: {
            type_info_ptr = BlobInfo::Make($2.width);
            break;
        }
        case LogicalType::kBitmap: {
            type_info_ptr = BitmapInfo::Make($2.width);
            break;
        }
        case LogicalType::kEmbedding: {
            type_info_ptr = EmbeddingInfo::Make($2.embedding_type_, $2.width);
            break;
        }
    }
    $$ = new ColumnDef($2.logical_type_, type_info_ptr);

    ParserHelper::ToLower($1);
    $$->name_ = $1;
    $$->constraints_ = *$3;
    delete $3;
    free($1);
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
};

column_type :
BOOLEAN { $$ = ColumnType{LogicalType::kBoolean}; }
| TINYINT { $$ = ColumnType{LogicalType::kTinyInt}; }
| SMALLINT { $$ = ColumnType{LogicalType::kSmallInt}; }
| INTEGER { $$ = ColumnType{LogicalType::kInteger}; }
| BIGINT { $$ = ColumnType{LogicalType::kBigInt}; }
| HUGEINT { $$ = ColumnType{LogicalType::kHugeInt}; }
| FLOAT { $$ = ColumnType{LogicalType::kFloat}; }
| REAL  { $$ = ColumnType{LogicalType::kFloat}; }
| DOUBLE { $$ = ColumnType{LogicalType::kDouble}; }
| DATE { $$ = ColumnType{LogicalType::kDate}; }
| TIME { $$ = ColumnType{LogicalType::kTime}; }
| DATETIME { $$ = ColumnType{LogicalType::kDateTime}; }
| TIMESTAMP { $$ = ColumnType{LogicalType::kTimestamp}; }
| UUID { $$ = ColumnType{LogicalType::kUuid}; }
| POINT { $$ = ColumnType{LogicalType::kPoint}; }
| LINE { $$ = ColumnType{LogicalType::kLine}; }
| LSEG { $$ = ColumnType{LogicalType::kLineSeg}; }
| BOX { $$ = ColumnType{LogicalType::kBox}; }
| PATH { $$ = ColumnType{LogicalType::kPath}; }
| POLYGON { $$ = ColumnType{LogicalType::kPolygon}; }
| CIRCLE { $$ = ColumnType{LogicalType::kCircle}; }
// Variable types
| CHAR '(' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kChar, $3}; }
| VARCHAR '(' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kVarchar, $3}; }
| DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kDecimal64, 0, $3, $5}; }
| DECIMAL '(' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kDecimal64, 0, $3, 0}; }
| DECIMAL { $$ = ColumnType{LogicalType::kDecimal64, 0, 0, 0}; }
| BLOB '(' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kBlob, $3}; }
| BITMAP '(' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kBitmap, $3}; }
| EMBEDDING '(' BIT ',' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kEmbedding, $5, 0, 0, kElemBit}; }
| EMBEDDING '(' TINYINT ',' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kEmbedding, $5, 0, 0, kElemInt8}; }
| EMBEDDING '(' SMALLINT ',' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kEmbedding, $5, 0, 0, kElemInt16}; }
| EMBEDDING '(' INTEGER ',' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kEmbedding, $5, 0, 0, kElemInt32}; }
| EMBEDDING '(' BIGINT ',' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kEmbedding, $5, 0, 0, kElemInt64}; }
| EMBEDDING '(' FLOAT ',' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kEmbedding, $5, 0, 0, kElemFloat}; }
| EMBEDDING '(' DOUBLE ',' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kEmbedding, $5, 0, 0, kElemDouble}; }
| VECTOR '(' BIT ',' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kEmbedding, $5, 0, 0, kElemBit}; }
| VECTOR '(' TINYINT ',' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kEmbedding, $5, 0, 0, kElemInt8}; }
| VECTOR '(' SMALLINT ',' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kEmbedding, $5, 0, 0, kElemInt16}; }
| VECTOR '(' INTEGER ',' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kEmbedding, $5, 0, 0, kElemInt32}; }
| VECTOR '(' BIGINT ',' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kEmbedding, $5, 0, 0, kElemInt64}; }
| VECTOR '(' FLOAT ',' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kEmbedding, $5, 0, 0, kElemFloat}; }
| VECTOR '(' DOUBLE ',' LONG_VALUE ')' { $$ = ColumnType{LogicalType::kEmbedding, $5, 0, 0, kElemDouble}; }
/*
| DECIMAL opt_decimal_specification {
  $$ = ColumnType{DataType::DECIMAL, 0, $2->first, $2->second};
  delete $2;
}
| TEXT { $$ = ColumnType{DataType::TEXT}; }
| TIME opt_time_precision { $$ = ColumnType{DataType::TIME, 0, $2}; }
| TIMESTAMP { $$ = ColumnType{DataType::DATETIME}; }
| VARCHAR '(' INTVAL ')' { $$ = ColumnType{DataType::VARCHAR, $3}; }

opt_time_precision : '(' INTVAL ')' { $$ = $2; }
| { $$ = 0; };

opt_decimal_specification : '(' INTVAL ',' INTVAL ')' { $$ = new std::pair<int64_t, int64_t>{$2, $4}; }
| '(' INTVAL ')' { $$ = new std::pair<int64_t, int64_t>{$2, 0}; }
|  { $$ = new std::pair<int64_t, int64_t>{0, 0}; };
*/

column_constraints : column_constraint {
    $$ = new HashSet<ConstraintType>();
    $$->insert($1);
}
| column_constraints column_constraint {
    if($1->contains($2)) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete $1;
        YYERROR;
    }
    $1->insert($2);
    $$ = $1;
}

column_constraint : PRIMARY KEY {
    $$ = ConstraintType::kPrimaryKey;
}
| UNIQUE {
    $$ = ConstraintType::kUnique;
}
| NULLABLE {
    $$ = ConstraintType::kNull;
}
| NOT NULLABLE {
    $$ = ConstraintType::kNotNull;
};

table_constraint : PRIMARY KEY '(' identifier_array ')' {
    $$ = new TableConstraint();
    $$->names_ptr_ = $4;
    $$->constraint_ = ConstraintType::kPrimaryKey;
}
| UNIQUE '(' identifier_array ')' {
    $$ = new TableConstraint();
    $$->names_ptr_ = $3;
    $$->constraint_ = ConstraintType::kUnique;
};


identifier_array : IDENTIFIER {
    $$ = new Vector<String>();
    ParserHelper::ToLower($1);
    $$->emplace_back($1);
    free($1);
}
| identifier_array ',' IDENTIFIER {
    ParserHelper::ToLower($3);
    $1->emplace_back($3);
    free($3);
    $$ = $1;
};

/*
 * DELETE STATEMENT
 */
delete_statement : DELETE FROM table_name where_clause {
    $$ = new DeleteStatement();

    if($3->schema_name_ptr_ != nullptr) {
        $$->schema_name_ = $3->schema_name_ptr_;
        free($3->schema_name_ptr_);
    }
    $$->table_name_ = $3->table_name_ptr_;
    free($3->table_name_ptr_);
    delete $3;
    $$->where_expr_ = $4;
};

/*
 * INSERT STATEMENT
 */
insert_statement: INSERT INTO table_name optional_identifier_array VALUES '(' expr_array ')' {
    $$ = new InsertStatement();
    if($3->schema_name_ptr_ != nullptr) {
        $$->schema_name_ = $3->schema_name_ptr_;
        free($3->schema_name_ptr_);
    }
    $$->table_name_ = $3->table_name_ptr_;
    free($3->table_name_ptr_);
    delete $3;
    $$->columns_ = $4;
    $$->values_ = $7;
}
| INSERT INTO table_name optional_identifier_array select_without_paren {
    $$ = new InsertStatement();
    if($3->schema_name_ptr_ != nullptr) {
        $$->schema_name_ = $3->schema_name_ptr_;
        free($3->schema_name_ptr_);
    }
    $$->table_name_ = $3->table_name_ptr_;
    free($3->table_name_ptr_);
    delete $3;
    $$->columns_ = $4;
    $$->select_ = $5;
}

optional_identifier_array: '(' identifier_array ')' {
    $$ = $2;
}
| {
    $$ = nullptr;
}

/*
 * EXPLAIN STATEMENT
 */
explain_statement : EXPLAIN explain_type explainable_statement {
    $$ = new ExplainStatement();
    $$->type_ = $2;
    $$->statement_ = $3;
}

explain_type: ANALYZE {
    $$ = ExplainType::kAnalyze;
}
| AST {
    $$ = ExplainType::kAst;
}
| UNOPT LOGICAL {
    $$ = ExplainType::kUnOpt;
}
| OPT LOGICAL {
    $$ = ExplainType::kOpt;
}
| PHYSICAL {
    $$ = ExplainType::kPhysical;
}
| PIPELINE {
    $$ = ExplainType::kPipeline;
}
| {
    $$ = ExplainType::kPhysical;
}

/*
 * UPDATE STATEMENT
 */
update_statement: UPDATE table_name SET update_expr_array where_clause {
    $$ = new UpdateStatement();
    if($2->schema_name_ptr_ != nullptr) {
        $$->schema_name_ = $2->schema_name_ptr_;
        free($2->schema_name_ptr_);
    }
    $$->table_name_ = $2->table_name_ptr_;
    free($2->table_name_ptr_);
    delete $2;
    $$->where_expr_ = $5;
    $$->update_expr_array_ = $4;
}

update_expr_array: update_expr {
    $$ = new Vector<UpdateExpr*>();
    $$->emplace_back($1);
}
| update_expr_array ',' update_expr {
    $1->emplace_back($3);
    $$ = $1;
}

update_expr : IDENTIFIER '=' expr {
    $$ = new UpdateExpr();
    ParserHelper::ToLower($1);
    $$->column_name = $1;
    free($1);
    $$->value = $3;
};

/*
 * DROP STATEMENT
 */

/* DROP SCHEMA schema_name; */
drop_statement: DROP SCHEMA if_exists IDENTIFIER {
    $$ = new DropStatement();
    UniquePtr<DropSchemaInfo> drop_schema_info = MakeUnique<DropSchemaInfo>();

    ParserHelper::ToLower($4);
    drop_schema_info->schema_name_ = $4;
    free($4);

    $$->drop_info_ = std::move(drop_schema_info);
    $$->drop_info_->conflict_type_ = $3 ? ConflictType::kIgnore : ConflictType::kError;
};

/* DROP COLLECTION collection_name; */
| DROP COLLECTION if_exists table_name {
    $$ = new DropStatement();
    std::unique_ptr<DropCollectionInfo> drop_collection_info = std::make_unique<DropCollectionInfo>();
    if($4->schema_name_ptr_ != nullptr) {
        drop_collection_info->schema_name_ = $4->schema_name_ptr_;
        free($4->schema_name_ptr_);
    }
    drop_collection_info->collection_name_ = $4->table_name_ptr_;
    free($4->table_name_ptr_);
    $$->drop_info_ = std::move(drop_collection_info);
    $$->drop_info_->conflict_type_ = $3 ? ConflictType::kIgnore : ConflictType::kError;
    delete $4;
}

/* DROP TABLE table_name; */
| DROP TABLE if_exists table_name {
    $$ = new DropStatement();
    std::unique_ptr<DropTableInfo> drop_table_info = std::make_unique<DropTableInfo>();
    if($4->schema_name_ptr_ != nullptr) {
        drop_table_info->schema_name_ = $4->schema_name_ptr_;
        free($4->schema_name_ptr_);
    }
    drop_table_info->table_name_ = $4->table_name_ptr_;
    free($4->table_name_ptr_);
    $$->drop_info_ = std::move(drop_table_info);
    $$->drop_info_->conflict_type_ = $3 ? ConflictType::kIgnore : ConflictType::kError;
    delete $4;
}

/* DROP VIEW view_name; */
| DROP VIEW if_exists table_name {
    $$ = new DropStatement();
    std::unique_ptr<DropViewInfo> drop_view_info = std::make_unique<DropViewInfo>();
    if($4->schema_name_ptr_ != nullptr) {
        drop_view_info->schema_name_ = $4->schema_name_ptr_;
        free($4->schema_name_ptr_);
    }
    drop_view_info->view_name_ = $4->table_name_ptr_;
    free($4->table_name_ptr_);
    $$->drop_info_ = std::move(drop_view_info);
    $$->drop_info_->conflict_type_ = $3 ? ConflictType::kIgnore : ConflictType::kError;
    delete $4;
}

/* DROP INDEX index_name; */
| DROP INDEX if_exists table_name {
    $$ = new DropStatement();
    UniquePtr<DropIndexInfo> drop_index_info = MakeUnique<DropIndexInfo>();
    if($4->schema_name_ptr_ != nullptr) {
        drop_index_info->schema_name_ = $4->schema_name_ptr_;
        free($4->schema_name_ptr_);
    }
    drop_index_info->index_name_ = $4->table_name_ptr_;
    free($4->table_name_ptr_);
    delete $4;

    $$->drop_info_ = std::move(drop_index_info);
    $$->drop_info_->conflict_type_ = $3 ? ConflictType::kIgnore : ConflictType::kError;
};

/*
 * COPY STATEMENT
 */
// COPY schema.table TO file_path WITH (FORMAT csv, DELIMITER ',', HEADER TRUE)
copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')' {
    $$ = new CopyStatement();

    // Copy To
    $$->copy_from_ = false;

    // table_name
    if($2->schema_name_ptr_ != nullptr) {
        $$->schema_name_ = $2->schema_name_ptr_;
        free($2->schema_name_ptr_);
    }
    $$->table_name_ = $2->table_name_ptr_;
    free($2->table_name_ptr_);
    delete $2;

    // file path
    $$->file_path_ = $4;
    free($4);

    // copy options
    SizeT option_count = (*$7).size();
    for(SizeT idx = 0; idx < option_count; ++ idx) {
        CopyOption* option_ptr = (*$7)[idx];
        switch(option_ptr->option_type_) {
            case CopyOptionType::kFormat: {
                $$->copy_file_type_ = option_ptr->file_type_;
                break;
            }
            case CopyOptionType::kDelimiter: {
                $$->delimiter_ = option_ptr->delimiter_;
                break;
            }
            case CopyOptionType::kHeader: {
                $$->header_ = option_ptr->header_;
                break;
            }
        }
        delete option_ptr;
    }
    delete $7;
}
| COPY table_name FROM file_path WITH '(' copy_option_list ')' {
    $$ = new CopyStatement();

    // Copy From
    $$->copy_from_ = true;

    // table_name
    if($2->schema_name_ptr_ != nullptr) {
        $$->schema_name_ = $2->schema_name_ptr_;
        free($2->schema_name_ptr_);
    }
    $$->table_name_ = $2->table_name_ptr_;
    free($2->table_name_ptr_);
    delete $2;

    // file path
    $$->file_path_ = $4;
    free($4);

    // copy options
    SizeT option_count = (*$7).size();
    for(SizeT idx = 0; idx < option_count; ++ idx) {
        CopyOption* option_ptr = (*$7)[idx];
        switch(option_ptr->option_type_) {
            case CopyOptionType::kFormat: {
                $$->copy_file_type_ = option_ptr->file_type_;
                break;
            }
            case CopyOptionType::kDelimiter: {
                $$->delimiter_ = option_ptr->delimiter_;
                break;
            }
            case CopyOptionType::kHeader: {
                $$->header_ = option_ptr->header_;
                break;
            }
        }
        delete option_ptr;
    }
    delete $7;
};

/*
 * SELECT STATEMENT
 */
select_statement : select_without_paren {
    $$ = $1;
}
| select_with_paren {
    $$ = $1;
}
| select_statement set_operator select_clause_without_modifier_paren {
    SelectStatement* node = $1;
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = $2;
    node->nested_select_ = $3;
    $$ = $1;
}
| select_statement set_operator select_clause_without_modifier {
    SelectStatement* node = $1;
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = $2;
    node->nested_select_ = $3;
    $$ = $1;
}

select_with_paren : '(' select_without_paren ')' {
    $$ = $2;
}
| '(' select_with_paren ')' {
    $$ = $2;
};

select_without_paren: with_clause select_clause_with_modifier {
    $2->with_exprs_ = $1;
    $$ = $2;
};

select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr {
    $1->order_by_list = $2;
    $1->limit_expr_ = $3;
    $1->offset_expr_ = $4;
    $$ = $1;
}

select_clause_without_modifier_paren: '(' select_clause_without_modifier ')' {
  $$ = $2;
}
| '(' select_clause_without_modifier_paren ')' {
    $$ = $2;
};

select_clause_without_modifier:
SELECT distinct expr_array from_clause where_clause group_by_clause having_clause {
    $$ = new SelectStatement();
    $$->select_list_ = $3;
    $$->select_distinct_ = $2;
    $$->table_ref_ = $4;
    $$->where_expr_ = $5;
    $$->group_by_list_ = $6;
    $$->having_expr_ = $7;

    if($$->group_by_list_ == nullptr && $$->having_expr_ != nullptr) {
        yyerror(&yyloc, scanner, result, "HAVING clause should follow after GROUP BY clause");
        YYERROR;
    }
};

order_by_clause : ORDER BY order_by_expr_list {
    $$ = $3;
}
| /* empty order by */ {
    $$ = nullptr;
};

order_by_expr_list: order_by_expr {
    $$ = new Vector<OrderByExpr*>();
    $$->emplace_back($1);
}
| order_by_expr_list ',' order_by_expr {
    $1->emplace_back($3);
    $$ = $1;
}

order_by_expr : expr order_by_type {
    $$ = new OrderByExpr();
    $$->expr_ = $1;
    $$->type_ = $2;
};

order_by_type: ASC {
    $$ = kAsc;
}
| DESC {
    $$ = kDesc;
}
| {
    $$ = kAsc;
}

limit_expr: LIMIT expr {
    $$ = $2;
}
| /* empty limit expression */
{   $$ = nullptr; };

offset_expr: OFFSET expr {
    $$ = $2;
}
| /* empty offset expression */
{   $$ = nullptr; };

distinct : DISTINCT {
    $$ = true;
}
| {
    $$ = false;
}

from_clause: FROM table_reference {
    $$ = $2;
}
| /* no from clause */ {
    $$ = nullptr;
}

where_clause: WHERE expr {
    $$ = $2;
}
| /* no where clause */ {
    $$ = nullptr;
}

having_clause: HAVING expr {
    $$ = $2;
}
| /* no where clause */ {
    $$ = nullptr;
}

group_by_clause: GROUP BY expr_array {
    $$ = $3;
}
| {
    $$ = nullptr;
}

set_operator : UNION {
    $$ = SetOperatorType::kUnion;
}
| UNION ALL {
    $$ = SetOperatorType::kUnionAll;
}
| INTERSECT {
    $$ = SetOperatorType::kIntersect;
}
| EXCEPT {
    $$ = SetOperatorType::kExcept;
};

/*
 * TABLE REFERENCE
 */

table_reference : table_reference_unit {
    $$ = $1;
}
| table_reference ',' table_reference_unit {
    CrossProductReference* cross_product_ref = nullptr;
    if($1->type_ == TableRefType::kCrossProduct) {
        cross_product_ref = (CrossProductReference*)$1;
        cross_product_ref->tables_.emplace_back($3);
    } else {
        cross_product_ref = new CrossProductReference();
        cross_product_ref->tables_.emplace_back($1);
        cross_product_ref->tables_.emplace_back($3);
    }

    $$ = cross_product_ref;
};


table_reference_unit : table_reference_name | join_clause;

table_reference_name : table_name table_alias {
    TableReference* table_ref = new TableReference();
    if($1->schema_name_ptr_ != nullptr) {
        table_ref->schema_name_ = $1->schema_name_ptr_;
        free($1->schema_name_ptr_);
    }
    table_ref->table_name_ = $1->table_name_ptr_;
    free($1->table_name_ptr_);
    delete $1;

    table_ref->alias_ = $2;
    $$ = table_ref;
}
/* FROM (select * from t1) AS t2 */
| '(' select_statement ')' table_alias {
    SubqueryReference* subquery_reference = new SubqueryReference();
    subquery_reference->select_statement_ = $2;
    subquery_reference->alias_ = $4;
    $$ = subquery_reference;
}


/* 'table_name' or 'schema_name.table_name' */
table_name : IDENTIFIER {
    if(!result->IsError()) {
        $$ = new TableName();
        ParserHelper::ToLower($1);
        $$->table_name_ptr_ = $1;
    }
}
| IDENTIFIER '.' IDENTIFIER {
    if(!result->IsError()) {
        $$ = new TableName();
        ParserHelper::ToLower($1);
        ParserHelper::ToLower($3);
        $$->schema_name_ptr_ = $1;
        $$->table_name_ptr_ = $3;
    }
};

/* AS 'table_alias' or AS 'table_alias(col1_alias, col2_alias ... )' */
table_alias : AS IDENTIFIER {
    $$ = new TableAlias();
    ParserHelper::ToLower($2);
    $$->alias_ = $2;
}
| IDENTIFIER {
    $$ = new TableAlias();
    ParserHelper::ToLower($1);
    $$->alias_ = $1;
}
| AS IDENTIFIER '(' identifier_array ')' {
    $$ = new TableAlias();
    ParserHelper::ToLower($2);
    $$->alias_ = $2;
    $$->column_alias_array_ = $4;
}
| {
    $$ = nullptr;
}

/*
 * WITH CLAUSE
 */
with_clause : WITH with_expr_list {
    $$ = $2;
}
| /* empty with clause */ {
    $$ = nullptr;
}

with_expr_list: with_expr {
    $$ = new Vector<WithExpr*>();
    $$->emplace_back($1);
} | with_expr_list ',' with_expr {
    $1->emplace_back($3);
    $$ = $1;
}

with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')' {
    $$ = new WithExpr();
    ParserHelper::ToLower($1);
    $$->alias_ = $1;
    free($1);
    $$->select_ = $4;
}

/*
 * JOIN CLAUSE
 */

join_clause: table_reference_unit NATURAL JOIN table_reference_name {
    JoinReference* join_reference = new JoinReference();
    join_reference->left_ = $1;
    join_reference->right_ = $4;
    join_reference->join_type_ = JoinType::kNatural;
    $$ = join_reference;
}
| table_reference_unit join_type JOIN table_reference_name ON expr {
    JoinReference* join_reference = new JoinReference();
    join_reference->left_ = $1;
    join_reference->right_ = $4;
    join_reference->join_type_ = $2;
    join_reference->condition_ = $6;
    $$ = join_reference;
};
/* Using column name to JOIN
| table_reference_unit join_type JOIN table_reference_name USING '(' column_name ')' {
}
*/


join_type : INNER {
    $$ = JoinType::kInner;
}
| LEFT {
    $$ = JoinType::kLeft;
}
| RIGHT {
    $$ = JoinType::kRight;
}
| OUTER {
    $$ = JoinType::kFull;
}
| FULL {
    $$ = JoinType::kFull;
}
| CROSS {
    $$ = JoinType::kCross;
}
| /* default */ {
};

/*
 * SHOW STATEMENT
 */
show_statement: SHOW TABLES {
    $$ = new ShowStatement();
    $$->show_type_ = ShowStmtType::kTables;
}
| DESCRIBE table_name {
    $$ = new ShowStatement();
    $$->show_type_ = ShowStmtType::kColumns;
    if($2->schema_name_ptr_ != nullptr) {
        $$->schema_name_ = $2->schema_name_ptr_;
        free($2->schema_name_ptr_);
    }
    $$->table_name_ = $2->table_name_ptr_;
    free($2->table_name_ptr_);
    delete $2;
};

/*
 * EXPRESSION
 */

expr_array : expr_alias {
    $$ = new Vector<ParsedExpr*>();
    $$->emplace_back($1);
}
| expr_array ',' expr_alias {
    $1->emplace_back($3);
    $$ = $1;
};

/*
constant_expr_array: constant_expr {
    $$ = new Vector<ParsedExpr*>();
    $$->emplace_back($1);
}
| constant_expr_array ',' constant_expr {
    $1->emplace_back($3);
    $$ = $1;
}
*/

expr_alias : expr AS IDENTIFIER {
    $$ = $1;
    ParserHelper::ToLower($3);
    $$->alias_ = $3;
    free($3);
}
| expr {
    $$ = $1;
};

expr : operand
| subquery_expr
| in_expr
| between_expr
| conjunction_expr;

operand: '(' expr ')' {
   $$ = $2;
}
| '(' select_without_paren ')' {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kScalar;
    subquery_expr->select_ = $2;
    $$ = subquery_expr;
}
| constant_expr
| column_expr
| function_expr
| case_expr
| cast_expr

function_expr : IDENTIFIER '(' ')' {
    FunctionExpr* func_expr = new FunctionExpr();
    ParserHelper::ToLower($1);
    func_expr->func_name_ = $1;
    free($1);
    func_expr->arguments_ = nullptr;
    $$ = func_expr;
}
| IDENTIFIER '(' expr_array ')' {
    FunctionExpr* func_expr = new FunctionExpr();
    ParserHelper::ToLower($1);
    func_expr->func_name_ = $1;
    free($1);
    func_expr->arguments_ = $3;
    $$ = func_expr;
}
| IDENTIFIER '(' DISTINCT expr_array ')' {
    FunctionExpr* func_expr = new FunctionExpr();
    ParserHelper::ToLower($1);
    func_expr->func_name_ = $1;
    free($1);
    func_expr->arguments_ = $4;
    func_expr->distinct_ = true;
    $$ = func_expr;
}
| operand IS NOT NULLABLE {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    $$ = func_expr;
}
| operand IS NULLABLE {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    $$ = func_expr;
}
| NOT operand {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($2);
    $$ = func_expr;
}
| '-' operand {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($2);
    $$ = func_expr;
}
| '+' operand {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($2);
    $$ = func_expr;
}
| operand '-' operand {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand '+' operand {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand '*' operand {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand '/' operand {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand '%' operand {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand '=' operand {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand EQUAL operand {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand NOT_EQ operand {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand '<' operand {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand '>' operand {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand LESS_EQ operand {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand GREATER_EQ operand {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| EXTRACT '(' STRING FROM operand ')' {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "extract";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    ParserHelper::ToLower($3);
    TimeUnit time_unit = TimeUnit::kInvalidUnit;
    if(strcmp($3, "year") == 0) {
        time_unit = TimeUnit::kYear;
    } else if(strcmp($3, "month") == 0) {
        time_unit = TimeUnit::kMonth;
    } else if(strcmp($3, "day") == 0) {
        time_unit = TimeUnit::kDay;
    } else if(strcmp($3, "hour") == 0) {
        time_unit = TimeUnit::kHour;
    } else if(strcmp($3, "minute") == 0) {
        time_unit = TimeUnit::kMinute;
    } else if(strcmp($3, "second") == 0) {
        time_unit = TimeUnit::kSecond;
    } else {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }

    free($3);
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kTimeUnit);
    const_expr->time_unit_ = time_unit;

    func_expr->arguments_->emplace_back(const_expr);
    func_expr->arguments_->emplace_back($5);
    $$ = func_expr;
}
| operand LIKE operand {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand NOT LIKE operand {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($4);
    $$ = func_expr;
};

conjunction_expr: expr AND expr {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| expr OR expr {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
};

between_expr: operand BETWEEN operand AND operand {
    BetweenExpr* between_expr = new BetweenExpr();
    between_expr->value_ = $1;
    between_expr->lower_bound_ = $3;
    between_expr->upper_bound_ = $5;
    $$ = between_expr;
}

in_expr: operand IN '(' expr_array ')' {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "in";
    func_expr->arguments_ = $4;
    func_expr->arguments_->emplace_back($1);
    $$ = func_expr;
}
| operand NOT IN '(' expr_array ')' {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "not_in";
    func_expr->arguments_ = $5;
    func_expr->arguments_->emplace_back($1);
    $$ = func_expr;
};

case_expr: CASE expr case_check_array END {
    CaseExpr* case_expr = new CaseExpr();
    case_expr->expr_ = $2;
    case_expr->case_check_array_ = $3;
    $$ = case_expr;
}
| CASE expr case_check_array ELSE expr END {
    CaseExpr* case_expr = new CaseExpr();
    case_expr->expr_ = $2;
    case_expr->case_check_array_ = $3;
    case_expr->else_expr_ = $5;
    $$ = case_expr;
}
| CASE case_check_array END {
    CaseExpr* case_expr = new CaseExpr();
    case_expr->case_check_array_ = $2;
    $$ = case_expr;
}
| CASE case_check_array ELSE expr END {
    CaseExpr* case_expr = new CaseExpr();
    case_expr->case_check_array_ = $2;
    case_expr->else_expr_ = $4;
    $$ = case_expr;
};

case_check_array: WHEN expr THEN expr {
    $$ = new Vector<WhenThen*>();
    WhenThen* when_then_ptr = new WhenThen();
    when_then_ptr->when_ = $2;
    when_then_ptr->then_ = $4;
    $$->emplace_back(when_then_ptr);
}
| case_check_array WHEN expr THEN expr {
    WhenThen* when_then_ptr = new WhenThen();
    when_then_ptr->when_ = $3;
    when_then_ptr->then_ = $5;
    $1->emplace_back(when_then_ptr);
    $$ = $1;
};

cast_expr: CAST '(' expr AS column_type ')' {
    SharedPtr<TypeInfo> type_info_ptr{nullptr};
    switch($5.logical_type_) {
        case LogicalType::kChar: {
            type_info_ptr = CharInfo::Make($5.width);
            break;
        }
        case LogicalType::kVarchar: {
            type_info_ptr = VarcharInfo::Make($5.width);
            break;
        }
        case LogicalType::kDecimal64: {
            type_info_ptr = Decimal64Info::Make($5.precision, $5.scale);
            break;
        }
        case LogicalType::kBlob: {
            type_info_ptr = BlobInfo::Make($5.width);
            break;
        }
        case LogicalType::kBitmap: {
            type_info_ptr = BitmapInfo::Make($5.width);
            break;
        }
        case LogicalType::kEmbedding: {
            type_info_ptr = EmbeddingInfo::Make($5.embedding_type_, $5.width);
            break;
        }
    }
    CastExpr* cast_expr = new CastExpr($5.logical_type_, type_info_ptr);
    cast_expr->expr_ = $3;
    $$ = cast_expr;
};

subquery_expr: EXISTS '(' select_without_paren ')' {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kExists;
    subquery_expr->select_ = $3;
    $$ = subquery_expr;
}
| NOT EXISTS '(' select_without_paren ')' {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kNotExists;
    subquery_expr->select_ = $4;
    $$ = subquery_expr;
}
| operand IN '(' select_without_paren ')' {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kIn;
    subquery_expr->left_ = $1;
    subquery_expr->select_ = $4;
    $$ = subquery_expr;
}
| operand NOT IN '(' select_without_paren ')' {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kNotIn;
    subquery_expr->left_ = $1;
    subquery_expr->select_ = $5;
    $$ = subquery_expr;
};

column_expr : IDENTIFIER {
    ColumnExpr* column_expr = new ColumnExpr();
    ParserHelper::ToLower($1);
    column_expr->names_.emplace_back($1);
    free($1);
    $$ = column_expr;
}
| column_expr '.' IDENTIFIER {
    ColumnExpr* column_expr = (ColumnExpr*)$1;
    ParserHelper::ToLower($3);
    column_expr->names_.emplace_back($3);
    free($3);
    $$ = column_expr;
}
| '*' {
    ColumnExpr* column_expr = new ColumnExpr();
    column_expr->star_ = true;
    $$ = column_expr;
}
| column_expr '.' '*' {
    ColumnExpr* column_expr = (ColumnExpr*)$1;
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    $$ = column_expr;
};

constant_expr: STRING {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kString);
    const_expr->str_value_ = $1;
    $$ = const_expr;
}
| TRUE {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    $$ = const_expr;
}
| FALSE {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    $$ = const_expr;
}
| DOUBLE_VALUE {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kDouble);
    const_expr->double_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInteger);
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| DATE STRING {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kDate);
    const_expr->date_value_ = $2;
    $$ = const_expr;
}
| INTERVAL interval_expr {
    $$ = $2;
}
| interval_expr {
    $$ = $1;
};

interval_expr: LONG_VALUE SECONDS {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = TimeUnit::kSecond;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE SECOND {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = TimeUnit::kSecond;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE MINUTES {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = TimeUnit::kMinute;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE MINUTE {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = TimeUnit::kMinute;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE HOURS {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = TimeUnit::kHour;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE HOUR {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = TimeUnit::kHour;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE DAYS {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = TimeUnit::kDay;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE DAY {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = TimeUnit::kDay;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE MONTHS {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = TimeUnit::kMonth;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE MONTH {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = TimeUnit::kMonth;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE YEARS {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = TimeUnit::kYear;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE YEAR {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = TimeUnit::kYear;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
};

/*
 * Misc.
 */

copy_option_list : copy_option {
    $$ = new Vector<CopyOption*>();
    $$->push_back($1);
}
| copy_option_list ',' copy_option {
    $1->push_back($3);
    $$ = $1;
};

copy_option : FORMAT IDENTIFIER {
    $$ = new CopyOption();
    $$->option_type_ = CopyOptionType::kFormat;
    if (strcasecmp($2, "csv") == 0) {
        $$->file_type_ = CopyFileType::kCSV;
        free($2);
    } else {
        free($2);
        delete $$;
        yyerror(&yyloc, scanner, result, "Unknown file type");
        YYERROR;
    }
}
| DELIMITER STRING {
    $$ = new CopyOption();
    $$->option_type_ = CopyOptionType::kDelimiter;
    $$->delimiter_ = $2[0];
    free($2);
}
| HEADER {
    $$ = new CopyOption();
    $$->option_type_ = CopyOptionType::kHeader;
    $$->header_ = true;
};

file_path : STRING {
    $$ = $1;
};

if_exists: IF EXISTS { $$ = true; }
| { $$ = false; };

if_not_exists : IF NOT EXISTS { $$ = true; }
| { $$ = false; };

semicolon : ';'
| /* nothing */
;

%%

void
yyerror(YYLTYPE * llocp, void* lexer, ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = String(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

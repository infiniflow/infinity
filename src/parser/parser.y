
%{
#include <stdint.h>
#include <stdio.h>
#include "parser.h"
#include "lexer.h"

void yyerror(YYLTYPE * llocp, void* lexer, ParserResult* result, const char* msg);
%}

%code requires {

#include "statement.h"
#include "parser_result.h"
#include "defer_operation.h"
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
    SetStatement*     set_stmt;

    Vector<BaseStatement*>* stmt_array;

    Vector<TableElement*>*  table_element_array_t;
    TableElement*           table_element_t;
    ColumnDef*              table_column_t;
    ColumnType              column_type_t;
    ConstraintType          column_constraint_t;
    HashSet<ConstraintType>* column_constraints_t;
    Vector<String>*         identifier_array_t;
    TableConstraint*        table_constraint_t;

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
    fprintf(stderr, "destroy table name\n");
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

%token <str_value>      IDENTIFIER STRING
%token <double_value>   DOUBLE_VALUE
%token <long_value>     LONG_VALUE
%token <bool_value>     BOOL_VALUE

/* SQL keywords */

%token CREATE SELECT INSERT DROP UPDATE DELETE COPY SET EXPLAIN SHOW ALTER EXECUTE PREPARE DESCRIBE
%token SCHEMA TABLE COLLECTION TABLES
%token IF NOT EXISTS FROM TO WITH DELIMITER FORMAT HEADER
%token BOOLEAN INTEGER TINYINT SMALLINT BIGINT HUGEINT CHAR VARCHAR FLOAT DOUBLE REAL DECIMAL DATE TIME DATETIME
%token TIMESTAMP UUID POINT LINE LSEG BOX PATH POLYGON CIRCLE BLOB BITMAP EMBEDDING VECTOR BIT
%token PRIMARY KEY UNIQUE NULLABLE

%token NUMBER

/* nonterminal symbol */


%type <base_stmt>         statement
%type <create_stmt>       create_statement
%type <drop_stmt>         drop_statement
%type <copy_stmt>         copy_statement
%type <show_stmt>         show_statement

%type <stmt_array>        statement_list

%type <table_element_t>         table_element
%type <table_column_t>          table_column
%type <column_type_t>           column_type
%type <identifier_array_t>      identifier_array
%type <table_constraint_t>      table_constraint
%type <column_constraint_t>     column_constraint
%type <column_constraints_t>    column_constraints


%type <table_element_array_t>   table_element_array

%type <table_name_t>      table_name
%type <copy_option_array> copy_option_list
%type <copy_option_t>     copy_option

%type <str_value>         file_path


%type <bool_value>        if_not_exists
%type <bool_value>        if_exists

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
| show_statement { $$ = $1; };

/*
 * CREATE STATEMENT
 */

/* CREATE SCHEMA schema_name; */
create_statement : CREATE SCHEMA if_not_exists IDENTIFIER {
    $$ = new CreateStatement();
    UniquePtr<CreateSchemaInfo> create_schema_info = MakeUnique<CreateSchemaInfo>();
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
    create_collection_info->table_name_ = $4->table_name_ptr_;
    free($4->table_name_ptr_);
    $$->create_info_ = std::move(create_collection_info);
    $$->create_info_->conflict_type_ = $3 ? ConflictType::kIgnore : ConflictType::kError;
    delete $4;
}

/* CREATE TABLE table_name ( column list ); */
| CREATE TABLE if_not_exists table_name '(' table_element_array ')' {
    if(result->IsError()) {
        printf("Error happened, release memory\n");
        if($4->schema_name_ptr_ != nullptr) {
            free($4->schema_name_ptr_);
        }
        free($4->table_name_ptr_);
        delete($4);
        delete($6);
        YYERROR;
    }

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
    if(result->IsError()) {
        free($1);
    }

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

    $$->name_ = $1;
    free($1);
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
};
| IDENTIFIER column_type column_constraints {
    if(result->IsError()) {
        free($1);
        delete($3);
    }

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

    $$->name_ = $1;
    $$->constraints_ = $3;
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
    $$->emplace_back($1);
    free($1);
}
| identifier_array ',' IDENTIFIER {
    $1->emplace_back($3);
    free($3);
    $$ = $1;
};

/*
 * DROP STATEMENT
 */

/* DROP SCHEMA schema_name; */
drop_statement: DROP SCHEMA if_exists IDENTIFIER {
    $$ = new DropStatement();
    UniquePtr<DropSchemaInfo> drop_schema_info = MakeUnique<DropSchemaInfo>();
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
    drop_collection_info->table_name_ = $4->table_name_ptr_;
    free($4->table_name_ptr_);
    $$->drop_info_ = std::move(drop_collection_info);
    $$->drop_info_->conflict_type_ = $3 ? ConflictType::kIgnore : ConflictType::kError;
    delete $4;
};

/*
 * COPY STATEMENT
 */
// COPY schema.table TO file_path WITH (FORMAT csv, DELIMITER ',', HEADER TRUE)
copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')' {
    if(result->IsError()) {
        if($2->schema_name_ptr_ != nullptr) {
            free($2->schema_name_ptr_);
        }
        free($2->table_name_ptr_);
        delete($2);
        free($4);
        delete $7;
        YYERROR;
    }

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
    if(result->IsError()) {
        if($2->schema_name_ptr_ != nullptr) {
            free($2->schema_name_ptr_);
        }
        free($2->table_name_ptr_);
        delete($2);
        free($4);
        delete $7;
        YYERROR;
    }

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
}

/*
 * Misc.
 */

table_name : IDENTIFIER {
    if(!result->IsError()) {
        $$ = new TableName();
        $$->table_name_ptr_ = $1;
    }
}
| IDENTIFIER '.' IDENTIFIER {
    if(!result->IsError()) {
        $$ = new TableName();
        $$->schema_name_ptr_ = $1;
        $$->table_name_ptr_ = $3;
    }
};

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
	fprintf(stderr, "Error: %s, %d\n", msg, llocp->first_column);
}
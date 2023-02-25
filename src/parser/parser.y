
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
    AlterStatement* alter_stmt;
    ShowStatement* show_stmt;
    ExplainStatement* explain_stmt;
    SetStatement* set_stmt;

    Vector<BaseStatement*>* stmt_array;

    TableName* table_name_t;
    CopyOption* copy_option_t;
    Vector<CopyOption*>* copy_option_array;
}

%destructor {
    fprintf(stderr, "destructor statement array\n");
    if (($$) != nullptr) {
        for (auto ptr : *($$)) {
            delete ptr;
        }
        delete ($$);
    }
} <stmt_array>

%destructor { } <table_name_t>

%token <str_value>      IDENTIFIER STRING
%token <double_value>   DOUBLE_VALUE
%token <long_value>     LONG_VALUE
%token <bool_value>     BOOL_VALUE

/* SQL keywords */

%token CREATE SELECT INSERT DROP UPDATE DELETE COPY SET EXPLAIN SHOW ALTER EXECUTE PREPARE DESCRIBE
%token SCHEMA TABLE COLLECTION TABLES
%token IF NOT EXISTS FROM TO WITH DELIMITER FORMAT HEADER

%token NUMBER

/* nonterminal symbol */

%type <stmt_array>        statement_list
%type <base_stmt>         statement
%type <create_stmt>       create_statement
%type <drop_stmt>         drop_statement
%type <copy_stmt>         copy_statement
%type <show_stmt>         show_statement

%type <table_name_t>      table_name
%type <copy_option_array> copy_option_list
%type <copy_option_t>     copy_option

%type <str_value>         file_path


%type <bool_value>   if_not_exists
%type <bool_value>   if_exists

%%

input_pattern : statement_list semicolon {
    for (BaseStatement* stmt : *$1) {
        result->statements_.emplace_back(stmt);
    }
    delete $1;
};

statement_list : statement {
    $1->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    $$ = new std::vector<BaseStatement*>();
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
    std::unique_ptr<CreateSchemaInfo> create_schema_info = std::make_unique<CreateSchemaInfo>();
    create_schema_info->schema_name_ = $4;
    $$->create_info_ = std::move(create_schema_info);
    $$->create_info_->conflict_type_ = $3 ? ConflictType::kIgnore : ConflictType::kError;
}

/* CREATE COLLECTION collection_name; */
| CREATE COLLECTION if_not_exists table_name {
    $$ = new CreateStatement();
    std::unique_ptr<CreateCollectionInfo> create_collection_info = std::make_unique<CreateCollectionInfo>();
    if($4->schema_name_ptr_ != nullptr) {
        create_collection_info->schema_name_ = $4->schema_name_ptr_;
    }
    create_collection_info->table_name_ = $4->table_name_ptr_;
    $$->create_info_ = std::move(create_collection_info);
    $$->create_info_->conflict_type_ = $3 ? ConflictType::kIgnore : ConflictType::kError;
    delete $4;
}

/* CREATE TABLE table_name ( column list ); */
/*
| CREATE TABLE if_not_exists table_name '(' table_elem_commalist ')' {
      $$ = new CreateStatement(kCreateTable);
      $$->ifNotExists = $3;
      $$->schema = $4.schema;
      $$->tableName = $4.name;
      $$->setColumnDefsAndConstraints($6);
      delete $6;
      if (result->errorMsg()) {
        delete $$;
        YYERROR;
      }
}
*/


/*
 * DROP STATEMENT
 */

/* DROP SCHEMA schema_name; */
drop_statement: DROP SCHEMA if_exists IDENTIFIER {
    $$ = new DropStatement();
    std::unique_ptr<DropSchemaInfo> drop_schema_info = std::make_unique<DropSchemaInfo>();
    drop_schema_info->schema_name_ = $4;
    $$->drop_info_ = std::move(drop_schema_info);
    $$->drop_info_->conflict_type_ = $3 ? ConflictType::kIgnore : ConflictType::kError;
};

/* DROP COLLECTION collection_name; */
| DROP COLLECTION if_exists table_name {
    $$ = new DropStatement();
    std::unique_ptr<DropCollectionInfo> drop_collection_info = std::make_unique<DropCollectionInfo>();
    if($4->schema_name_ptr_ != nullptr) {
        drop_collection_info->schema_name_ = $4->schema_name_ptr_;
    }
    drop_collection_info->table_name_ = $4->table_name_ptr_;
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
    }
    $$->table_name_ = $2->table_name_ptr_;
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
    }
    $$->table_name_ = $2->table_name_ptr_;
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
    }
    $$->table_name_ = $2->table_name_ptr_;
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
    } else {
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
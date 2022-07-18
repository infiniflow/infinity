
# Build sql-parser library
set(PARSER_DIR sql-parser)

add_library(
        sqlparser

        ${PARSER_DIR}/src/parser/bison_parser.cpp
        ${PARSER_DIR}/src/parser/flex_lexer.cpp
        ${PARSER_DIR}/src/sql/Expr.cpp
        ${PARSER_DIR}/src/sql/PrepareStatement.cpp
        ${PARSER_DIR}/src/sql/SQLStatement.cpp
        ${PARSER_DIR}/src/sql/statements.cpp
        ${PARSER_DIR}/src/SQLParser.cpp
        ${PARSER_DIR}/src/SQLParserResult.cpp
        ${PARSER_DIR}/src/util/sqlhelper.cpp
)

target_include_directories(
        sqlparser

        INTERFACE
        ${PARSER_DIR}/src
)

target_compile_options(
        sqlparser
        PRIVATE
        -std=c++20
        -O3 -Wno-sign-compare
        -fPIC
)

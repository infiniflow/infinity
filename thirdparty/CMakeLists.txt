
# Build sql-parser
set(PARSER_DIR sql-parser)

add_library(
        sql-parser

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
        sql-parser

        INTERFACE
        ${PARSER_DIR}/src
)

target_compile_options(
        sql-parser
        PRIVATE
        -std=c++20
        -O3 -Wno-sign-compare
        -fPIC
)

# Build cxxopts
add_library(cxxopts INTERFACE)
target_include_directories(cxxopts INTERFACE cxxopts/include)

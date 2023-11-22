cmake_minimum_required(VERSION 3.5)
project("sql_parser")

## Flex/Bison configuration 
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

#set (COMPILE_FLAGS "-Wcounterexamples")

flex_target(LEXER ${CMAKE_CURRENT_SOURCE_DIR}/lexer.l ${CMAKE_CURRENT_SOURCE_DIR}/lexer.cpp DEFINES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/lexer.h)
bison_target(PARSER ${CMAKE_CURRENT_SOURCE_DIR}/parser.y ${CMAKE_CURRENT_SOURCE_DIR}/parser.cpp DEFINES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/parser.h COMPILE_FLAGS "-Wcounterexamples -d -v")

ADD_FLEX_BISON_DEPENDENCY(LEXER PARSER)

flex_target(QueryLexer ${CMAKE_CURRENT_SOURCE_DIR}/query_lexer.l ${CMAKE_CURRENT_SOURCE_DIR}/query_lexer.cpp DEFINES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/query_lexer.h)
bison_target(QueryParser ${CMAKE_CURRENT_SOURCE_DIR}/query_parser.y ${CMAKE_CURRENT_SOURCE_DIR}/query_parser.cpp DEFINES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/query_parser.h COMPILE_FLAGS "-Wcounterexamples -d -v")

ADD_FLEX_BISON_DEPENDENCY(QueryLexer QueryParser)

file(GLOB_RECURSE
        parser_files
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

list(FILTER parser_files EXCLUDE REGEX .*main.cpp)

add_library(sql_parser STATIC ${parser_files} ${BISON_parser_OUTPUTS} ${FLEX_lexer_OUTPUTS} ${BISON_QueryParser_OUTPUTS} ${FLEX_QueryLexer_OUTPUTS})

target_include_directories(sql_parser PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(sql_parser PUBLIC "${CMAKE_SOURCE_DIR}/third_party/spdlog/include")
target_include_directories(sql_parser PUBLIC "${CMAKE_SOURCE_DIR}/third_party/nlohmann")
target_include_directories(sql_parser PUBLIC "${CMAKE_SOURCE_DIR}/third_party/iresearch/core")
target_include_directories(sql_parser PUBLIC "${CMAKE_SOURCE_DIR}/third_party/iresearch/external")
target_include_directories(sql_parser PUBLIC "${CMAKE_SOURCE_DIR}/third_party/iresearch/external/frozen/include")
target_include_directories(sql_parser PUBLIC "${CMAKE_SOURCE_DIR}/third_party/parallel-hashmap")

cmake_minimum_required(VERSION 3.5)
project("sql_parser")

## Flex/Bison configuration 
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

#set (COMPILE_FLAGS "-Wcounterexamples")

flex_target(LEXER ${CMAKE_CURRENT_SOURCE_DIR}/lexer.l ${CMAKE_CURRENT_SOURCE_DIR}/lexer.cpp DEFINES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/lexer.h)
bison_target(PARSER ${CMAKE_CURRENT_SOURCE_DIR}/parser.y ${CMAKE_CURRENT_SOURCE_DIR}/parser.cpp DEFINES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/parser.h COMPILE_FLAGS "-Wcounterexamples -d -v")

ADD_FLEX_BISON_DEPENDENCY(LEXER PARSER)

file(GLOB_RECURSE
        parser_files
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(sql_parser STATIC ${parser_files} ${BISON_PARSER_OUTPUTS} ${FLEX_LEXER_OUTPUTS})

target_include_directories(sql_parser PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(sql_parser PUBLIC "${CMAKE_SOURCE_DIR}/third_party/spdlog/include")

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

flex_target(LEXER
            ${CMAKE_CURRENT_SOURCE_DIR}/parser/lexer.l
            ${CMAKE_CURRENT_SOURCE_DIR}/parser/lexer.cpp
            DEFINES_FILE
            ${CMAKE_CURRENT_SOURCE_DIR}/parser/lexer.h)
bison_target(PARSER
            ${CMAKE_CURRENT_SOURCE_DIR}/parser/parser.y
            ${CMAKE_CURRENT_SOURCE_DIR}/parser/parser.cpp
            DEFINES_FILE
            ${CMAKE_CURRENT_SOURCE_DIR}/parser/parser.h
            COMPILE_FLAGS "-d -v -Wcounterexamples")

ADD_FLEX_BISON_DEPENDENCY(LEXER PARSER)

file(GLOB_RECURSE
        infinity_main_src
        CONFIGURE_DEPENDS
        bin/infinity_main.cpp
        )

file(GLOB_RECURSE
        infinity_console_src
        CONFIGURE_DEPENDS
        bin/infinity_console.cpp
        )

file(GLOB_RECURSE
        console_src
        CONFIGURE_DEPENDS
        bin/console.cpp
        bin/console.h
        )

file(GLOB_RECURSE
        common_src
        CONFIGURE_DEPENDS
        common/*.cpp
        common/*.h
        common/utility/*.cpp
        common/utility/*.h
        )

file(GLOB_RECURSE
        executor_src
        CONFIGURE_DEPENDS
        executor/*.cpp
        executor/*.h
        )

file(GLOB_RECURSE
        expression_src
        CONFIGURE_DEPENDS
        expression/*.cpp
        expression/*.h
        )

file(GLOB_RECURSE
        function_src
        CONFIGURE_DEPENDS
        function/*.cpp
        function/*.h
        )

file(GLOB_RECURSE
        network_src
        CONFIGURE_DEPENDS
        network/*.cpp
        )

file(GLOB_RECURSE
        planner_src
        CONFIGURE_DEPENDS
        planner/*.cpp
        )

file(GLOB_RECURSE
        infinity_src
        CONFIGURE_DEPENDS
        main/*.cpp
        )

file(GLOB_RECURSE
        legacy_sched_src
        CONFIGURE_DEPENDS
        legacy_sched/*.cpp
)

file(GLOB_RECURSE
        scheduler_src
        CONFIGURE_DEPENDS
        scheduler/*.cpp
        )

file(GLOB_RECURSE
        storage_src
        CONFIGURE_DEPENDS
        storage/*.cpp
        )

file(GLOB_RECURSE
        parser_src
        CONFIGURE_DEPENDS
        parser/*.cpp
        parser/*.h
        )

file(GLOB_RECURSE
        third_party_src
        CONFIGURE_DEPENDS
        ../third_party/simdjson/*.cpp
        ../third_party/simdjson/*.h
        ../third_party/nlohmann/*.h
        ../third_party/tlx/*.cpp
        )

add_library(infinity_parser STATIC
            ${parser_src}
            ${BISON_PARSER_OUTPUTS}
            ${FLEX_LEXER_OUTPUTS})

target_include_directories(infinity_parser PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_include_directories(infinity_parser PUBLIC "/usr/local/include")
#target_include_directories(infinity_parser PUBLIC "${CMAKE_SOURCE_DIR}/third_party/tlsf")
target_include_directories(infinity_parser PUBLIC "${CMAKE_SOURCE_DIR}/third_party/spdlog/include")
#target_include_directories(infinity_parser PUBLIC "${CMAKE_SOURCE_DIR}/third_party/magic_enum/include")
target_include_directories(infinity_parser PUBLIC "${CMAKE_SOURCE_DIR}/third_party/ctpl")
target_include_directories(infinity_parser PUBLIC "${CMAKE_SOURCE_DIR}/third_party/nlohmann")

# Configure the version file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/compilation_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/bin/compilation_config.h)

set(INFINITY_CORE_SOURCE
        ${console_src}
        ${common_src}
        ${executor_src}
        ${expression_src}
        ${function_src}
        ${infinity_src}
        ${network_src}
        ${parser_src}
        ${planner_src}
        ${legacy_sched_src}
        ${scheduler_src}
        ${storage_src}
        ${third_party_src}
)

add_library(infinity_core STATIC ${INFINITY_CORE_SOURCE})
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_include_directories(infinity_core PUBLIC "/usr/local/include")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/tlsf")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/spdlog/include")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/magic_enum/include")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/croaring/include")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/ctpl")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/concurrentqueue")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/simdjson")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/nlohmann")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/fastpfor")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/tomlplusplus")
#set_property(TARGET infinity_core PROPERTY CXX_INCLUDE_WHAT_YOU_USE /usr/bin/iwyu)

add_executable(infinity_main
        ${infinity_main_src}
        )
target_include_directories(infinity_main PUBLIC "${CMAKE_SOURCE_DIR}/third_party/cxxopts/include")
#set_property(TARGET infinity_main PROPERTY CXX_INCLUDE_WHAT_YOU_USE /usr/bin/iwyu)

add_executable(infinity_console
        ${infinity_console_src}
        )
target_include_directories(infinity_console PUBLIC "${CMAKE_SOURCE_DIR}/third_party/linenoise")

target_link_libraries(
        infinity_main
        infinity_core
        infinity_parser
        tlsf
        roaring
        faiss
        fastpfor
        zsv_parser
        ${EXTERNAL_LIBS}
)

target_link_libraries(
        infinity_console
        infinity_core
        infinity_parser
        tlsf
        linenoise
        roaring
        fastpfor
        zsv_parser
        ${EXTERNAL_LIBS}
)
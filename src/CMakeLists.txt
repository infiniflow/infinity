#add_subdirectory(bin)

# Configure the version file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/compilation_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/bin/compilation_config.h)

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
        parser_src
        CONFIGURE_DEPENDS
        parser/*.cpp
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
        scheduler_src
        CONFIGURE_DEPENDS
        scheduler/*.cpp
        )

file(GLOB_RECURSE
        storage_src
        CONFIGURE_DEPENDS
        storage/*.cpp
        )

set(INFINITY_CORE_SOURCE
        ${common_src}
        ${executor_src}
        ${expression_src}
        ${function_src}
        ${infinity_src}
        ${network_src}
        ${parser_src}
        ${planner_src}
        ${scheduler_src}
        ${storage_src}
)

add_library(infinity_core STATIC ${INFINITY_CORE_SOURCE})

add_executable(infinity_main
        ${infinity_main_src}
        )

add_executable(infinity_console
        ${infinity_console_src}
        )

target_link_libraries(
        infinity_main
        infinity_core
        sql-parser
)

target_link_libraries(
        infinity_console
        infinity_core
        sql-parser
        linenoise
)
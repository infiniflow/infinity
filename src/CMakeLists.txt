#add_subdirectory(bin)
file(GLOB_RECURSE
        bin_src
        CONFIGURE_DEPENDS
        bin/*.cpp
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
        scheduler_src
        CONFIGURE_DEPENDS
        scheduler/*.cpp
        )

file(GLOB_RECURSE
        storage_src
        CONFIGURE_DEPENDS
        storage/*.cpp
        )

set(
        INFINITY_CORE_SOURCE
        ${common_src}
        ${executor_src}
        ${expression_src}
        ${infinity_src}
        ${network_src}
        ${planner_src}
        ${scheduler_src}
        ${storage_src}
)

add_executable(infinity_main
        ${bin_src}
        ${INFINITY_CORE_SOURCE}
        )

target_link_libraries(
        infinity_main
        sql-parser
        #        pqxx_static
)
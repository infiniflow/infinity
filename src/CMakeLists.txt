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

add_executable(infinity_main
        ${bin_src}
        ${common_src}
        ${infinity_src}
        ${network_src}
        ${planner_src}
        )

target_link_libraries(
        infinity_main
        sql-parser
        #        pqxx_static
)
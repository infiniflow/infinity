#add_subdirectory(bin)
file(GLOB_RECURSE
        infinity_main_src
        CONFIGURE_DEPENDS
        bin/*.cpp
        )

file(GLOB_RECURSE
        network_src
        CONFIGURE_DEPENDS
        network/*.cpp
        )

add_executable(infinity_main
        ${infinity_main_src}
        ${network_src})

target_link_libraries(
        infinity_main
        sql-parser
        hv
        #        pqxx_static
)
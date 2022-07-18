add_executable(infinity_server server.cpp)

target_link_libraries(
        infinity_server
        sqlparser
#        pqxx_static
)
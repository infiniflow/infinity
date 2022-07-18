add_executable(infinity_server server.cpp)

target_link_libraries(
        infinity_server
        sql-parser
#        pqxx_static
)
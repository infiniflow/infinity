
add_executable(asio_wal
        asio_wal.cpp
)

target_link_libraries(asio_wal
        uring
        spdlog
)

target_include_directories(asio_wal
        PUBLIC "${CMAKE_SOURCE_DIR}/third_party/third_party/spdlog/include")

target_compile_options(asio_wal PUBLIC -DBOOST_ASIO_HAS_FILE -DBOOST_ASIO_HAS_IO_URING)

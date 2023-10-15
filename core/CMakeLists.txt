
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/compilation_config.cppm.in ${CMAKE_CURRENT_SOURCE_DIR}/bin/compilation_config.cppm)

file(GLOB_RECURSE
        main_cpp
        CONFIGURE_DEPENDS
        bin/infinity_main.cpp
)

file(GLOB_RECURSE
        main_cppm
        CONFIGURE_DEPENDS
        bin/*.cppm
)

file(GLOB_RECURSE
        infinity_cpp
        CONFIGURE_DEPENDS
        main/*.cpp
)

file(GLOB_RECURSE
        infinity_cppm
        CONFIGURE_DEPENDS
        main/*.cppm
)

file(GLOB_RECURSE
        network_cpp
        CONFIGURE_DEPENDS
        network/*.cpp
)

file(GLOB_RECURSE
        network_cppm
        CONFIGURE_DEPENDS
        network/*.cppm
)

file(GLOB_RECURSE
        common_cppm
        CONFIGURE_DEPENDS
        common/*.cppm
)


add_executable(infinity
        ${main_cpp}
        ${infinity_cpp}
        ${network_cpp}
)

target_sources(infinity
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        ${main_cppm}
        ${infinity_cppm}
        ${network_cppm}
        ${common_cppm}
)

target_include_directories(infinity PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(infinity PUBLIC "${CMAKE_SOURCE_DIR}/third_party/spdlog/include")
target_include_directories(infinity PUBLIC "${CMAKE_SOURCE_DIR}/third_party/cxxopts/include")
target_include_directories(infinity PUBLIC "${CMAKE_SOURCE_DIR}/third_party/ctpl")
target_include_directories(infinity PUBLIC "${CMAKE_SOURCE_DIR}/third_party/tomlplusplus")

